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

void CIndividual:: Mutate(CIndividualPtr A)
{
	unsigned int image_size = A->image.size();

	std::default_random_engine pixel_generator;
	std::uniform_int_distribution<int> pixel_distribution(0,image_size);

	// Init the array values randomly:
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<> dist(0, 1);

	unsigned int n_mutations = 10;
	unsigned int pixel;
	float flux;
	for(int i = 0; i < n_mutations; i++)
	{
		pixel = pixel_distribution(pixel_generator);
		flux = dist(e2);
		A->image[pixel] = flux;
	}
}

void CIndividual::RandomImage()
{
	// Init the array values randomly:
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<> dist(0, 1);

	for(unsigned int i = 0; i < image.size(); i++)
		image[i] = dist(e2);
}
