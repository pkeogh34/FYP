
// BRNN ver. 3.01 (5/11/2003)
// Copyright (C) Gianluca Pollastri 2003

#include "BRNN.h"

int min(int a, int b) {
	if (a < b)
		return a;
	return b;
}

void BRNN::alloc(int the_MAX) {

	MAX = the_MAX;

	int t;

	FF = new double *[MAX];
	FFbp = new double *[MAX];
	BB = new double *[MAX];
	BBbp = new double *[MAX];
	P_F = new double[NF];
	P_B = new double[NB];

	for (t = 0; t < MAX; t++) {
		FF[t] = new double[NF];
		FFbp[t] = new double[NF];
		BB[t] = new double[NB];
		BBbp[t] = new double[NB];
	}

	Y = new double[MAX * NY];

	for (int f = 0; f < NF; f++)
		P_F[f] = 0;
	for (int b = 0; b < NB; b++)
		P_B[b] = 0;
}

void BRNN::dealloc() {
	int t;

	for (t = 0; t < MAX; t++) {
		delete[] FF[t];
		delete[] FFbp[t];
		delete[] BB[t];
		delete[] BBbp[t];
	}

	delete[] FF;
	delete[] FFbp;
	delete[] BB;
	delete[] BBbp;
	delete[] P_F;
	delete[] P_B;

	delete[] Y;
}

BRNN::BRNN(int the_NU, int the_NY, int the_NH, int the_context, int the_Moore, int the_NF, int the_NB, int the_NH2, int the_CoF, int the_CoB, int the_Step, int the_shortcut, int the_doubleo) : NU(the_NU), NY(the_NY), NH(the_NH), context(the_context), Moore(the_Moore), NF(the_NF), NB(the_NB), NH2(the_NH2), CoF(the_CoF), CoB(the_CoB), Step(the_Step), shortcut(the_shortcut), doubleo(the_doubleo) {

	int NK[8196];

	for (int c = 0; c < 2 * context + 1; c++) {
		NK[c] = NU;
	}
	if (Moore)
		NetOut = new NN((2 * context + 1), (2 * CoF + 1) * NF + (2 * CoB + 1) * NB, NH, NY, NK);
	else
		NetOut = new NN(0, (2 * CoF + 1) * NF + (2 * CoB + 1) * NB, NH, NY, NK);

	if (shortcut > 0) {
		NetF = new NNt((2 * context + 1), (shortcut)*NF, NH2, NF, NK, 0);
		NetB = new NNt((2 * context + 1), (shortcut)*NB, NH2, NB, NK, 0);
	} else {
		NetF = new NNt((2 * context + 1), NF, NH2, NF, NK, 0);
		NetB = new NNt((2 * context + 1), NB, NH2, NB, NK, 0);
	}

	NetOut->set_input(1);
	NetF->set_input(1);
	NetB->set_input(1);

	NetOut->set_output(1);
	NetF->set_output(0);
	NetB->set_output(0);

	alloc(8196);
	resetGradient();
}

BRNN::BRNN(BRNN *from, int seq_length) {

	NU = from->NU;
	NY = from->NY;
	NH = from->NH;
	context = from->context;
	Moore = from->Moore;
	NF = from->NF;
	NB = from->NB;
	NH2 = from->NH2;
	CoF = from->CoF;
	CoB = from->CoB;
	Step = from->Step;
	shortcut = from->shortcut;
	doubleo = from->doubleo;

	int NK[8196];

	for (int c = 0; c < 2 * context + 1; c++) {
		NK[c] = NU;
	}
	NetOut = new NN(from->NetOut);
	NetF = new NNt(from->NetF);
	NetB = new NNt(from->NetB);

	NetOut->set_input(1);
	NetF->set_input(1);
	NetB->set_input(1);

	NetOut->set_output(1);
	NetF->set_output(0);
	NetB->set_output(0);

	alloc(seq_length + 1);
	resetGradient();
}

BRNN::~BRNN() {
	delete NetOut;
	delete NetF;
	delete NetB;

	dealloc();
}

BRNN::BRNN(istream &is) {
	is >> NU >> NY >> NH >> context >> Moore;
	is >> NF >> NB >> NH2 >> CoF >> CoB >> Step >> shortcut >> doubleo;

	NetOut = new NN(is);
	NetF = new NNt(is);
	NetB = new NNt(is);

	NetOut->set_input(1);
	NetF->set_input(1);
	NetB->set_input(1);

	NetOut->set_output(1);
	NetF->set_output(0);
	NetB->set_output(0);

	alloc(8192);
	resetGradient();
}

void BRNN::read(istream &is) {
	is >> NU >> NY >> NH >> context >> Moore;
	is >> NF >> NB >> NH2 >> CoF >> CoB >> Step >> shortcut >> doubleo;

	NetOut->read(is);
	NetF->read(is);
	NetB->read(is);

	NetOut->set_input(1);
	NetF->set_input(1);
	NetB->set_input(1);

	Moore = NetOut->get_NI();
	if (Moore)
		Moore = 1;
}

void BRNN::write(ostream &os) {
	os << NU << " " << NY << " " << NH << " " << context << " " << Moore << "\n";
	os << NF << " " << NB << " " << NH2 << " " << CoF << " " << CoB << " " << Step << " " << shortcut << " " << doubleo << "\n";

	NetOut->write(os);
	NetF->write(os);
	NetB->write(os);
}

void BRNN::resetGradient() {
	NetOut->resetGradient();
	NetF->resetGradient();
	NetB->resetGradient();
}

void BRNN::initWeights(int seed) {
	NetOut->initWeights(seed++);
	NetF->initWeights(seed++);
	NetB->initWeights(seed++);
}

void BRNN::F1_F(double *seq, int t, int length) {

	double I[8196];
	int f, s, c;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = 0.0;
		} else {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = seq[NU * (t + c) + i];
		}
	}
	double *X = new double[(1 + shortcut) * NF];
	for (f = 0; f < NF; f++) {
		X[f] = FF[t - 1][f];
	}

	for (s = 2; s <= shortcut; s++)
		for (f = (s - 1) * NF; f < s * NF; f++) {
			if (t - s >= 0)
				X[f] = FF[t - s][f - (s - 1) * NF];
			else
				X[f] = 0;
		}

	NetF->forward(I, X);
	for (f = 0; f < NF; f++) {
		FF[t][f] = NetF->out()[f];
	}
	delete[] X;
}

void BRNN::F1_F(int *seq, int t, int length) {

	int I[8196];
	int f, s, c;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			I[context + c] = 0;
		} else {
			I[context + c] = seq[t + c];
		}
	}
	double *X = new double[(1 + shortcut) * NF];
	for (f = 0; f < NF; f++) {
		X[f] = FF[t - 1][f];
	}

	for (s = 2; s <= shortcut; s++)
		for (f = (s - 1) * NF; f < s * NF; f++) {
			if (t - s >= 0)
				X[f] = FF[t - s][f - (s - 1) * NF];
			else
				X[f] = 0;
		}

	NetF->forward(I, X);
	for (f = 0; f < NF; f++) {
		FF[t][f] = NetF->out()[f];
	}
	delete[] X;
}

void BRNN::B1_B(double *seq, int t, int length) {

	double I[8196];
	int b, s, c;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = 0.0;
		} else {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = seq[NU * (t + c) + i];
		}
	}
	double *X = new double[(1 + shortcut) * NB];
	for (b = 0; b < NB; b++) {
		X[b] = BB[t + 1][b];
	}

	for (s = 2; s <= shortcut; s++)
		for (b = (s - 1) * NB; b < s * NB; b++) {
			if (t + s <= length + 1)
				X[b] = BB[t + s][b - (s - 1) * NB];
			else
				X[b] = 0;
		}

	NetB->forward(I, X);
	for (b = 0; b < NB; b++) {
		BB[t][b] = NetB->out()[b];
	}

	delete[] X;
}

void BRNN::B1_B(int *seq, int t, int length) {

	int I[8196];
	int b, s, c;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			I[context + c] = 0;
		} else {
			I[context + c] = seq[t + c];
		}
	}
	double *X = new double[(1 + shortcut) * NB];
	for (b = 0; b < NB; b++) {
		X[b] = BB[t + 1][b];
	}

	for (s = 2; s <= shortcut; s++)
		for (b = (s - 1) * NB; b < s * NB; b++) {
			if (t + s <= length + 1)
				X[b] = BB[t + s][b - (s - 1) * NB];
			else
				X[b] = 0;
		}

	NetB->forward(I, X);
	for (b = 0; b < NB; b++) {
		BB[t][b] = NetB->out()[b];
	}

	delete[] X;
}

void BRNN::propagate(double *seq, int length) {
	int T = length;
	int t, f, b;

	for (f = 0; f < NF; f++)
		FF[0][f] = P_F[f];
	for (b = 0; b < NB; b++)
		BB[T + 1][b] = P_B[b];

	for (t = 1; t <= T; t++)
		F1_F(seq, t, T);
	for (t = T; t > 0; t--)
		B1_B(seq, t, T);
}

void BRNN::propagate(int *seq, int length) {
	int T = length;
	int t, f, b;

	for (f = 0; f < NF; f++)
		FF[0][f] = P_F[f];
	for (b = 0; b < NB; b++)
		BB[T + 1][b] = P_B[b];

	for (t = 1; t <= T; t++)
		F1_F(seq, t, T);
	for (t = T; t > 0; t--)
		B1_B(seq, t, T);
}

void BRNN::forward(double *seq, int t, int length) {
	int f, b, v, c;
	double I[8196];

	// Next cycle sets the input vector
	if (Moore) {
		for (c = -context; c <= context; c++) {
			if (t + c <= 0 || t + c > length) {
				for (int i = 0; i < NU; i++)
					I[NU * (context + c) + i] = 0.0;
			} else {
				for (int i = 0; i < NU; i++)
					I[NU * (context + c) + i] = seq[NU * (t + c) + i];
			}
		}
	}

	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB];
	// We now set the hidden inputs
	for (v = -CoF; v <= CoF; v++) {
		if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
			for (f = 0; f < NF; f++)
				X[NF * (CoF + v) + f] = 0;
		} else {
			for (f = 0; f < NF; f++)
				X[NF * (CoF + v) + f] = FF[t + (Step * v)][f];
		}
	}

	for (v = -CoB; v <= CoB; v++) {
		if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
			for (b = 0; b < NB; b++)
				X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = 0;
		} else {
			for (b = 0; b < NB; b++)
				X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = BB[t + (Step * v)][b];
		}
	}

	NetOut->forward(I, X);

	for (int y = 0; y < NY; y++)
		Y[NY * t + y] = NetOut->out()[y];

	delete[] X;
}

void BRNN::forward(int *seq, int t, int length) {
	int f, b, v, c;
	int I[8196];

	// Next cycle sets the input vector
	if (Moore) {
		for (c = -context; c <= context; c++) {
			if (t + c <= 0 || t + c > length) {
				I[context + c] = 0;
			} else {
				I[context + c] = seq[t + c];
			}
		}
	}

	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB + 1024];
	// We now set the hidden inputs
	for (v = -CoF; v <= CoF; v++) {
		if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
			for (f = 0; f < NF; f++)
				X[NF * (CoF + v) + f] = 0;
		} else {
			for (f = 0; f < NF; f++)
				X[NF * (CoF + v) + f] = FF[t + (Step * v)][f];
		}
	}

	for (v = -CoB; v <= CoB; v++) {
		if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
			for (b = 0; b < NB; b++)
				X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = 0;
		} else {
			for (b = 0; b < NB; b++)
				X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = BB[t + (Step * v)][b];
		}
	}

	NetOut->forward(I, X);

	for (int y = 0; y < NY; y++)
		Y[NY * t + y] = NetOut->out()[y];

	delete[] X;
}

void BRNN::forward(double *seq, int t1, int t2, int length) {
	int f, b, v, c;
	double I[8196];

	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB];

	for (int t = t1; t < t2; t++) {

		// Next cycle sets the input vector
		if (Moore) {
			for (c = -context; c <= context; c++) {
				if (t + c <= 0 || t + c > length) {
					for (int i = 0; i < NU; i++)
						I[NU * (context + c) + i] = 0.0;
				} else {
					for (int i = 0; i < NU; i++)
						I[NU * (context + c) + i] = seq[NU * (t + c) + i];
				}
			}
		}

		// We now set the hidden inputs
		for (v = -CoF; v <= CoF; v++) {
			if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
				for (f = 0; f < NF; f++)
					X[NF * (CoF + v) + f] = 0;
			} else {
				for (f = 0; f < NF; f++)
					X[NF * (CoF + v) + f] = FF[t + (Step * v)][f];
			}
		}

		for (v = -CoB; v <= CoB; v++) {
			if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
				for (b = 0; b < NB; b++)
					X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = 0;
			} else {
				for (b = 0; b < NB; b++)
					X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = BB[t + (Step * v)][b];
			}
		}

		NetOut->forward(I, X);

		for (int y = 0; y < NY; y++)
			Y[NY * t + y] = NetOut->out()[y];

	} // t

	delete[] X;
}

void BRNN::forward(int *seq, int t1, int t2, int length) {
	int f, b, v, c;
	int I[8196];

	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB + 1024];

	for (int t = t1; t < t2; t++) {

		// Next cycle sets the input vector
		if (Moore) {
			for (c = -context; c <= context; c++) {
				if (t + c <= 0 || t + c > length) {
					I[context + c] = 0;
				} else {
					I[context + c] = seq[t + c];
				}
			}
		}

		// We now set the hidden inputs
		for (v = -CoF; v <= CoF; v++) {
			if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
				for (f = 0; f < NF; f++)
					X[NF * (CoF + v) + f] = 0;
			} else {
				for (f = 0; f < NF; f++)
					X[NF * (CoF + v) + f] = FF[t + (Step * v)][f];
			}
		}

		for (v = -CoB; v <= CoB; v++) {
			if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
				for (b = 0; b < NB; b++)
					X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = 0;
			} else {
				for (b = 0; b < NB; b++)
					X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = BB[t + (Step * v)][b];
			}
		}

		NetOut->forward(I, X);

		for (int y = 0; y < NY; y++)
			Y[NY * t + y] = NetOut->out()[y];
	}

	delete[] X;
}

void BRNN::forward_backward(double *seq, int t, int length, double *target, int backp) {
	int f, b, v, c;
	double I[8196];

	// Next cycle sets the input vector
	if (Moore) {
		for (c = -context; c <= context; c++) {
			if (t + c <= 0 || t + c > length) {
				for (int i = 0; i < NU; i++)
					I[NU * (context + c) + i] = 0.0;
			} else {
				for (int i = 0; i < NU; i++)
					I[NU * (context + c) + i] = seq[NU * (t + c) + i];
			}
		}
	}

	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB];
	// We now set the hidden inputs
	for (v = -CoF; v <= CoF; v++) {
		if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
			for (f = 0; f < NF; f++)
				X[NF * (CoF + v) + f] = 0;
		} else {
			for (f = 0; f < NF; f++)
				X[NF * (CoF + v) + f] = FF[t + (Step * v)][f];
		}
	}

	for (v = -CoB; v <= CoB; v++) {
		if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
			for (b = 0; b < NB; b++)
				X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = 0;
		} else {
			for (b = 0; b < NB; b++)
				X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = BB[t + (Step * v)][b];
		}
	}

	NetOut->forward(I, X);

	for (int y = 0; y < NY; y++)
		Y[NY * t + y] = NetOut->out()[y];

	double errloc = NetOut->backward(target);

	error += errloc;

	for (v = -CoF; v <= CoF; v++) {
		if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
		} else {
			for (f = 0; f < NF; f++)
				FFbp[t + (Step * v)][f] += NetOut->back_out()[NU * (2 * context + 1) + NF * (CoF + v) + f];
		}
	}
	for (v = -CoB; v <= CoB; v++) {
		if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
		} else {
			for (b = 0; b < NB; b++)
				BBbp[t + (Step * v)][b] += NetOut->back_out()[NU * (2 * context + 1) + NF * (2 * CoF + 1) + NB * (CoB + v) + b];
		}
	}

	NetOut->gradient(I, X, target);

	delete[] X;
}

void BRNN::forward_backward(int *seq, int t, int length, double *target, int backp) {
	int f, b, v, c;
	int I[8196];

	// Next cycle sets the input vector
	if (Moore) {
		for (c = -context; c <= context; c++) {
			if (t + c <= 0 || t + c > length) {
				I[context + c] = 0;
			} else {
				I[context + c] = seq[t + c];
			}
		}
	}

	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB + 1024];
	// We now set the hidden inputs
	for (v = -CoF; v <= CoF; v++) {
		if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
			for (f = 0; f < NF; f++)
				X[NF * (CoF + v) + f] = 0;
		} else {
			for (f = 0; f < NF; f++)
				X[NF * (CoF + v) + f] = FF[t + (Step * v)][f];
		}
	}

	for (v = -CoB; v <= CoB; v++) {
		if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
			for (b = 0; b < NB; b++)
				X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = 0;
		} else {
			for (b = 0; b < NB; b++)
				X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = BB[t + (Step * v)][b];
		}
	}

	NetOut->forward(I, X);

	for (int y = 0; y < NY; y++)
		Y[NY * t + y] = NetOut->out()[y];

	double errloc = NetOut->backward(target);

	error += errloc;

	for (v = -CoF; v <= CoF; v++) {
		if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
		} else {
			for (f = 0; f < NF; f++)
				FFbp[t + (Step * v)][f] += NetOut->back_out()[NU * (2 * context + 1) + NF * (CoF + v) + f];
		}
	}
	for (v = -CoB; v <= CoB; v++) {
		if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
		} else {
			for (b = 0; b < NB; b++)
				BBbp[t + (Step * v)][b] += NetOut->back_out()[NU * (2 * context + 1) + NF * (2 * CoF + 1) + NB * (CoB + v) + b];
		}
	}

	NetOut->gradient(I, X, target);

	delete[] X;
}

void BRNN::forward_backward(double *seq, int t1, int t2, int length, double **target, int backp) {
	int f, b, v, c;
	double I[8196];

	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB];

	for (int t = t1; t < t2; t++) {

		// Next cycle sets the input vector
		if (Moore) {
			for (c = -context; c <= context; c++) {
				if (t + c <= 0 || t + c > length) {
					for (int i = 0; i < NU; i++)
						I[NU * (context + c) + i] = 0.0;
				} else {
					for (int i = 0; i < NU; i++)
						I[NU * (context + c) + i] = seq[NU * (t + c) + i];
				}
			}
		}

		// We now set the hidden inputs
		for (v = -CoF; v <= CoF; v++) {
			if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
				for (f = 0; f < NF; f++)
					X[NF * (CoF + v) + f] = 0;
			} else {
				for (f = 0; f < NF; f++)
					X[NF * (CoF + v) + f] = FF[t + (Step * v)][f];
			}
		}

		for (v = -CoB; v <= CoB; v++) {
			if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
				for (b = 0; b < NB; b++)
					X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = 0;
			} else {
				for (b = 0; b < NB; b++)
					X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = BB[t + (Step * v)][b];
			}
		}

		NetOut->forward(I, X);

		for (int y = 0; y < NY; y++)
			Y[NY * t + y] = NetOut->out()[y];

		double errloc = NetOut->backward(target[t]);

		error += errloc;

		for (v = -CoF; v <= CoF; v++) {
			if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
			} else {
				for (f = 0; f < NF; f++)
					FFbp[t + (Step * v)][f] += NetOut->back_out()[NU * (2 * context + 1) + NF * (CoF + v) + f];
			}
		}
		for (v = -CoB; v <= CoB; v++) {
			if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
			} else {
				for (b = 0; b < NB; b++)
					BBbp[t + (Step * v)][b] += NetOut->back_out()[NU * (2 * context + 1) + NF * (2 * CoF + 1) + NB * (CoB + v) + b];
			}
		}

		NetOut->gradient(I, X, target[t]);
	}

	delete[] X;
}

void BRNN::forward_backward(int *seq, int t1, int t2, int length, double **target, int backp) {
	int f, b, v, c;
	int I[8196];

	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB + 1024];

	for (int t = t1; t < t2; t++) {

		// Next cycle sets the input vector
		if (Moore) {
			for (c = -context; c <= context; c++) {
				if (t + c <= 0 || t + c > length) {
					I[context + c] = 0;
				} else {
					I[context + c] = seq[t + c];
				}
			}
		}

		// We now set the hidden inputs
		for (v = -CoF; v <= CoF; v++) {
			if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
				for (f = 0; f < NF; f++)
					X[NF * (CoF + v) + f] = 0;
			} else {
				for (f = 0; f < NF; f++)
					X[NF * (CoF + v) + f] = FF[t + (Step * v)][f];
			}
		}

		for (v = -CoB; v <= CoB; v++) {
			if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
				for (b = 0; b < NB; b++)
					X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = 0;
			} else {
				for (b = 0; b < NB; b++)
					X[NF * (2 * CoF + 1) + NB * (CoB + v) + b] = BB[t + (Step * v)][b];
			}
		}

		NetOut->forward(I, X);

		for (int y = 0; y < NY; y++)
			Y[NY * t + y] = NetOut->out()[y];

		double errloc = NetOut->backward(target[t]);

		error += errloc;

		for (v = -CoF; v <= CoF; v++) {
			if ((t + (Step * v)) < 0 || (t + (Step * v)) > length) {
			} else {
				for (f = 0; f < NF; f++)
					FFbp[t + (Step * v)][f] += NetOut->back_out()[NU * (2 * context + 1) + NF * (CoF + v) + f];
			}
		}
		for (v = -CoB; v <= CoB; v++) {
			if ((t + (Step * v)) < 1 || (t + (Step * v)) > length + 1) {
			} else {
				for (b = 0; b < NB; b++)
					BBbp[t + (Step * v)][b] += NetOut->back_out()[NU * (2 * context + 1) + NF * (2 * CoF + 1) + NB * (CoB + v) + b];
			}
		}

		NetOut->gradient(I, X, target[t]);
	}

	delete[] X;
}

void BRNN::F1_Fbp(double *seq, int t, int length, int backp) {
	double I[8196];
	int f, s, c;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = 0.0;
		} else {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = seq[NU * (t + c) + i];
		}
	}
	double *X = new double[(1 + shortcut) * NF];
	for (f = 0; f < NF; f++) {
		X[f] = FF[t - 1][f];
	}

	for (s = 2; s <= shortcut; s++)
		for (f = (s - 1) * NF; f < s * NF; f++) {
			if (t - s >= 0)
				X[f] = FF[t - s][f - (s - 1) * NF];
			else
				X[f] = 0;
		}

	NetF->forward(I, X);

	errorF += NetF->backward(FFbp[t]);
	for (f = 0; f < NF; f++)
		FFbp[t - 1][f] += NetF->back_out()[NU * (2 * context + 1) + f];

	for (s = 2; s <= shortcut; s++)
		for (f = (s - 1) * NF; f < s * NF; f++) {
			if (t - s >= 0)
				FFbp[t - s][f - (s - 1) * NF] += NetF->back_out()[NU * (2 * context + 1) + f];
		}

	NetF->gradient(I, X, FFbp[t]);

	delete[] X;
}

void BRNN::F1_Fbp(int *seq, int t, int length, int backp) {
	int I[8196];
	int f, s, c;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			I[context + c] = 0;
		} else {
			I[context + c] = seq[t + c];
		}
	}
	double *X = new double[(1 + shortcut) * NF];
	for (f = 0; f < NF; f++) {
		X[f] = FF[t - 1][f];
	}

	for (s = 2; s <= shortcut; s++)
		for (f = (s - 1) * NF; f < s * NF; f++) {
			if (t - s >= 0)
				X[f] = FF[t - s][f - (s - 1) * NF];
			else
				X[f] = 0;
		}

	NetF->forward(I, X);

	errorF += NetF->backward(FFbp[t]);
	for (f = 0; f < NF; f++)
		FFbp[t - 1][f] += NetF->back_out()[NU * (2 * context + 1) + f];

	for (s = 2; s <= shortcut; s++)
		for (f = (s - 1) * NF; f < s * NF; f++) {
			if (t - s >= 0)
				FFbp[t - s][f - (s - 1) * NF] += NetF->back_out()[NU * (2 * context + 1) + f];
		}

	NetF->gradient(I, X, FFbp[t]);

	delete[] X;
}

void BRNN::B1_Bbp(double *seq, int t, int length, int backp) {
	double I[8196];
	int b, s, c;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = 0.0;
		} else {
			for (int i = 0; i < NU; i++)
				I[NU * (context + c) + i] = seq[NU * (t + c) + i];
		}
	}
	double *X = new double[(1 + shortcut) * NB];
	for (b = 0; b < NB; b++) {
		X[b] = BB[t + 1][b];
	}

	for (s = 2; s <= shortcut; s++)
		for (b = (s - 1) * NB; b < s * NB; b++) {
			if (t + s <= length + 1)
				X[b] = BB[t + s][b - (s - 1) * NB];
			else
				X[b] = 0;
		}

	NetB->forward(I, X);

	errorB += NetB->backward(BBbp[t]);
	for (b = 0; b < NB; b++)
		BBbp[t + 1][b] += NetB->back_out()[NU * (2 * context + 1) + b];

	for (s = 2; s <= shortcut; s++)
		for (b = (s - 1) * NB; b < s * NB; b++)
			if (t + s <= length + 1)
				BBbp[t + s][b - (s - 1) * NB] += NetB->back_out()[NU * (2 * context + 1) + b];

	NetB->gradient(I, X, BBbp[t]);

	delete[] X;
}

void BRNN::B1_Bbp(int *seq, int t, int length, int backp) {
	int I[8196];
	int b, s, c;

	for (c = -context; c <= context; c++) {
		if (t + c <= 0 || t + c > length) {
			I[context + c] = 0;
		} else {
			I[context + c] = seq[t + c];
		}
	}
	double *X = new double[(1 + shortcut) * NB];
	for (b = 0; b < NB; b++) {
		X[b] = BB[t + 1][b];
	}

	for (s = 2; s <= shortcut; s++)
		for (b = (s - 1) * NB; b < s * NB; b++) {
			if (t + s <= length + 1)
				X[b] = BB[t + s][b - (s - 1) * NB];
			else
				X[b] = 0;
		}

	NetB->forward(I, X);

	errorB += NetB->backward(BBbp[t]);
	for (b = 0; b < NB; b++)
		BBbp[t + 1][b] += NetB->back_out()[NU * (2 * context + 1) + b];

	for (s = 2; s <= shortcut; s++)
		for (b = (s - 1) * NB; b < s * NB; b++)
			if (t + s <= length + 1)
				BBbp[t + s][b - (s - 1) * NB] += NetB->back_out()[NU * (2 * context + 1) + b];

	NetB->gradient(I, X, BBbp[t]);

	delete[] X;
}

void BRNN::back_propagate(double *seq, int length, int backp) {
	int T = length;
	int t;

	for (t = T; t > 0; t--) {
		F1_Fbp(seq, t, T, backp);
	}
	for (t = 1; t <= T; t++) {
		B1_Bbp(seq, t, T, backp);
	}
}

void BRNN::back_propagate(int *seq, int length, int backp) {
	int T = length;
	int t;

	for (t = T; t > 0; t--) {
		F1_Fbp(seq, t, T, backp);
	}
	for (t = 1; t <= T; t++) {
		B1_Bbp(seq, t, T, backp);
	}
}

void BRNN::extimation(double *seq, int *y, int length, int backp) {

	int t, f, b, v, c;

	// cout << length << " "<<flush;
	// cout << "*" << flush;
	propagate(seq, length);
	// cout << "*" << flush;

	for (t = 0; t < length + 2; t++) {
		memset(FFbp[t], 0, NF * sizeof(double));
		memset(BBbp[t], 0, NB * sizeof(double));
	}

	double **target = new double *[length + 1];
	for (int c = 0; c <= length; c++) {
		target[c] = new double[NY];
		memset(target[c], 0, NY * sizeof(double));
		if (y[c] >= 0 && y[c] < NY)
			target[c][y[c]] = 1.0;
	}

	int chunk = length / NTH;

	for (int t = 1; t <= length; t += chunk) {
		forward_backward(seq, t, min(t + chunk, length + 1), length, target);
	}

	for (int c = 0; c <= length; c++) {
		delete[] target[c];
	}
	delete[] target;

	back_propagate(seq, length, backp);

	NetOut->set_input(1);
	NetF->set_input(1);
	NetB->set_input(1);
}

void BRNN::extimation(int *seq, int *y, int length, int backp) {

	int t, f, b, v, c;

	propagate(seq, length);

	for (t = 0; t < length + 2; t++) {
		memset(FFbp[t], 0, NF * sizeof(double));
		memset(BBbp[t], 0, NB * sizeof(double));
	}

	double **target = new double *[length + 1];
	for (int c = 0; c <= length; c++) {
		target[c] = new double[NY];
		memset(target[c], 0, NY * sizeof(double));
		if (y[c] >= 0 && y[c] < NY)
			target[c][y[c]] = 1.0;
	}

	int chunk = length / NTH;

	for (int t = 1; t <= length; t += chunk) {
		forward_backward(seq, t, min(t + chunk, length + 1), length, target);
	}

	for (int c = 0; c <= length; c++) {
		delete[] target[c];
	}
	delete[] target;

	back_propagate(seq, length, backp);

	NetOut->set_input(1);
	NetF->set_input(1);
	NetB->set_input(1);
}

void BRNN::extimation(double *seq, double *y, int length, int backp) {

	int t, f, b, v, c;

	double *target = new double[1024];

	// cout << length << " "<<flush;

	propagate(seq, length);

	for (t = 0; t < length + 2; t++) {
		memset(FFbp[t], 0, NF * sizeof(double));
		memset(BBbp[t], 0, NB * sizeof(double));
	}

	for (t = 1; t <= length; t++) {
		for (c = 0; c < NY; c++) {
			target[c] = y[NY * t + c];
		}
		forward_backward(seq, t, length, target);
	}

	back_propagate(seq, length, backp);

	NetOut->set_input(1);
	NetF->set_input(1);
	NetB->set_input(1);

	delete[] target;
}

void BRNN::extimation(int *seq, double *y, int length, int backp) {

	int t, f, b, v, c;
	int I[8192];
	double *X = new double[(2 * CoF + 1) * NF + (2 * CoB + 1) * NB];

	propagate(seq, length);

	for (t = 0; t < length + 2; t++) {
		memset(FFbp[t], 0, NF * sizeof(double));
		memset(BBbp[t], 0, NB * sizeof(double));
	}

	for (t = 1; t <= length; t++) {
		double *target = new double[1024];

		for (c = 0; c < NY; c++) {
			target[c] = y[NY * t + c];
		}

		forward_backward(seq, t, length, target);
		delete[] target;
	}

	back_propagate(seq, length, backp);

	NetOut->set_input(1);
	NetF->set_input(1);
	NetB->set_input(1);
}

void BRNN::resetBP(int length) {
	for (int t = 0; t < length + 2; t++) {
		memset(FFbp[t], 0, NF * sizeof(double));
		memset(BBbp[t], 0, NB * sizeof(double));
	}
}

void BRNN::maximization() {

	NetOut->updateWeights(epsilon);
	NetOut->resetGradient();

	NetF->updateWeights(epsilon);
	NetF->resetGradient();

	NetB->updateWeights(epsilon);
	NetB->resetGradient();
}

void BRNN::maximizationL1() {

	NetOut->updateWeightsL1(epsilon);
	NetOut->resetGradient();

	NetF->updateWeightsL1(epsilon);
	NetF->resetGradient();

	NetB->updateWeightsL1(epsilon);
	NetB->resetGradient();
}

void BRNN::maximizationClipped() {

	NetOut->updateWeights(epsilon);
	NetOut->resetGradient();

	NetF->updateWeightsClipped(epsilon);
	NetF->resetGradient();

	NetB->updateWeightsClipped(epsilon);
	NetB->resetGradient();
}

void BRNN::Feed(double *seq, int length) {
	int t;

	propagate(seq, length);

	int chunk = length / NTH;

	for (int t = 1; t <= length; t += chunk) {
		forward(seq, t, min(t + chunk, length + 1), length);
	}
}

void BRNN::Feed(int *seq, int length) {
	int t;

	propagate(seq, length);

	int chunk = length / NTH;

	for (int t = 1; t <= length; t += chunk) {
		forward(seq, t, min(t + chunk, length + 1), length);
	}
}

void BRNN::predict(double *seq, int length) {

	Feed(seq, length);
}

void BRNN::predict(int *seq, int length) {

	Feed(seq, length);
}
