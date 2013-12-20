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

    unsigned int n_children = 20;
	// Init the array values randomly:
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_int_distribution<> dist(0, n_children);

    // Init the population
    vector<CIndividualPtr> population;
    for(int i = 0; i < n_children; i++)
    {
    	CIndividualPtr temp = CIndividualPtr(new CIndividual(image_size));
    	temp->RandomImage();
    	population.push_back(temp);
    }

    for(unsigned int n_iterations = 0; n_iterations < 100; n_iterations++)
    {
		cout << "Computing chi2r for population." << endl;
		// Compute their chi2r
		for(auto individual: population)
		{
			individual->chi2r = 0;

			// set the image
			float * image = &(individual->image[0]);
			liboi.SetImageSource(image);
			liboi.CopyImageToBuffer(0);

			// Copute the chi2r. Because we only have one data set, we only do it on the 0th item.
			individual->chi2r = liboi.ImageToChi2(0);
		}

		// Sort The population in ascending order. Most fit individuals have the lowest
		// chi2r values.
		std::sort(population.begin(), population.end(), best_fit_individuals);
		cout << "Best individual: " << population.front()->chi2r << endl;
		cout << "Worst individual: " << population.back()->chi2r << endl;

		// Now breed the individuals
		for(unsigned int i = 0; i < n_children; i++)
		{
			CIndividualPtr A = population[dist(e2) / 2];
			CIndividualPtr B = population[dist(e2) / 2];

			CIndividualPtr C = CIndividual::Breed(A,B);
			CIndividual::Mutate(C);
			population.push_back(C);
		}

		// remove parents from the population
		population.erase(population.begin(), population.begin() + n_children);
    }

}


#endif /* MAIN_CPP_ */
