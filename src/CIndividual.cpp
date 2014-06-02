/*
 * CIndividual.cpp
 *
 *  Created on: Dec 20, 2013
 *      Author: bkloppenborg
 */

#include "CIndividual.h"
#include <cassert>
#include <random>
using namespace std;

CIndividual::CIndividual(unsigned int image_size)
{
	image.resize(image_size);
	chi2r = 0;
}

CIndividual::~CIndividual()
{
	// TODO Auto-generated destructor stub
}

CIndividualPtr CIndividual::Breed(CIndividualPtr A, CIndividualPtr B)
{
	assert(A->image.size() == B->image.size());
	unsigned int image_size = A->image.size();
	// create a new child
	CIndividualPtr child = CIndividualPtr(new CIndividual(image_size));
	for(unsigned int i = 0; i < image_size; i++)
	{
		// average the images
		child->image[i] = (A->image[i] + B->image[i]) / 2;
	}

	return child;
}

void CIndividual::Mutate(CIndividualPtr A)
{
	unsigned int image_size = A->image.size();

	std::default_random_engine pixel_generator;
	std::uniform_int_distribution<int> pixel_distribution(0,image_size);

	// swap flux
	unsigned int n_mutations = 0.05 * image_size;
	double flux_temp;
	unsigned int pixel_a, pixel_b;
	for(int i = 0; i < n_mutations; i++)
	{
		pixel_a = pixel_distribution(pixel_generator);
		pixel_b = pixel_distribution(pixel_generator);

		flux_temp = A->image[pixel_a];
		A->image[pixel_a] = A->image[pixel_b];
		A->image[pixel_b] = flux_temp;
	}
}


void CIndividual::RandomImage()
{
	unsigned int n_types = 2;
	// Init the array values randomly:
	std::random_device rd;
	std::mt19937 e2(rd());

	std::uniform_int_distribution<> image_type_chooser(0, n_types);
	unsigned int image_type_id = image_type_chooser(e2);

	std::uniform_real_distribution<> real_dist(0, 1);

	// Information about the image
	unsigned int image_width = sqrt(image.size());

	if(image_type_id == 1)	// point source
	{
		image[image.size() / 2] = 1.0;
	}
	else if(image_type_id == 2)	// Uniform disk of some random size.
	{
		double radius = real_dist(e2) * image_width / 2;

		// Account for the image indexing by shifting by one pixel:
		double center_col = double(image_width - 1) / 2;
		double center_row = double(image_width - 1) / 2;
		double rad_sq = radius * radius;
		double dx = 0;
		double dy = 0;

		for(unsigned int row = 0; row < image_width; row++)
		{
			dy = row - center_row;
			for(unsigned int col = 0; col < image_width; col++)
			{
				dx = col - center_col;

				if(dx*dx + dy*dy < rad_sq)
					image[image_width * row + col] = 1;
				else
					image[image_width * row + col] = 0;
			}
		}
	}
	else						// Fully randomized image
	{
		for(unsigned int i = 0; i < image.size(); i++)
			image[i] = real_dist(e2);
	}

}
