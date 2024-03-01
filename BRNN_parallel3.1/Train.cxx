
#include <math.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include "Model.h"
#include "Sequence.h"

#include <omp.h>

using namespace std;

#define NTH_TRAIN 8


class Options
{
public:
  double epsilon;
  int NU;
  int NF;
  int NB;
  int NHY;
  int NHF;
  int NHB;
  int context;
  int Moore; 
  int CoF;
  int CoB;
  int Cseg;		//Semi-size of window of averages input
  int Cwin;		//Semi-size of window on which SS are averaged

  int Classes;
  double Thresholds[128];

  int Step;
  int shortcut;
  int seed;
  int shuffle;
  int batch_blocks;
  int readModel;
  int readEpoch;
  int nEpochs;

  int adaptive;
  int reload;
  double belief;
  double threshold;
  int cycles;

  void write(ostream& os=cout) 
    {
    int i;
      cout << "epsilon " << epsilon << "\n";
      cout << "NU " << NU << "\n";
      cout << "NF " << NF << "\n";
      cout << "NB " << NB << "\n";
      cout << "NHY " << NHY << "\n";
      cout << "NHF " << NHF << "\n";
      cout << "NHB " << NHB << "\n";
      cout << "context " << context << "\n";
      cout << "Moore " << Moore << "\n";
      cout << "CoF " << CoF << "\n";
      cout << "CoB " << CoB << "\n";
      cout << "Cseg " << Cseg << "\n";
      cout << "Cwin " << Cwin << "\n";
      cout << "Classes " << Classes << "\n";
	  cout << "Thresholds ";
	  for (i=0;i<Classes-1;i++) {
	      cout << Thresholds[i] << " ";
	  }
      cout << "\n";
      cout << "Step " << Step << "\n";
      cout << "shortcut " << shortcut << "\n";
      cout << "seed " << seed << "\n";
      cout << "shuffle " << shuffle << "\n";
      cout << "batch_blocks " << batch_blocks << "\n";
      cout << "readModel " << readModel << "\n";
      cout << "readEpoch " << readEpoch << "\n";
      cout << "nEpochs " << nEpochs << "\n";
      cout << "adaptive " << adaptive << "\n";
      cout << "reload " << reload << "\n";
      cout << "belief " << belief << "\n";
      cout << "threshold " << threshold << "\n";
      cout << "cycles " << cycles << "\n";
    };
  Options(istream& is)
    {
      int i;
      char str[1024];


      while (is) {
	is >> str;
	if (!strcmp(str, "epsilon")) is >> epsilon;
	else if (!strcmp(str, "NU")) is >> NU;
	else if (!strcmp(str, "NF")) is >> NF;
	else if (!strcmp(str, "NB")) is >> NB;
	else if (!strcmp(str, "NHY")) is >> NHY;
	else if (!strcmp(str, "NHF")) is >> NHF;
	else if (!strcmp(str, "NHB")) is >> NHB;
	else if (!strcmp(str, "context")) is >> context;
	else if (!strcmp(str, "Moore")) is >> Moore; 
	else if (!strcmp(str, "CoF")) is >> CoF;
	else if (!strcmp(str, "CoB")) is >> CoB;
	else if (!strcmp(str, "Cseg")) is >> Cseg;
	else if (!strcmp(str, "Cwin")) is >> Cwin;
	else if (!strcmp(str, "Step")) is >> Step;
	else if (!strcmp(str, "Classes")) is >> Classes;
	else if (!strcmp(str, "Thresholds")) {
		for (i=0;i<Classes-1;i++)
			is >> Thresholds[i];
	}
	else if (!strcmp(str, "shortcut")) is >> shortcut;
	else if (!strcmp(str, "seed")) is >> seed;
	else if (!strcmp(str, "shuffle")) is >> shuffle;
	else if (!strcmp(str, "batch_blocks")) is >> batch_blocks;
	else if (!strcmp(str, "readModel")) is >> readModel;
	else if (!strcmp(str, "readEpoch")) is >> readEpoch;
	else if (!strcmp(str, "nEpochs")) is >> nEpochs;
	else if (!strcmp(str, "adaptive")) is >> adaptive;
	else if (!strcmp(str, "reload")) is >> reload; 
	else if (!strcmp(str, "belief")) is >> belief; 
	else if (!strcmp(str, "threshold")) is >> threshold; 
	else if (!strcmp(str, "cycles")) is >> cycles; 
      }
    };
};



int Errcomp=10000000;
void
load(int epoch, Model* M)
{
  filebuf inbuf;
  char fname[1024];
  sprintf(fname, "trained-%d.model", epoch);
  if (inbuf.open(fname, ios::in) != 0) {
    istream is(&inbuf);
    M->read(is);
  } else {
    cout << "Failed to read file " << fname;
    exit;
  }
  inbuf.close();
}

void
save(int epoch, Model* M)
{
  filebuf outbuf;
  char fname[1024];
  sprintf(fname, "trained-%d.model", epoch);
  if (outbuf.open(fname, ios::out) != 0) {
    ostream os(&outbuf);
    M->write(os);
  } else {
    cout << "Failed to write to file " << fname;
    exit;
  }
  outbuf.close();
}




void
shuffle(DataSet& D, int* pos)
{
  // Shuffle training set positions
  for (int k=0; k<D.length; k++) {
//    int p1= (int)(drand48()*D.length);
//    int p2= (int)(drand48()*D.length);
    int f1=rand();
    int f2=rand();
    int p1= (int)((double)f1/(1.0+(double)(RAND_MAX))*D.length);
    int p2= (int)((double)f2/(1.0+(double)(RAND_MAX))*D.length);
    int tmp=pos[p1];
    pos[p1]=pos[p2];
    pos[p2]=tmp;
  }
}



void
save_map(int epoch, Model* M)
{
  filebuf outbuf;
  int i,j;
  char fname[1024];
  sprintf(fname, "trained-%d.map", epoch);
  if (outbuf.open(fname, ios::out) != 0) {
    ostream os(&outbuf);
    for (i=0;i<101;i++) {
		for (j=0;j<101;j++) {
			os << M->getConf()[i][j] << " ";
		}
		os << "\n";
	}

  } else {
    cout << "Failed to read file " << fname;
    exit;
  }
  outbuf.close();
}


void
evaluate(Model* M, DataSet& D, char* which, int cycles)
{
	int y;

  cout << "\n counting_" << which << "_errors" << flush;
  M->resetNErrors();
  int p;
#pragma omp parallel num_threads(NTH_TRAIN)
{
#pragma omp for
  for (p=0; p<D.length; p++) {
    M->predict(D.seq[p], cycles);
    if (p%200==0) cout << "." << flush;
  }
}

double a[128];
double all=0;

for (y=0;y<M->getClasses();y++) {
	a[y]=M->getCounted()[y];
	all += M->getCounted()[y];
}

  cout << "\n\n" << which << cycles<<"_NErrors= " << M->getNErrors() << "/" << all;
  cout << " " << (double)M->getNErrors()/(double)(all)*100.0;

//  cout << "\n\n" << which << cycles << "_SError= " << sqrt(M->get_error()/all);

for (y=0;y<M->getClasses();y++) {
  cout << "\nClass" << y << cycles<<"= " << M->getNErrors_(y) << "/" << a[y];
  cout << "\t" << (double)M->getNErrors_(y)/(double)a[y]*100.0;
}
//  save_map(0,M);

  if ((strncmp(which,"test",4)==0) && (M->getNErrors()<Errcomp))
	{
	save(-10,M);
	Errcomp = M->getNErrors();
	}

  cout<<"\n";
}






void
train(Model* M, DataSet& D, DataSet& T, Options& Opt)
{

  int Gui = Opt.adaptive;
	     //Number of steps at increasing error before
	     //rescaling the learning rate.
  int gui=0;
  int cy;

  double ep=Opt.epsilon/(double)D.totSize;
  double ep0=ep;
  if (Opt.batch_blocks>1) {
    ep *= (double)(Opt.batch_blocks-1);
  }
  cout << "Actual lrate= " << ep << "\n";
  M->setEpsilon(ep);
  cout << "Start learning\n";
  int* wait=new int[D.length];
  for (int p=0; p<D.length; p++) {
    wait[p]=0;
  }
//  srand48(9199298);
  srand(Opt.seed);

  int* pos=new int[D.length];
  for (int pp=0; pp<D.length; pp++) pos[pp]=pp;
  double previous_squared_error=1e35;

  
  for (int epoch=Opt.readEpoch+1; epoch<=Opt.readEpoch + Opt.nEpochs; epoch++) {
    if (Opt.shuffle) shuffle(D, pos);


    M->reset_squared_error();
	double sofar=0;
    int batch_cnt=0;

    int blocksize = D.length/Opt.batch_blocks;

int tid;
int counts[NTH_TRAIN];
for (int ff=0;ff<NTH_TRAIN;ff++) counts[ff]=0;

    for (int pp=0; pp<D.length; pp+=blocksize) {

	int mx = pp+blocksize-1;
	if (mx>=D.length) {mx=D.length-1;}
	for (int i1=pp;i1<=mx;i1++) {
		for (int i2=mx;i2>i1;i2--) {
			if (D.seq[pos[i2]]->length > D.seq[pos[i2-1]]->length) {
				int tmp = pos[i2];
				pos[i2] = pos[i2-1];
				pos[i2-1] = tmp;
			}
		}
	}
//	for (int i1=pp;i1<=mx;i1++) {
//		cout << D.seq[pos[i1]]->length << " ";
//	}
//	cout << "\n" << flush;


#pragma omp parallel num_threads(NTH_TRAIN) private(tid)
{
#pragma omp for schedule(dynamic,1)
	for (int ppp = 0; ppp<blocksize; ppp++) {

	tid = omp_get_thread_num();
	counts[tid]++;


	if  ((pp+ppp)>=D.length) {continue;}
      int p= pos[pp+ppp]; // drand48()*D.length;

//#pragma omp critical
//{
//	cout << "(" << D.seq[p]->length << "," << tid << ") " << flush;
///}

      M->extimation(D.seq[p]);

      batch_cnt++;
      if ((pp+ppp)%200==0) {
			cout << "." << flush;
	  }


      } // ppp
} // pragma
//cout << "\n";
	M->maximization();

    } // += blocksize


//    M->maximization();
    double current_squared_error = M->get_squared_error();
    cout << "\nEpoch " << epoch << " Error= " << current_squared_error << " "<< M->get_squared_errorf();
    cout << " ("<<M->get_squared_errorF()<<","<<M->get_squared_errorB()<<") ";
//	for (cy=1;cy<Opt.cycles;cy++) {
//		cout << sqrt(M->getdcycles()[cy]/sofar) << " ";
//	}

//cout << "\n";
//for (int ff=0;ff<NTH_TRAIN;ff++) cout << counts[ff] << " ";
//cout << flush;

    if (current_squared_error < previous_squared_error) {
      gui=0;
      save(0,M);
      if (Gui>0) {
  //      ep += ep0*0.01;
  //      M->setEpsilon(ep);
        }
      previous_squared_error=current_squared_error;
    } else {
      gui++;
      if ((Gui) && (gui>=Gui)) {
        gui=0;
        ep *= 0.5;
        cout << "-newEpsilon(" << ep << ")"<<flush;
//		Opt.batch_blocks = (int)(0.5*Opt.batch_blocks);
//        cout << "-newBB(" << Opt.batch_blocks << ")"<<flush;

		if (Opt.reload)
        	load(0,M);
        M->setEpsilon(ep);
        }
    }

    if (epoch && epoch%10==0) {
      save(epoch, M);
		  	for (cy=1;cy<=Opt.cycles;cy++) {
				evaluate(M, D, "train", cy);
				evaluate(M, T, "test", cy);
//				D.write("train-predictions");
//				T.write("test-predictions");
			}
    }
    cout << "\n"<<flush;
  }
}



int
main(int argc, char** argv)
{
  if (argc<2) {
    cerr << "Usage: " << argv[0] << " option-file\n";
    exit(1);
  }
  ifstream optstream(argv[1]);
  Options Opt(optstream);
  Opt.write();

  Model* M;
  if (Opt.readModel) {
    char tmp[1024];
    sprintf(tmp, "trained-%d.model", Opt.readEpoch);
    cout << "Reading model from " << tmp << "\n";
    ifstream mstream(tmp);
    M = new Model(mstream);
  } else {
    cout << "Creating model\n"<<flush;

    M = new Model(Opt.NU, Opt.Classes, Opt.NHY, Opt.context, Opt.Moore, Opt.NF, Opt.NB, Opt.NHF,
		Opt.CoF, Opt.CoB, Opt.Cseg, Opt.Cwin, Opt.Step, Opt.shortcut, Opt.Thresholds, Opt.cycles);

    cout << "Generating random parameters\n"<<flush;
    M->randomize(Opt.seed);
    save(-1, M);
    Opt.readEpoch = 0;
  }

  cout << "Reading train dataset\n"<<flush;
  ifstream dstream("train.dataset");
  DataSet D(dstream);
//  D.set_belief(Opt.belief);
  cout << "Reading test dataset\n"<<flush;
  ifstream tstream("test.dataset");
  DataSet T(tstream);
//  T.set_belief(Opt.belief);
  
  train(M, D, T, Opt);

  return 0;
}


