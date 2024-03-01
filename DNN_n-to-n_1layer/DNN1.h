
#ifndef DRNN1_h
#define DRNN1_h 1

#include <stdlib.h>
#include <fstream>
#include <math.h>
#include "NN1.h"
#include "NNt.h"
#include "NNt1.h"

#define MAX 8192

#define NTH 12



// DNN1 ver. 0.01 (22/03/2023)
// Copyright (C) Gianluca Pollastri 2023
//
// Convolutional Neural Network for sequence-to-sequence mapping
// inputs real valued or categorical
//
// Notice: One-layered kernels here



class DNN1 {
 private:
  int NU;
  int NY;

  int NLayers;				//Total number of hidden layers
  int gamma, context;			//hidden-to-hidden and input-to-hidden contexts
  int Hi,NHi;

  int* Ht;


  int doubleo;
  int modular;


  NN1* NetO;
  NNt1** NetT;
  NNt* NetI;



  double*** Hcache;  		//(NLayers + 1), seq_len, Ht
  double*** Hcachebp;  	//(NLayers + 1), seq_len, Ht 

  double* HcacheGlobal;
  double* HcacheGlobalbp; 

  double* Y;			//NY x seq_len
  double* BP;			//NU x seq_len

  double error;

  double epsilon;

  void alloc();


 public:

  DNN1(int NU, int NY, int NLayers, int* Ht, int NHi, int Hi, int gamma, int context, int doubleo=0);
  DNN1(istream & is, int NU, int NY, int NLayers, int* Ht, int NHi, int Hi, int gamma, int context, int doubleo=0);
  DNN1(istream& is);
  void read(istream& is);
  void write(ostream& os);

  void resetGradient();
  void initWeights(int seed);
  void initWeights_top(int seed);

  void I_T(double* seq, int t, int length);
  void I_T(int* seq, int t, int length);
  void T_T(int Lay, int t, int length);
  void T_O(int t, int length);


  void I_T(double* seq, int t1, int t2, int length);
  void I_T(int* seq, int t1, int t2, int length);
  void T_T(int Lay, int t1, int t2, int length);
  void T_O(int t1, int t2, int length);


  void make_global_input(int length);

  void propagate(double* seq, int length);
  void propagate(int* seq, int length);

//  void forward(double* seq, int t, int length);
//  void forward(int* seq, int t, int length);

  void I_Tbp(double* seq, int t, int length);
  void I_Tbp(int* seq, int t, int length);
  void T_Tbp(int Lay, int t, int length);
  void T_Obp(double* target, int t, int length);
  void T_Obp(int target, int t, int length);

  void I_Tbp(double* seq, int t1, int t2, int length);
  void I_Tbp(int* seq, int t1, int t2, int length);
  void T_Tbp(int Lay, int t1, int t2, int length);
  void T_Obp(double** target, int t1, int t2, int length);
  void T_Obp(int* target, int t1, int t2, int length);




//  void back_propagate(double* seq, int length, int backp=0);
//  void back_propagate(int* seq, int length, int backp=0);

  void clear_cache();

  void make_global_backprop(int length);

  void extimation(double* seq, int* y, int length, int backp=0);
  void extimation(int* seq, int* y, int length, int backp=0);

  void extimation(double* seq, double* y, int length, int backp=0);
  void extimation(int* seq, double* y, int length, int backp=0);




  void maximization();
  void maximizationL1();
  void maximizationClipped();

  void Feed(double* seq, int length);
  void Feed(int* seq, int length);
  void predict(double* seq, int length);
  void predict(int* seq, int length);


  double* out() {return Y;}
  double* back_out() {return BP;}




  double getError() {
	return error;
	};
  void resetError() {
	error=0.0;
	};

  void setEpsilon(double eps) { epsilon=eps; };


};


#endif // DNN1_h
