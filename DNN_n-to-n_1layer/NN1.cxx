
// NN1 ver. 3.02b (27/03/2023)
//
// Copyright (C) Gianluca Pollastri 2023

#include "NN1.h"

NN1::NN1(istream &is) {
	is >> NO >> NI >> NIr >> which >> outp >> inp;
	upper = new Layer_soft(is);
	if (outp)
		upper->set_output(1);
	upper->set_ninput(inp);

	int i;
	NK = new int[NI];
	NItot = 0;
	for (i = 0; i < NI; i++) {
		NK[i] = upper->get_NK()[i];
		NItot += NK[i];
	}
	NK2 = new int[NIr];
	for (i = 0; i < NIr; i++)
		NK2[i] = upper->get_NK()[NI + i];
	backprop = new double[NItot + NIr];
}

void NN1::read(istream &is) {
	is >> NO >> NI >> NIr >> which >> outp >> inp;
	upper->read(is);
	if (outp)
		upper->set_output(1);
	upper->set_ninput(inp);

	int i;
	NItot = 0;
	for (i = 0; i < NI; i++) {
		NK[i] = upper->get_NK()[i];
		NItot += NK[i];
	}
	for (i = 0; i < NIr; i++)
		NK2[i] = upper->get_NK()[NI + i];
}

void NN1::forward(int *I) {
	upper->forward(I);
}

void NN1::forward(double *I) {
	upper->forward(I);
}

void NN1::forward(int *I1, double *I2) {
	upper->forward(I1, I2);
}

void NN1::forward(double *I1, double *I2) {
	upper->forward(I1, I2);
}

double
NN1::backward(double *t, double weight) {
	double err = upper->backward(t, weight);
	if (inp == 1)
		for (int r = NItot; r < NItot + NIr; r++)
			backprop[r] = upper->back_out()[r];
	else if (inp == 2)
		for (int r = 0; r < NItot + NIr; r++)
			backprop[r] = upper->back_out()[r];
	return err;
}

void NN1::gradient(int *I, double *t) {
	upper->gradient(I);
}
void NN1::gradient(double *I, double *t) {
	upper->gradient(I);
}
void NN1::gradient(int *I1, double *I2, double *t) {
	upper->gradient(I1, I2);
}
void NN1::gradient(double *I1, double *I2, double *t) {
	upper->gradient(I1, I2);
}

void NN1::resetGradient() {
	upper->resetGradient();
}

void NN1::updateWeights(double epsilon) {
	upper->updateWeights(epsilon);
}

void NN1::updateWeightsL1(double epsilon) {
	upper->updateWeightsL1(epsilon);
}

void NN1::updateWeightsClipped(double epsilon) {
	upper->updateWeightsClipped(epsilon);
}

void NN1::initWeights(int seed) {
	upper->initWeights(seed);
}

void NN1::write(ostream &os) {
	os << NO << " " << NI << " " << NIr << " ";
	os << which << " " << outp << " " << inp << "\n";
	upper->write(os);
}
