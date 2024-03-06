
// DNN1 ver. 0.01 (23/03/2023)
// Copyright (C) Gianluca Pollastri 2023

#include "DNN1.h"

void DNN1::alloc() {
	int t;
	Hcache = new double **[NLayers + 1];
	Hcachebp = new double **[NLayers + 1];
	for (int l = 0; l < NLayers; l++) {
		Hcache[l] = new double *[MAX];
		Hcachebp[l] = new double *[MAX];
		for (int t = 0; t < MAX; t++) {
			Hcache[l][t] = new double[Ht[l]];
			Hcachebp[l][t] = new double[Ht[l]];
			memset(Hcache[l][t], 0, Ht[l] * sizeof(double));
			memset(Hcachebp[l][t], 0, Ht[l] * sizeof(double));
		}
	}

	// Top cache double the size to (redundantly) accommodate the global pooling input
	Hcache[NLayers] = new double *[MAX];
	Hcachebp[NLayers] = new double *[MAX];
	for (int t = 0; t < MAX; t++) {
		Hcache[NLayers][t] = new double[2 * Ht[NLayers]];
		Hcachebp[NLayers][t] = new double[2 * Ht[NLayers]];
		memset(Hcache[NLayers][t], 0, 2 * Ht[NLayers] * sizeof(double));
		memset(Hcachebp[NLayers][t], 0, 2 * Ht[NLayers] * sizeof(double));
	}

	HcacheGlobal = new double[Ht[NLayers]];
	HcacheGlobalbp = new double[Ht[NLayers]];
	memset(HcacheGlobal, 0, Ht[NLayers] * sizeof(double));
	memset(HcacheGlobalbp, 0, Ht[NLayers] * sizeof(double));

	Y = new double[MAX * NY];
	BP = new double[MAX * NU];
}

DNN1::DNN1(int the_NU, int the_NY, int the_NLayers, int *the_Ht, int the_NHi, int the_Hi, int the_gamma, int the_context, int the_doubleo) : NU(the_NU), NY(the_NY), NLayers(the_NLayers), NHi(the_NHi), Hi(the_Hi), gamma(the_gamma), context(the_context), doubleo(the_doubleo) {

	int NK[8196];
	int NKgamma[8196];

	for (int c = 0; c < 2 * context + 1; c++) {
		NK[c] = NU;
	}
	for (int c = 0; c < 8196; c++) {
		NKgamma[c] = 1;
	}

	Ht = new int[NLayers + 1];
	Ht[0] = Hi;
	for (int c = 0; c < NLayers; c++) {
		Ht[c + 1] = the_Ht[c];
	}

	NetO = new NN1(0, 2 * Ht[NLayers], NY, NK);
	NetO->set_input(2);
	NetO->set_output(1);

	NetT = new NNt1 *[NLayers];

	for (int l = 0; l < NLayers; l++) {
		NetT[l] = new NNt1(0, (2 * gamma + 1) * Ht[l], Ht[l + 1], NKgamma, 0);
		NetT[l]->set_input(2);
		NetT[l]->set_output(0);
	}

	NetI = new NNt((2 * context + 1), 0, NHi, Hi, NK, 0);
	NetI->set_input(0);
	NetI->set_output(0);

	alloc();
	resetGradient();
}

DNN1::DNN1(istream &is, int the_NU, int the_NY, int the_NLayers, int *the_Ht, int the_NHi, int the_Hi, int the_gamma, int the_context, int the_doubleo) : NU(the_NU), NY(the_NY), NLayers(the_NLayers), NHi(the_NHi), Hi(the_Hi), gamma(the_gamma), context(the_context), doubleo(the_doubleo) {

	int NK[8196];
	int NKgamma[8196];

	for (int c = 0; c < 2 * context + 1; c++) {
		NK[c] = NU;
	}
	for (int c = 0; c < 8196; c++) {
		NKgamma[c] = 1;
	}

	Ht = new int[NLayers + 1];
	Ht[0] = Hi;
	for (int c = 0; c < NLayers; c++) {
		Ht[c + 1] = the_Ht[c];
	}
	NetT = new NNt1 *[NLayers];

	int dummy;
	double fdummy;

	is >> dummy >> dummy;
	is >> dummy >> dummy >> dummy;
	for (int c = 0; c < NLayers - 1; c++) {
		is >> dummy;
	}
	is >> dummy >> dummy >> fdummy;

	NN1 *dummynet = new NN1(is);

	cout << dummynet->get_NI() << " " << dummynet->get_NIr() << " "
		 << " " << dummynet->get_NO() << "\n"
		 << flush;

	delete dummynet;

	for (int l = 0; l < NLayers - 1; l++) {
		NetT[l] = new NNt1(is);
		NetT[l]->set_input(2);
		NetT[l]->set_output(0);
	}
	// NNt* dummynet2 = new NNt(is);
	// delete dummynet2;

	NetI = new NNt(is);
	NetI->set_input(0);
	NetI->set_output(0);

	NetO = new NN1(0, 2 * Ht[NLayers], NY, NK);
	NetO->set_input(2);
	NetO->set_output(1);

	NetT[NLayers - 1] = new NNt1(0, (2 * gamma + 1) * Ht[NLayers - 1], Ht[NLayers], NKgamma, 0);
	NetT[NLayers - 1]->set_input(2);
	NetT[NLayers - 1]->set_output(0);

	alloc();
	resetGradient();
}

DNN1::DNN1(istream &is) {
	is >> NU >> NY >> NLayers;

	NetT = new NNt1 *[NLayers];
	Ht = new int[NLayers + 1];

	is >> NHi >> Hi;
	Ht[0] = Hi;

	for (int c = 0; c < NLayers; c++) {
		is >> Ht[c + 1];
	}
	is >> gamma >> context >> doubleo;

	NetO = new NN1(is);
	NetO->set_input(2);
	NetO->set_output(1);

	for (int l = 0; l < NLayers; l++) {
		NetT[l] = new NNt1(is);
		NetT[l]->set_input(2);
		NetT[l]->set_output(0);
	}
	NetI = new NNt(is);
	NetI->set_input(0);
	NetI->set_output(0);

	alloc();
	resetGradient();
}

void DNN1::read(istream &is) {
	is >> NU >> NY >> NLayers;
	is >> NHi >> Hi;
	Ht[0] = Hi;
	for (int c = 0; c < NLayers; c++) {
		is >> Ht[c + 1];
	}
	is >> gamma >> context >> doubleo;

	NetO->read(is);
	NetO->set_input(2);
	NetO->set_output(1);

	for (int l = 0; l < NLayers; l++) {
		NetT[l]->read(is);
		NetT[l]->set_input(2);
		NetT[l]->set_output(0);
	}

	NetI->read(is);
	NetI->set_input(0);
	NetI->set_output(0);
	memset(HcacheGlobal, 0, Ht[NLayers] * sizeof(double));
}

void DNN1::write(ostream &os) {
	os << NU << " " << NY << " " << NLayers << "\n";
	os << NHi << " " << Hi << "\n";

	for (int c = 0; c < NLayers; c++) {
		os << Ht[c + 1] << " ";
	}
	os << "\n";
	os << gamma << " " << context << " " << doubleo << "\n";

	NetO->write(os);
	for (int l = 0; l < NLayers; l++) {
		NetT[l]->write(os);
	}
	NetI->write(os);
}

void DNN1::resetGradient() {
	NetO->resetGradient();
	for (int l = 0; l < NLayers; l++) {
		NetT[l]->resetGradient();
	}
	NetI->resetGradient();
}

void DNN1::initWeights(int seed) {
	NetO->initWeights(seed++);
	for (int l = 0; l < NLayers; l++) {
		NetT[l]->initWeights(seed++);
	}
	NetI->initWeights(seed++);
}

void DNN1::initWeights_top(int seed) {
	NetO->initWeights(seed++);
	NetT[NLayers - 1]->initWeights(seed++);
}

void DNN1::I_T(double *seq, int t, int length) {
	double I[8196];
	int c, h;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = 0.0;
		} else {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = seq[NU * (t + c) + i];
		}
	}
	double *X;
	NNt *tempo = new NNt(NetI);
	tempo->forward(I, X);

	for (h = 0; h < Hi; h++) {
		Hcache[0][t][h] = tempo->out()[h];
	}
	delete tempo;
}

void DNN1::I_T(double *seq, int t1, int t2, int length) {
	double I[8196];
	int c, h;
	double *X;
	NNt *tempo = new NNt(NetI);

	for (int t = t1; t < t2; t++) {
		for (c = -context; c <= context; c++) {
			if (t + c <= 0 || t + c > length) {
				for (int i = 0; i < NU; i++)
					I[NU * (context + c) + i] = 0.0;
			} else {
				for (int i = 0; i < NU; i++)
					I[NU * (context + c) + i] = seq[NU * (t + c) + i];
			}
		}
		tempo->forward(I, X);

		for (h = 0; h < Hi; h++) {
			Hcache[0][t][h] = tempo->out()[h];
		}
	}
	delete tempo;
}

void DNN1::I_T(int *seq, int t, int length) {
	int I[8196];
	int c, h;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			I[context + c] = -1;
		} else {
			I[context + c] = seq[t + c];
		}
	}
	double *X;
	NNt *tempo = new NNt(NetI);
	tempo->forward(I, X);

	for (h = 0; h < Hi; h++) {
		Hcache[0][t][h] = tempo->out()[h];
	}
	delete tempo;
}

void DNN1::I_T(int *seq, int t1, int t2, int length) {
	int I[8196];
	int c, h;
	double *X;
	NNt *tempo = new NNt(NetI);

	for (int t = t1; t < t2; t++) {
		for (c = -context; c <= context; c++) {
			if (t + c <= 0 || t + c > length) {
				I[context + c] = -1;
			} else {
				I[context + c] = seq[t + c];
			}
		}
		tempo->forward(I, X);

		for (h = 0; h < Hi; h++) {
			Hcache[0][t][h] = tempo->out()[h];
		}
	}

	delete tempo;
}

void DNN1::T_T(int Lay, int t, int length) {
	double I[8196];
	int c, h;

	for (c = -gamma; c <= gamma; c++) {
		if (t + c <= 0 || t + c > length) {
			for (h = 0; h < Ht[Lay]; h++)
				I[Ht[Lay] * (gamma + c) + h] = 0.0;
		} else {
			for (h = 0; h < Ht[Lay]; h++)
				I[Ht[Lay] * (gamma + c) + h] = Hcache[Lay][t + c][h];
		}
	}
	double *X;
	NNt1 *tempo = new NNt1(NetT[Lay]);
	tempo->forward(X, I);

	for (h = 0; h < Ht[Lay + 1]; h++) {
		Hcache[Lay + 1][t][h] = tempo->out()[h];
	}
	delete tempo;
}

void DNN1::T_T(int Lay, int t1, int t2, int length) {
	double I[8196];
	int c, h;
	double *X;
	NNt1 *tempo = new NNt1(NetT[Lay]);

	for (int t = t1; t < t2; t++) {
		for (c = -gamma; c <= gamma; c++) {
			if (t + c <= 0 || t + c > length) {
				for (h = 0; h < Ht[Lay]; h++)
					I[Ht[Lay] * (gamma + c) + h] = 0.0;
			} else {
				for (h = 0; h < Ht[Lay]; h++)
					I[Ht[Lay] * (gamma + c) + h] = Hcache[Lay][t + c][h];
			}
		}
		tempo->forward(X, I);

		for (h = 0; h < Ht[Lay + 1]; h++) {
			Hcache[Lay + 1][t][h] = tempo->out()[h];
		}
	}
	delete tempo;
}

void DNN1::T_O(int t, int length) {
	double *X;
	NN1 *tempo = new NN1(NetO);
	tempo->forward(X, Hcache[NLayers][t]);
	for (int y = 0; y < NY; y++) {
		Y[NY * t + y] = tempo->out()[y];
	}
	delete tempo;
}

void DNN1::T_O(int t1, int t2, int length) {
	double *X;
	NN1 *tempo = new NN1(NetO);

	for (int t = t1; t < t2; t++) {
		tempo->forward(X, Hcache[NLayers][t]);
		for (int y = 0; y < NY; y++) {
			Y[NY * t + y] = tempo->out()[y];
		}
	}
	delete tempo;
}

int min(int a, int b) {
	if (a < b)
		return a;
	return b;
}

void DNN1::make_global_input(int length) {
	int t;

	memset(HcacheGlobal, 0, Ht[NLayers] * sizeof(double));
	for (t = 1; t <= length; t++) {
		for (int h = 0; h < Ht[NLayers]; h++) {
			HcacheGlobal[h] += Hcache[NLayers][t][h];
		}
	}

	for (int h = 0; h < Ht[NLayers]; h++) {
		HcacheGlobal[h] *= 0.01;
	}

	for (t = 1; t <= length; t++) {
		for (int h = 0; h < Ht[NLayers]; h++) {
			Hcache[NLayers][t][Ht[NLayers] + h] = HcacheGlobal[h];
		}
	}
}

void DNN1::propagate(double *seq, int length) {
	int t, l;
	int chunk = length / NTH;
	if (chunk < 1)
		chunk = 1;

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (t = 1; t <= length; t += chunk) {
			I_T(seq, t, min(t + chunk, length + 1), length);
		}
	}

	for (l = 0; l < NLayers; l++) {
#pragma omp parallel num_threads(NTH)
		{
#pragma omp for
			for (t = 1; t <= length; t += chunk) {
				T_T(l, t, min(t + chunk, length + 1), length);
			}
		}
	}

	make_global_input(length);

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (t = 1; t <= length; t += chunk) {
			T_O(t, min(t + chunk, length + 1), length);
		}
	}
}

void DNN1::propagate(int *seq, int length) {
	int t, l;
	int chunk = length / NTH;
	if (chunk < 1)
		chunk = 1;

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (t = 1; t <= length; t += chunk) {
			I_T(seq, t, min(t + chunk, length + 1), length);
		}
	}

	for (l = 0; l < NLayers; l++) {
#pragma omp parallel num_threads(NTH)
		{
#pragma omp for
			for (t = 1; t <= length; t += chunk) {
				T_T(l, t, min(t + chunk, length + 1), length);
			}
		}
	}

	make_global_input(length);

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (t = 1; t <= length; t += chunk) {
			T_O(t, min(t + chunk, length + 1), length);
		}
	}
}

void DNN1::I_Tbp(double *seq, int t, int length) {
	double I[8196];
	int c, h;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = 0.0;
		} else {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = seq[NU * (t + c) + i];
		}
	}
	double *X;
	NNt *tempo = new NNt(NetI);

	tempo->forward(I, X);

	tempo->backward(Hcachebp[0][t]);
	tempo->gradient(I, I, Hcachebp[0][t]);
#pragma omp critical
	{
		NetI->copy_dW(tempo);
	}
	delete tempo;
}

void DNN1::I_Tbp(double *seq, int t1, int t2, int length) {
	double I[8196];
	int c, h;
	double *X;
	NNt *tempo = new NNt(NetI);

	for (int t = t1; t < t2; t++) {
		for (c = -context; c <= context; c++) {
			if (t + c <= 0 || t + c > length) {
				for (int i = 0; i < NU; i++)
					I[NU * (context + c) + i] = 0.0;
			} else {
				for (int i = 0; i < NU; i++)
					I[NU * (context + c) + i] = seq[NU * (t + c) + i];
			}
		}
		tempo->forward(I, X);

		tempo->backward(Hcachebp[0][t]);
		tempo->gradient(I, I, Hcachebp[0][t]);
	}
#pragma omp critical
	{
		NetI->copy_dW(tempo);
	}
	delete tempo;
}

void DNN1::I_Tbp(int *seq, int t, int length) {
	int I[8196];
	int c, h;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			I[context + c] = -1;
		} else {
			I[context + c] = seq[t + c];
		}
	}
	double *X;
	NNt *tempo = new NNt(NetI);
	tempo->forward(I, X);

	tempo->backward(Hcachebp[0][t]);
	tempo->gradient(I, X, Hcachebp[0][t]);
#pragma omp critical
	{
		NetI->copy_dW(tempo);
	}
	delete tempo;
}

void DNN1::I_Tbp(int *seq, int t1, int t2, int length) {
	int I[8196];
	int c, h;
	double *X;
	NNt *tempo = new NNt(NetI);

	for (int t = t1; t < t2; t++) {
		for (c = -context; c <= context; c++) {
			if (t + c <= 0 || t + c > length) {
				I[context + c] = -1;
			} else {
				I[context + c] = seq[t + c];
			}
		}
		tempo->forward(I, X);

		tempo->backward(Hcachebp[0][t]);
		tempo->gradient(I, X, Hcachebp[0][t]);
	}

#pragma omp critical
	{
		NetI->copy_dW(tempo);
	}
	delete tempo;
}

void DNN1::T_Tbp(int Lay, int t, int length) {
	double I[8196];
	int c, h;

	for (c = -gamma; c <= gamma; c++) {
		if (t + c <= 0 || t + c > length) {
			for (h = 0; h < Ht[Lay]; h++)
				I[Ht[Lay] * (gamma + c) + h] = 0.0;
		} else {
			for (h = 0; h < Ht[Lay]; h++)
				I[Ht[Lay] * (gamma + c) + h] = Hcache[Lay][t + c][h];
		}
	}
	double *X;
	NNt1 *tempo = new NNt1(NetT[Lay]);
	tempo->forward(X, I);
	tempo->backward(Hcachebp[Lay + 1][t]);

#pragma omp critical
	{
		for (c = -gamma; c <= gamma; c++) {
			if (t + c <= 0 || t + c > length) {
			} else {
				for (h = 0; h < Ht[Lay]; h++) {
					Hcachebp[Lay][t + c][h] += tempo->back_out()[Ht[Lay] * (gamma + c) + h];
				}
			}
		}
	}

	tempo->gradient(I, I, Hcachebp[Lay + 1][t]);
#pragma omp critical
	{
		NetT[Lay]->copy_dW(tempo);
	}
	delete tempo;
}

void DNN1::T_Tbp(int Lay, int t1, int t2, int length) {
	double I[8196];
	int c, h;
	double *X;
	NNt1 *tempo = new NNt1(NetT[Lay]);

	for (int t = t1; t < t2; t++) {
		for (c = -gamma; c <= gamma; c++) {
			if (t + c <= 0 || t + c > length) {
				for (h = 0; h < Ht[Lay]; h++)
					I[Ht[Lay] * (gamma + c) + h] = 0.0;
			} else {
				for (h = 0; h < Ht[Lay]; h++)
					I[Ht[Lay] * (gamma + c) + h] = Hcache[Lay][t + c][h];
			}
		}
		tempo->forward(X, I);
		tempo->backward(Hcachebp[Lay + 1][t]);

#pragma omp critical
		{
			for (c = -gamma; c <= gamma; c++) {
				if (t + c <= 0 || t + c > length) {
				} else {
					for (h = 0; h < Ht[Lay]; h++) {
						Hcachebp[Lay][t + c][h] += tempo->back_out()[Ht[Lay] * (gamma + c) + h];
					}
				}
			}
		}

		tempo->gradient(I, I, Hcachebp[Lay + 1][t]);
	}

#pragma omp critical
	{
		NetT[Lay]->copy_dW(tempo);
	}
	delete tempo;
}

void DNN1::T_Obp(double *target, int t, int length) {
	double *X;
	NN1 *tempo = new NN1(NetO);
	tempo->forward(X, Hcache[NLayers][t]);
	for (int y = 0; y < NY; y++) {
		Y[NY * t + y] = tempo->out()[y];
	}
	double errloc = tempo->backward(target);

	for (int h = 0; h < 2 * Ht[NLayers]; h++) {
		Hcachebp[NLayers][t][h] = tempo->back_out()[h];
	}
	tempo->gradient(Hcache[NLayers][t], Hcache[NLayers][t], target);
#pragma omp critical
	{
		error += errloc;
		NetO->copy_dW(tempo);
	}
	delete tempo;
}

void DNN1::T_Obp(double **target, int t1, int t2, int length) {
	double *X;
	NN1 *tempo = new NN1(NetO);
	double errloc = 0;

	for (int t = t1; t < t2; t++) {
		tempo->forward(X, Hcache[NLayers][t]);
		for (int y = 0; y < NY; y++) {
			Y[NY * t + y] = tempo->out()[y];
		}
		errloc += tempo->backward(target[t]);

		for (int h = 0; h < 2 * Ht[NLayers]; h++) {
			Hcachebp[NLayers][t][h] = tempo->back_out()[h];
		}
		tempo->gradient(Hcache[NLayers][t], Hcache[NLayers][t], target[t]);
	}

#pragma omp critical
	{
		error += errloc;
		NetO->copy_dW(tempo);
	}
	delete tempo;
}

void DNN1::T_Obp(int target, int t, int length) {

	double *target2 = new double[NY];
	memset(target2, 0, NY * sizeof(double));
	target2[target] = 1.0;
	double *X;
	NN1 *tempo = new NN1(NetO);

	tempo->forward(X, Hcache[NLayers][t]);
	for (int y = 0; y < NY; y++) {
		Y[NY * t + y] = tempo->out()[y];
	}
	double errloc = tempo->backward(target2);

	for (int h = 0; h < 2 * Ht[NLayers]; h++) {
		Hcachebp[NLayers][t][h] = tempo->back_out()[h];
	}
	tempo->gradient(Hcache[NLayers][t], Hcache[NLayers][t], target2);
#pragma omp critical
	{
		error += errloc;
		NetO->copy_dW(tempo);
	}
	delete tempo;
	delete[] target2;
}

void DNN1::T_Obp(int *target, int t1, int t2, int length) {

	double *target2 = new double[NY];
	double *X;
	NN1 *tempo = new NN1(NetO);
	double errloc = 0;

	for (int t = t1; t < t2; t++) {
		memset(target2, 0, NY * sizeof(double));
		target2[target[t]] = 1.0;

		tempo->forward(X, Hcache[NLayers][t]);
		for (int y = 0; y < NY; y++) {
			Y[NY * t + y] = tempo->out()[y];
		}
		errloc += tempo->backward(target2);

		for (int h = 0; h < 2 * Ht[NLayers]; h++) {
			Hcachebp[NLayers][t][h] = tempo->back_out()[h];
		}
		tempo->gradient(Hcache[NLayers][t], Hcache[NLayers][t], target2);
	}
#pragma omp critical
	{
		error += errloc;
		NetO->copy_dW(tempo);
	}
	delete tempo;
	delete[] target2;
}

void DNN1::clear_cache() {
	for (int l = 0; l < NLayers; l++) {
		for (int t = 0; t < MAX; t++) {
			memset(Hcachebp[l][t], 0, Ht[l] * sizeof(double));
		}
	}
	for (int t = 0; t < MAX; t++) {
		memset(Hcachebp[NLayers][t], 0, 2 * Ht[NLayers] * sizeof(double));
	}
}

void DNN1::make_global_backprop(int length) {

	int t;

	memset(HcacheGlobalbp, 0, Ht[NLayers] * sizeof(double));
	for (t = 1; t <= length; t++) {
		for (int h = 0; h < Ht[NLayers]; h++) {
			HcacheGlobalbp[h] += Hcachebp[NLayers][t][Ht[NLayers] + h];
		}
	}

	for (int h = 0; h < Ht[NLayers]; h++) {
		HcacheGlobalbp[h] *= 0.01;
	}

	for (t = 1; t <= length; t++) {
		for (int h = 0; h < Ht[NLayers]; h++) {
			Hcachebp[NLayers][t][h] += HcacheGlobalbp[h];
		}
	}
}

void DNN1::extimation(double *seq, int *y, int length, int backp) {

	clear_cache();
	propagate(seq, length);

	int l;
	int chunk = length / NTH;
	if (chunk < 1)
		chunk = 1;

	double **target = new double *[length + 1];
	for (int c = 0; c <= length; c++) {
		target[c] = new double[NY];
		memset(target[c], 0, NY * sizeof(double));
		if (y[c] >= 0 && y[c] < NY)
			target[c][y[c]] = 1.0;
	}

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (int t = 1; t <= length; t += chunk) {
			T_Obp(target, t, min(t + chunk, length + 1), length);
		}
	}

	for (int c = 0; c <= length; c++) {
		delete[] target[c];
	}
	delete[] target;

	make_global_backprop(length);

	for (l = NLayers - 1; l >= 0; l--) {
#pragma omp parallel num_threads(NTH)
		{
#pragma omp for
			for (int t = 1; t <= length; t += chunk) {
				T_Tbp(l, t, min(t + chunk, length + 1), length);
			}
		}
	}

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (int t = 1; t <= length; t += chunk) {
			I_Tbp(seq, t, min(t + chunk, length + 1), length);
		}
	}
}

void DNN1::extimation(int *seq, int *y, int length, int backp) {

	clear_cache();
	propagate(seq, length);

	int l;
#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (int t = 1; t <= length; t++) {
			T_Obp(seq[t], t, length);
		}
	}

	make_global_backprop(length);

	for (l = NLayers - 1; l >= 0; l--) {
#pragma omp parallel num_threads(NTH)
		{
#pragma omp for
			for (int t = 1; t <= length; t++) {
				T_Tbp(l, t, length);
			}
		}
	}

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (int t = 1; t <= length; t++) {
			I_Tbp(seq, t, length);
		}
	}

	// delete[] target;
}

void DNN1::extimation(double *seq, double *y, int length, int backp) {

	clear_cache();
	propagate(seq, length);

	int l;
#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (int t = 1; t <= length; t++) {
			double *target = new double[NY];
			for (int o = 0; o < NY; o++) {
				target[o] = y[NY * t + o];
			}
			T_Obp(target, t, length);
			delete[] target;
		}
	}

	make_global_backprop(length);

	for (l = NLayers - 1; l >= 0; l--) {
#pragma omp parallel num_threads(NTH)
		{
#pragma omp for
			for (int t = 1; t <= length; t++) {
				T_Tbp(l, t, length);
			}
		}
	}

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (int t = 1; t <= length; t++) {
			I_Tbp(seq, t, length);
		}
	}
}

void DNN1::extimation(int *seq, double *y, int length, int backp) {

	clear_cache();
	propagate(seq, length);

	// double* target= new double[NY];

	int l;
#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (int t = 1; t <= length; t++) {
			T_Obp(seq[t], t, length);
		}
	}

	make_global_backprop(length);

	for (l = NLayers - 1; l >= 0; l--) {
#pragma omp parallel num_threads(NTH)
		{
#pragma omp for
			for (int t = 1; t <= length; t++) {
				T_Tbp(l, t, length);
			}
		}
	}

#pragma omp parallel num_threads(NTH)
	{
#pragma omp for
		for (int t = 1; t <= length; t++) {
			I_Tbp(seq, t, length);
		}
	}

	// delete[] target;
}

void DNN1::maximization() {

	NetO->updateWeights(epsilon);
	NetO->resetGradient();

	for (int l = 0; l < NLayers; l++) {
		NetT[l]->updateWeights(epsilon);
		NetT[l]->resetGradient();
	}

	NetI->updateWeights(epsilon);
	NetI->resetGradient();
}

void DNN1::maximizationL1() {

	NetO->updateWeightsL1(epsilon);
	NetO->resetGradient();

	for (int l = 0; l < NLayers; l++) {
		NetT[l]->updateWeightsL1(epsilon);
		NetT[l]->resetGradient();
	}

	NetI->updateWeightsL1(epsilon);
	NetI->resetGradient();
}

void DNN1::maximizationClipped() {
	NetO->updateWeights(epsilon);
	NetO->resetGradient();

	for (int l = 0; l < NLayers; l++) {
		NetT[l]->updateWeightsClipped(epsilon);
		NetT[l]->resetGradient();
	}

	NetI->updateWeightsClipped(epsilon);
	NetI->resetGradient();
}

void DNN1::Feed(double *seq, int length) {
	int t;

	propagate(seq, length);
}

void DNN1::Feed(int *seq, int length) {
	int t;

	propagate(seq, length);
}

void DNN1::predict(double *seq, int length) {

	Feed(seq, length);
}

void DNN1::predict(int *seq, int length) {

	Feed(seq, length);
}
