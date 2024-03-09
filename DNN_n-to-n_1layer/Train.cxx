
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "Model.h"
#include "Sequence.h"

using namespace std;

class Options {
public:
	double epsilon;
	int NLayers;
	int NU;
	int *Ht;
	int NHi;
	int Hi;

	int gamma;
	int context;

	int Classes;
	double Thresholds[128];

	int seed;
	int shuffle;
	int batch_blocks;
	int readModel;
	int readEpoch;
	int nEpochs;

	int adaptive;
	int reload;

	void write(ostream &os = cout) {
		int i;
		cout << "epsilon " << epsilon << "\n";
		cout << "NLayers " << NLayers << "\n";
		cout << "NU " << NU << "\n";
		cout << "Ht ";
		for (i = 0; i < NLayers; i++) {
			cout << Ht[i] << " ";
		}
		cout << "\n";
		cout << "NHi " << NHi << "\n";
		cout << "Hi " << Hi << "\n";

		cout << "context " << context << "\n";
		cout << "gamma " << gamma << "\n";

		cout << "Classes " << Classes << "\n";
		cout << "Thresholds ";
		for (i = 0; i < Classes - 1; i++) {
			cout << Thresholds[i] << " ";
		}
		cout << "\n";

		cout << "seed " << seed << "\n";
		cout << "shuffle " << shuffle << "\n";
		cout << "batch_blocks " << batch_blocks << "\n";
		cout << "readModel " << readModel << "\n";
		cout << "readEpoch " << readEpoch << "\n";
		cout << "nEpochs " << nEpochs << "\n";
		cout << "adaptive " << adaptive << "\n";
		cout << "reload " << reload << "\n";
	};
	Options(istream &is) {
		int i;
		char str[1024];

		while (is) {
			is >> str;
			if (!strcmp(str, "epsilon"))
				is >> epsilon;
			else if (!strcmp(str, "NLayers")) {
				is >> NLayers;
				Ht = new int[NLayers];
			} else if (!strcmp(str, "NU"))
				is >> NU;
			else if (!strcmp(str, "Ht")) {
				for (i = 0; i < NLayers; i++)
					is >> Ht[i];
			} else if (!strcmp(str, "NHi"))
				is >> NHi;
			else if (!strcmp(str, "Hi"))
				is >> Hi;

			else if (!strcmp(str, "context"))
				is >> context;
			else if (!strcmp(str, "gamma"))
				is >> gamma;
			else if (!strcmp(str, "Classes"))
				is >> Classes;
			else if (!strcmp(str, "Thresholds")) {
				for (i = 0; i < Classes - 1; i++)
					is >> Thresholds[i];
			} else if (!strcmp(str, "seed"))
				is >> seed;
			else if (!strcmp(str, "shuffle"))
				is >> shuffle;
			else if (!strcmp(str, "batch_blocks"))
				is >> batch_blocks;
			else if (!strcmp(str, "readModel"))
				is >> readModel;
			else if (!strcmp(str, "readEpoch"))
				is >> readEpoch;
			else if (!strcmp(str, "nEpochs"))
				is >> nEpochs;
			else if (!strcmp(str, "adaptive"))
				is >> adaptive;
			else if (!strcmp(str, "reload"))
				is >> reload;
		}
	};
};

int Errcomp = 10000000;
void load(int epoch, Model *M) {
	filebuf inbuf;
	char fname[1024];
	snprintf(fname, sizeof(fname), "trained-%d.model", epoch);
	if (inbuf.open(fname, ios::in) != 0) {
		istream is(&inbuf);
		M->read(is);
	} else {
		cout << "Failed to read file " << fname;
	}
	inbuf.close();
}

void save(int epoch, Model *M) {
	filebuf outbuf;
	char fname[1024];
	snprintf(fname, sizeof(fname), "/app/data/trained-%d.model", epoch);
	if (outbuf.open(fname, ios::out) != 0) {
		ostream os(&outbuf);
		M->write(os);
	} else {
		cout << "Failed to write to file " << fname;
	}
	outbuf.close();
}

void shuffle(DataSet &D, int *pos) {
	// Shuffle training set positions
	for (int k = 0; k < D.length; k++) {
		//    int p1= (int)(drand48()*D.length);
		//    int p2= (int)(drand48()*D.length);
		int f1 = rand();
		int f2 = rand();
		int p1 = (int)((double)f1 / (1.0 + (double)(RAND_MAX)) * D.length);
		int p2 = (int)((double)f2 / (1.0 + (double)(RAND_MAX)) * D.length);
		int tmp = pos[p1];
		pos[p1] = pos[p2];
		pos[p2] = tmp;
	}
}

/*
void
save_map(int epoch, Model* M)
{
  filebuf outbuf;
  int i,j;
  char fname[1024];
  snprintf(fname, sizeof(fname), "trained-%d.map", epoch);
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
*/

void evaluate(Model *M, DataSet &D, const char *which) {
	int y;

	cout << "\n counting_" << which << "_errors" << flush;
	M->resetNErrors();
	int p;
	for (p = 0; p < D.length; p++) {
		M->predict(D.seq[p]);
		if (p % 200 == 0)
			cout << "." << flush;
	}

	double a[128];
	double all = 0;

	for (y = 0; y < M->getClasses(); y++) {
		a[y] = M->getCounted()[y];
		all += M->getCounted()[y];
	}

	cout << "\n\n"
		 << which << "_NErrors= " << M->getNErrors() << "/" << all;
	cout << " " << (double)M->getNErrors() / (double)(all) * 100.0;

	for (y = 0; y < M->getClasses(); y++) {
		cout << "\nClass" << y << "= " << M->getNErrors_(y) << "/" << a[y];
		cout << "\t" << (double)M->getNErrors_(y) / (double)a[y] * 100.0;
	}

	if ((strncmp(which, "test", 4) == 0) && (M->getNErrors() < Errcomp)) {
		save(-10, M);
		Errcomp = M->getNErrors();
	}

	cout << "\n";
}

void train(Model *M, DataSet &D, DataSet &T, Options &Opt) {

	int Gui = Opt.adaptive;
	// Number of steps at increasing error before
	// rescaling the learning rate.
	int gui = 0;
	int cy;

	double ep = Opt.epsilon / (double)D.totSize;
	double ep0 = ep;
	if (Opt.batch_blocks > 1) {
		ep *= (double)(Opt.batch_blocks - 1);
	}
	cout << "Actual lrate= " << ep << "\n";
	M->setEpsilon(ep);
	cout << "Start learning\n";
	int *wait = new int[D.length];
	for (int p = 0; p < D.length; p++) {
		wait[p] = 0;
	}

	srand(Opt.seed);

	int *pos = new int[D.length];
	for (int pp = 0; pp < D.length; pp++)
		pos[pp] = pp;
	double previous_squared_error = 1e35;

	for (int epoch = Opt.readEpoch + 1; epoch <= Opt.readEpoch + Opt.nEpochs; epoch++) {
		if (Opt.shuffle)
			shuffle(D, pos);

		M->reset_squared_error();
		double sofar = 0;
		int batch_cnt = 0;
		for (int pp = 0; pp < D.length; pp++) {
			int p = pos[pp]; // drand48()*D.length;
			wait[p]++;
			if (wait[p] <= 0) {
				cout << p << ",";
				continue;
			}
			//    cout << (D.seq[p])->name << "\n" << flush;
			M->extimation(D.seq[p]);
			//      cout << "*" << flush;
			sofar += (D.seq[p])->length;

			batch_cnt++;
			if (batch_cnt >= D.length / Opt.batch_blocks &&
				D.length - pp >= D.length / Opt.batch_blocks) {
				M->maximization();
				//		M->maximizationClipped();
				batch_cnt = 0;
			}
			if (pp % 200 == 0) {
				cout << "." << flush;
			}
		}
		//    M->maximizationClipped();
		M->maximization();
		double current_squared_error = M->get_squared_error();
		cout << "\nEpoch " << epoch << " Error= " << current_squared_error;

		if (current_squared_error < previous_squared_error) {
			gui = 0;
			save(0, M);
			if (Gui > 0) {
				//      ep += ep0*0.01;
				//      M->setEpsilon(ep);
			}
			previous_squared_error = current_squared_error;
		} else {
			gui++;
			if ((Gui) && (gui >= Gui)) {
				gui = 0;
				ep *= 0.5;
				cout << "-newEpsilon(" << ep << ")" << flush;
				if (Opt.reload)
					load(0, M);
				M->setEpsilon(ep);
			}
		}

		if (epoch && epoch % 10 == 0) {
			save(epoch, M);
			evaluate(M, D, "train");
			evaluate(M, T, "test");
			//	 D.write("train-predictions");
			//	 T.write("test-predictions");
		}
		cout << "\n"
			 << flush;
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " option-file\n";
		exit(1);
	}
	ifstream optstream(argv[1]);
	Options Opt(optstream);
	Opt.write();

	Model *M;
	if (Opt.readModel) {
		char tmp[1024];
		snprintf(tmp, sizeof(tmp), "trained-%d.model", Opt.readEpoch);
		cout << "Reading model from " << tmp << "\n";
		ifstream mstream(tmp);
		M = new Model(mstream);
	} else {
		cout << "Creating model\n"
			 << flush;

		M = new Model(Opt.NU, Opt.Classes, Opt.NLayers, Opt.Ht, Opt.NHi, Opt.Hi, Opt.gamma, Opt.context, Opt.Thresholds);

		cout << "Generating random parameters\n"
			 << flush;
		M->randomize(Opt.seed);
		save(-1, M);
		Opt.readEpoch = 0;
	}

	cout << "Reading train dataset\n"
		 << flush;
	ifstream dstream("train.dataset");
	if (!dstream.is_open()) {
		cerr << "Error opening 'train.dataset': " << strerror(errno) << endl;
		return 1;
	}
	DataSet D(dstream);
	dstream.close();

	cout << "Reading test dataset\n"
		 << flush;
	ifstream tstream("test.dataset");
	if (!tstream) {
		cerr << "Error opening 'test.dataset': " << strerror(errno) << endl;
		return 1;
	}
	DataSet T(tstream);
	tstream.close();

	train(M, D, T, Opt);

	return 0;
}
