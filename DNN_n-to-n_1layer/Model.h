
#ifndef Model_h
#define Model_h 1

#include <stdlib.h>
#include <math.h>
#include "Sequence.h"
#include "DNN1.h"


class Model {
 private:
  int NU;
  int NY;
  int NLayers;
  int NHi,Hi;
  int* Ht;
  int gamma, context;

  double* Thresholds;

  int modular;


  DNN1* Net;

  int** Conf;

//  double temp_error;
  int temp_aas;
  
  int* counted;
  double squared_error;
  double error;
  int nerrors;
  int* nerrors_;

  double epsilon;

  void alloc();


 public:


  Model(int NU, int NY, int NLayers, int* Ht, int NHi, int Hi, int gamma, int context, double* Thresholds);
  Model(istream& is, int NU, int NY, int NLayers, int* Ht, int NHi, int Hi, int gamma, int context, double* Thresholds);
  Model(istream& is);
  void read(istream& is);
  void write(ostream& os);

  void randomize(int seed);
  void randomize_top(int seed);


  void extimation(Sequence* seq);
  void maximization();
  void maximizationL1();
  void maximizationClipped();

  void predict(Sequence* seq);

  double* out() {return Net->out();}
  int** getConf() {return Conf;}

  int getNErrors() { return nerrors;};

  int getNErrors_(int i) { return nerrors_[i];};
  int getClasses() { return NY;};

  int* getCounted() {return counted;}

   void resetNErrors() { 
	error=0;
	nerrors=0;
	memset(nerrors_,0,NY*sizeof(int));
	memset(counted,0,NY*sizeof(int));
	for (int p=0;p<NY;p++)
	  for (int y=0;y<NY;y++)
		Conf[p][y]=0;
	Net->resetError();
	};

  double get_error() { 
	return error;
	};
  double get_squared_error() { 
	return Net->getError();
	};
  void reset_squared_error() { 
	Net->resetError();
	};

  void setEpsilon(double eps) { 
	  Net->setEpsilon(eps);
  };


};


#endif // Model_h
