/*
 * CIndividual.h
 *
 *  Created on: Dec 20, 2013
 *      Author: bkloppenborg
 */

#ifndef CINDIVIDUAL_H_
#define CINDIVIDUAL_H_

#include <vector>
#include <memory>
using namespace std;

class CIndividual;
typedef shared_ptr<CIndividual> CIndividualPtr;

class CIndividual
{
public:
	vector<float> image;
	float chi2r;

public:
	CIndividual(unsigned int image_size);
	virtual ~CIndividual();

	static CIndividualPtr Breed(CIndividualPtr A, CIndividualPtr B);
	static void Mutate(CIndividualPtr A);

	void RandomImage();
};

#endif /* CINDIVIDUAL_H_ */
