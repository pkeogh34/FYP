



#include "Model.h"




void
Model::alloc() {

	counted = new int[NY];
	nerrors_ = new int[NY];

	Conf=new int*[NY];
	for (int y=0;y<NY;y++)
		Conf[y]=new int[NY];
}



Model::Model(int the_NU, int the_NY, int the_NLayers, int* the_Ht, int the_NHi, int the_Hi, 
		  int the_gamma, int the_context, double* the_Thresholds) :
NU(the_NU), NY(the_NY), NLayers(the_NLayers), NHi(the_NHi), Hi(the_Hi), gamma(the_gamma), context(the_context)
{

Thresholds = new double[NY];
for (int y=0;y<NY-1;y++) 
	Thresholds[y] = the_Thresholds[y];

Ht = new int[NLayers];

for (int c=0;c<NLayers;c++) {
	Ht[c] = the_Ht[c];
}

Net = new DNN1(NU,NY,NLayers,Ht,NHi,Hi,gamma,context);
Net->resetGradient();

alloc();
}





Model::Model(istream& is, int the_NU, int the_NY, int the_NLayers, int* the_Ht, int the_NHi, int the_Hi, 
		  int the_gamma, int the_context, double* the_Thresholds) :
NU(the_NU), NY(the_NY), NLayers(the_NLayers), NHi(the_NHi), Hi(the_Hi), gamma(the_gamma), context(the_context)
{

Thresholds = new double[NY];
for (int y=0;y<NY-1;y++) 
	Thresholds[y] = the_Thresholds[y];

Ht = new int[NLayers];

for (int c=0;c<NLayers;c++) {
	Ht[c] = the_Ht[c];
}

int dummy;
double fdummy;
is >> dummy >> dummy >> dummy;
is >> dummy >> dummy >> dummy;
for (int c =0;c<NLayers-1;c++) {
	is >> dummy;
	is >> dummy;
}
is >> dummy >> dummy;

for (int y=0;y<NY-1;y++) 
	is >> fdummy;

Net = new DNN1(is, NU,NY,NLayers,Ht,NHi,Hi,gamma,context);
Net->resetGradient();

alloc();
}





Model::Model(istream& is) {
is >> NU >> NY >> NLayers;
is >> NHi >> Hi;
Ht = new int[NLayers];
for (int c =0;c<NLayers;c++) {
	is >> Ht[c];
}
is >> gamma >> context;

Thresholds = new double[NY];
for (int y=0;y<NY-1;y++) 
	is >> Thresholds[y];

Net = new DNN1(is);
Net->resetGradient();

alloc();
}





void
Model::read(istream& is) {
is >> NU >> NY >> NLayers;
is >> NHi >> Hi;
for (int c =0;c<NLayers;c++) {
	is >> Ht[c];
}
is >> gamma >> context;
for (int y=0;y<NY-1;y++) 
	is >> Thresholds[y];

Net->read(is);
Net->resetGradient();
}




void
Model::write(ostream& os) {
os << NU << " " << NY << " " << NLayers << "\n";
os << NHi << " " << Hi << "\n";
for (int c =0;c<NLayers;c++) {
	os << Ht[c] << " ";
}
os << "\n";
os << gamma << " " << context << "\n";

for (int y=0;y<NY-1;y++) 
	os << Thresholds[y] << " ";
os << "\n";

Net->write(os);
}



void
Model::randomize(int seed) {
Net->initWeights(seed);
}



void
Model::randomize_top(int seed) {
Net->initWeights_top(seed);
}



void
Model::extimation(Sequence *seq) {

int t,y;

int* O;
int a,c;//,m,maxm;
double sum=0;

//double* app=new double[NY*(seq->length+1)];


	sum=0;

	O=new int[seq->length+1];

	for (t=1; t<=seq->length; t++) {

		int close = 0;
		for (y=0;y<NY-1;y++) {
			if (seq->y[t]>Thresholds[y]) {
				close =y+1;
			}
		}
		O[t]= close;
		seq->yc[t] = close;
	}
	Net->extimation(seq->u,O,seq->length);
//	for (t=1; t<=seq->length; t++) {
//		for (c=0; c<NY; c++) {
//			app[NY*t+c]=Net->out()[NY*t+c];
//		}
//	}

	delete[] O;
//	delete[] app;
}





void
Model::maximization() {
Net->maximization();
}
void
Model::maximizationL1() {
Net->maximizationL1();
}
void
Model::maximizationClipped() {
Net->maximizationClipped();
}






void
Model::predict(Sequence* seq) {


int t,y;

int* O;
int a,c;//,m,maxm;
double sum=0;

double* app=new double[NY*(seq->length+1)];


	sum=0;

	O=new int[seq->length+1];

	for (t=1; t<=seq->length; t++) {

		int close = 0;
		for (y=0;y<NY-1;y++) {
			if (seq->y[t]>Thresholds[y]) {
				close =y+1;
			}
		}
		O[t]= close;
		seq->yc[t] = close;
	}

	Net->predict(seq->u,seq->length);
	for (t=1; t<=seq->length; t++) {
		for (c=0; c<NY; c++) {
			app[NY*t+c]=Net->out()[NY*t+c];
		}
	}

	delete[] O;


for (t=1; t<=seq->length; t++) {
	  double pred=0.0;
	  int argp=-1;

	  for (int c=0; c<NY; c++) {
		  if (app[NY*t+c]>pred) {
			  pred = app[NY*t+c];
			  argp = c;
		  }
	  }
	  seq->y_pred[t] = argp;
}


for (t=1; t<=seq->length; t++) {

	  if (seq->y_pred[t]!=seq->yc[t]) {
		    nerrors++;
		    nerrors_[seq->yc[t]]++;
	  }
	
	if (seq->yc[t] != -1 && seq->y_pred[t] != -1) {
	  Conf[seq->y_pred[t]][seq->yc[t]]++;
	  counted[seq->yc[t]]++;
	}
}

delete[] app;
}



