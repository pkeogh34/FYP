
// NNr ver. 3.02 (27/11/2003)
//
// Copyright (C) Gianluca Pollastri 2003


#include "NNr.h"


NNr::NNr(istream& is)
{
  is >> NO >> NH >> NI >> NIr >> which >> outp >> inp;
  upper= new Layer(is);
  if (outp)
    upper->set_output(1);
  upper->set_ninput(2);

  lower=new Layer_tanh(is);
  lower->set_ninput(inp);

  int i;
  NK=new int[NI];
  NItot=0;
  for (i=0; i<NI; i++) {
	NK[i]=lower->get_NK()[i];
	NItot += NK[i];
  }
  NK2=new int[NIr];
  for (i=0; i<NIr; i++)
	NK2[i]=lower->get_NK()[NI+i];
  backprop=new double[NItot+NIr];
}


void
NNr::read(istream& is)
{
  is >> NO >> NH >> NI >> NIr >> which >> outp >> inp;
  upper->read(is);
  if (outp)
  	upper->set_output(1);
  upper->set_ninput(2);

  lower->read(is);
  lower->set_ninput(inp);

  int i;
  NItot =0;
  for (i=0; i<NI; i++) {
	NK[i]=lower->get_NK()[i];
	NItot += NK[i];
  }
  for (i=0; i<NIr; i++)
	NK2[i]=lower->get_NK()[NI+i];
}



void
NNr::forward(int* I)
{
  lower->forward(I);
  upper->forward(lower->out(),lower->out());
}

void
NNr::forward(double* I)
{
  lower->forward(I);
  upper->forward(lower->out(),lower->out());
}

void
NNr::forward(int* I1,double* I2)
{
  lower->forward(I1,I2);
  upper->forward(lower->out(),lower->out());
}

void
NNr::forward(double* I1,double* I2)
{
  lower->forward(I1,I2);
  upper->forward(lower->out(),lower->out());
}

double
NNr::backward(double* t, double weight)
{
  double err=upper->backward(t,weight);
  double BKD[1024];
  for (int i=0;i<NH;i++)
	BKD[i]=upper->back_out()[i];
  lower->backward(BKD,weight);
  if (inp==1)
    for (int r=NItot;r<NItot+NIr;r++)
      backprop[r]=lower->back_out()[r];
  else if (inp==2)
    for (int r=0;r<NItot+NIr;r++)
      backprop[r]=lower->back_out()[r];
  return err;
}

void
NNr::gradient(int* I, double* t)
{
  upper->gradient();
  lower->gradient(I);
}
void
NNr::gradient(double* I, double* t)
{
  upper->gradient();
  lower->gradient(I);
}
void
NNr::gradient(int* I1,double* I2, double* t)
{
  upper->gradient();
  lower->gradient(I1,I2);
}
void
NNr::gradient(double* I1,double* I2, double* t)
{
  upper->gradient();
  lower->gradient(I1,I2);
}


void
NNr::resetGradient()
{
  lower->resetGradient();
  upper->resetGradient();
}

void
NNr::updateWeights(double epsilon)
{
  lower->updateWeights(epsilon);
  upper->updateWeights(epsilon);
}

void
NNr::updateWeightsL1(double epsilon)
{
  lower->updateWeightsL1(epsilon);
  upper->updateWeightsL1(epsilon);
}

void
NNr::updateWeightsClipped(double epsilon)
{
  lower->updateWeightsClipped(epsilon);
  upper->updateWeightsClipped(epsilon);
}

void
NNr::initWeights(int seed)
{
  lower->initWeights(seed);
  upper->initWeights(seed);
}


void
NNr::write(ostream& os)
{
  os << NO << " " << NH<< " " << NI<< " " << NIr <<" ";
  os << which << " " << outp << " " << inp << "\n";
  upper->write(os);
  lower->write(os);
}



