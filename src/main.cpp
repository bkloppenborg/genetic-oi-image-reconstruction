/*
 * main.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <memory>
#include <list>
using namespace std;

#include "main.h"
#include "liboi.hpp"
using namespace liboi;
#include "CIndividual.h"

extern string EXE_FOLDER;

bool best_fit_individuals (CIndividualPtr A, CIndividualPtr B) { return (A->chi2r < B->chi2r); }

// The main routine.
int main(int argc, char *argv[])
{
    // determine the absolute directory from which directory this program is running
    EXE_FOLDER = "/home/bkloppenborg/workspace/genetic-oi-image-reconstruction/bin/";

    unsigned int image_width = 128;
    unsigned int image_height = 128;
    unsigned int image_size = image_width * image_height;
    float image_scale = 0.025;

    // init liboi
    CLibOI liboi(CL_DEVICE_TYPE_GPU);
	liboi.SetKernelSourcePath(EXE_FOLDER + "/kernels/");
	liboi.SetImageInfo(image_width, image_height, 1, image_scale);

	cout << "Loading data files." << endl;
    string datafile = "/home/bkloppenborg/Projects/epsAur-Interferometry/data/CHARA-MIRC/eps_Aur.2009-11.AVG15m.oifits";
    liboi.LoadData(datafile);

	// Init the device.
    float * image_liboi_bug = 0;
   	liboi.SetImageSource(image_liboi_bug);	// liboi bug 40, must define storage location as host prior to Init being called.
	cout << "Building kernels." << endl;
	liboi.Init();

    unsigned int n_children = 50;
	// Init the array values randomly:
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_int_distribution<> population_random_dist(0, n_children);

	double breeding_sigma = n_children / 5.0;
	std::normal_distribution<double> breeding_normal_dist(0, breeding_sigma);

    // Init the population
    vector<CIndividualPtr> population;
    for(int i = 0; i < n_children; i++)
    {
    	CIndividualPtr temp = CIndividualPtr(new CIndividual(image_size));
    	temp->RandomImage();
    	population.push_back(temp);
    }

    float * image;
    for(unsigned int n_iterations = 0; n_iterations < 100; n_iterations++)
    {
		cout << "Computing chi2r for population." << endl;
		// Compute their chi2r
		for(auto individual: population)
		{
			individual->chi2r = 0;

			// set the image
			image = &(individual->image[0]);
			liboi.SetImageSource(image);
			liboi.CopyImageToBuffer(0);

			// Copute the chi2r. Because we only have one data set, we only do it on the 0th item.
			individual->chi2r = liboi.ImageToChi2(0);
		}

		// Sort The population in ascending order. Most fit individuals have the lowest
		// chi2r values.
		std::sort(population.begin(), population.end(), best_fit_individuals);
		cout << "Best: " << population.front()->chi2r << " " << population.front() << " ";
		cout << "Worst: " << population.back()->chi2r << endl;

		float * image = &(population.front()->image[0]);
		liboi.SetImageSource(image);
		liboi.CopyImageToBuffer(0);
		liboi.ExportImage("!/tmp/best_individual.fits");

		// Breed new children
		unsigned int keep = 5;
		unsigned int individual_a;
		unsigned int individual_b;
		for(unsigned int i = 0; i < n_children; i++)
		{
			individual_a = int(floor(abs(breeding_normal_dist(e2))));	// better individuals breed more often
			individual_b = population_random_dist(e2);	// anyone in the population

			CIndividualPtr A = population[individual_a];
			CIndividualPtr B = population[individual_b];

			CIndividualPtr C = CIndividual::Breed(A,B);
			CIndividual::Mutate(C);
			population.push_back(C);
		}

		// Erase all but the best individuals and the newly generated children.
		population.erase(population.begin() + keep, population.begin() + n_children +  keep);
    }

}


#endif /* MAIN_CPP_ */
