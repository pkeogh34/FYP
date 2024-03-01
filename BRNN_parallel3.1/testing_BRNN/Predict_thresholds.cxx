
#include <math.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include "MModel.h"
#include "Sequence.h"

#define NTH_TRAIN 6

#define t_class 2


using namespace std;





void
evaluate(MModel* M, DataSet& D, char* which, double threshold)
{
        int y;

//  cout << "\n counting_" << which << "_errors" << flush;
  M->resetNErrors();
  int p;
#pragma omp parallel num_threads(NTH_TRAIN)
{
#pragma omp for
  for (p=0; p<D.length; p++) {
    M->predict(D.seq[p], threshold);
//    if (p%20==0) cout << "." << flush;
  }
} // pragma

double a[128];
double all=0;

int PosC=0, FN=0, Pos=0, Neg=0;

for (y=0;y<M->getClasses();y++) {
	a[y]=M->getCounted()[y];
	all += a[y];

	if (y != t_class) { // not the positive class
		Neg += M->getCounted()[y];
	} else {
		Pos += M->getCounted()[y];
	}

}

cout << threshold << "\t";

// conf[pred][obs]
// PosC = conf[t_class][t_class]
// FN = conf[t_class][!t_class]

//  cout<<"\n";

/*
for (y=0;y<M->getClasses();y++) {
	for (int z=0;z<M->getClasses();z++) {
		cout << M->getConf()[z][y] << " ";
	}
	cout << "\n";
}
*/

for (y=0;y<M->getClasses();y++) {
	if (y != t_class) {
		FN += M->getConf()[t_class][y];
	} else {
		PosC += M->getConf()[t_class][y];
	}
}

  cout << PosC << "\t" << FN << "\t" << Pos << "\t" << Neg << "\n" << flush;

}








int
main(int argc, char** argv)
{

// This is for predicting


  if (argc<4) {
    cerr << "Usage: " << argv[0] << " model_file protein_file nThresholds threshold1 .. thresholdN\n";
    exit(1);
  }

    char model[256];
    char prot[256];
        strcpy(model,argv[1]);
        strcpy(prot,argv[2]);

	int nThresholds;
//	sscanf(argv[3],"%i",&nThresholds);
	nThresholds = atoi(argv[3]);

	double* thresholds = new double[nThresholds];

	for (int th = 0; th<nThresholds; th++) {
		thresholds[th] = atof(argv[4+th]);
	}

    MModel* M;
    char tmp[1024];
    strcpy(tmp, model);
    ifstream mstream(tmp);
    M = new MModel(mstream);

    M->set_target_class(t_class);

  cout << "Reading " << prot << " .. ";
  ifstream tstream(prot);
  DataSet T(tstream);

cout << "read\n" << flush;

for (int th = 0; th<nThresholds; th++) {
	evaluate(M, T, prot, thresholds[th]);
//	strcat(prot,"F");
//	T.write(prot);
}
        return 0;
}
