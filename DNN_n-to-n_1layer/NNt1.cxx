
// NNt1 ver. 3.02b (27/03/2023)
//
// Copyright (C) Gianluca Pollastri 2023

#include "NNt1.h"

NNt1::NNt1(istream &is) {
	is >> NO >> NI >> NIr >> which >> outp >> inp;
	upper = new Layer_tanh(is);
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

void NNt1::read(istream &is) {
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

void NNt1::forward(int *I) {
	upper->forward(I);
}

void NNt1::forward(double *I) {
	upper->forward(I);
}

void NNt1::forward(int *I1, double *I2) {
	upper->forward(I1, I2);
}

void NNt1::forward(double *I1, double *I2) {
	upper->forward(I1, I2);
}

double
NNt1::backward(double *t, double weight) {
	double err = upper->backward(t, weight);
	if (inp == 1)
		for (int r = NItot; r < NItot + NIr; r++)
			backprop[r] = upper->back_out()[r];
	else if (inp == 2)
		for (int r = 0; r < NItot + NIr; r++)
			backprop[r] = upper->back_out()[r];
	return err;
}

void NNt1::gradient(int *I, double *t) {
	upper->gradient(I);
}
void NNt1::gradient(double *I, double *t) {
	upper->gradient(I);
}
void NNt1::gradient(int *I1, double *I2, double *t) {
	upper->gradient(I1, I2);
}
void NNt1::gradient(double *I1, double *I2, double *t) {
	upper->gradient(I1, I2);
}

void NNt1::resetGradient() {
	upper->resetGradient();
}

void NNt1::updateWeights(double epsilon) {
	upper->updateWeights(epsilon);
}

void NNt1::updateWeightsL1(double epsilon) {
	upper->updateWeightsL1(epsilon);
}

void NNt1::updateWeightsClipped(double epsilon) {
	upper->updateWeightsClipped(epsilon);
}

void NNt1::initWeights(int seed) {
	upper->initWeights(seed);
}

void NNt1::write(ostream &os) {
	os << NO << " " << NI << " " << NIr << " ";
	os << which << " " << outp << " " << inp << "\n";
	upper->write(os);
}
