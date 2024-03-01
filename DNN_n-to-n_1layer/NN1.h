

#ifndef NN1_h
#define NN1_h 1
#include "Layer.h"
#include <math.h>

// NN1 ver. 3.02b (27/03/2023)
// Copyright (C) Gianluca Pollastri 2023
//
// NO-hidden layer Feedforward neural net.
// Input: categorical data (one-hot), real valued or mixed.
// ouput: softmax.
// Cost function: the proper one
// Gradient: plain backpropagation (no momentum)
//
// 
// Version 3.0:
// compatible with Layer 3.0
//
// Version 3.02
// Added updateWeightsL1

class NN1
{
private:
  int NI;
  int NIr;
  int NItot;
  int NO;
  int* NK;
  int* NK2;
  int which;
  int outp;
  int inp;

  double* backprop;

  Layer_soft* upper;


public:



  // Constructor. Parameters:
  // Number of input attributes, number of hidden units, number of output units;
  // t_NK contains the cardinalities of the attribute spans.

  NN1(int t_NI, int t_NO, int* t_NK) :
    NI(t_NI), NO(t_NO)
    {
      NK=new int[NI];
      NItot=0;
      for (int i=0; i<NI; i++) {
	   NK[i]=t_NK[i];
	   NItot += NK[i];
      }
      upper= new Layer_soft(NO,NK,NI);
      upper->set_output(1);
      upper->set_ninput(1);
      NIr=0;
      outp=1;
      inp=1;
    };

  // Constructor for a net with mixed inputs.
  // NI = number of input attributes (categorical inputs)
  // NIr = number of inputs (real valued)
  // ..
  // outp = output or non-output network (for backprop signal)
  // inp = input or non-inpput network (for backprop signal)


  NN1(int t_NI,int t_NIr, int t_NO, int* t_NK,
	  int t_outp=1, int t_inp=1, int t_which=1) :
	NI(t_NI), NIr(t_NIr),NO(t_NO), outp(t_outp), inp(t_inp)
    {
      int i;
      NK=new int[NI];
      NItot=0;
      for (i=0; i<NI; i++) {
	   NK[i]=t_NK[i];
	   NItot += NK[i];
      }
      NK2=new int[NIr];
      for (i=0; i<NIr; i++)
	   NK2[i]=1;

      which=1;
      upper= new Layer_soft(NO,NK,NI,NIr);

      if (outp)
        upper->set_output(1);
      if (inp)
        upper->set_ninput(1);
      backprop=new double[NItot+NIr];
    };

  NN1(NN1* from)
    {
	NI=from->NI; 
	NIr=from->NIr; 
	NO=from->NO;
	outp=from->outp;
	inp=from->inp;

      int i;
      NK=new int[NI];
      NItot=0;
      for (i=0; i<NI; i++) {
		NK[i]=from->NK[i];
		NItot += NK[i];
      }
      NK2=new int[NIr];
      for (i=0; i<NIr; i++)
		NK2[i]=1;

      which = 2;
      upper= new Layer_soft(from->upper);

      if (outp)
        upper->set_output(1);
      if (inp)
		upper->set_ninput(1);
      backprop=new double[NItot+NIr];
    };

  // Create/read a net from file
  NN1(istream& is);

~NN1() {
      delete[] NK;
      delete[] NK2;
      delete upper;
      delete[] backprop;
}

  void copy_dW(NN1* from) {
	upper->copy_dW(from->upper);
  }

  void dump_dW(ostream& os) {
	upper->dump_dW(os);
  }
  void dump_W(ostream& os) {
	upper->dump_W(os);
  }



  void read(istream& is);

  // Forward pass
  void forward(int* I);
  void forward(double* I);
  void forward(int* I1, double* I2);
  void forward(double* I1, double* I2);


  double f_cost(double* t) {
	return upper->f_cost(t);
  }

  // Backprop
  double backward(double* t, double weight=1.0);
  double* back_out() {return backprop;}

  // Update gradients
  void gradient(int* I, double* t);
  void gradient(double* I, double* t);
  void gradient(int* I1, double* I2, double* t);
  void gradient(double* I1, double* I2, double* t);

  // Update weights
  virtual void updateWeights(double epsilon);
  virtual void updateWeightsL1(double epsilon);
  virtual void updateWeightsClipped(double epsilon);
  void resetGradient();
  virtual void initWeights(int seed);
  inline double* out() { return upper->out(); };
  void write(ostream& os);

  void set_input(int vi) {
	  upper->set_ninput(vi);
	  inp=vi;
  }
  void set_output(int vo) {
	  upper->set_output(vo);
	  outp=vo;
  }


  inline int get_NI() { return NI; };
  inline int get_NIr() { return NIr; };
  inline int get_NO() { return NO; };

  double dlength() {
    return upper->dlength();
  }
};



#endif // NN1_h
