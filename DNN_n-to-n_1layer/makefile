OBJECTST = Layer.o Model.o DNN1.o NNt.o NNt1.o NN1.o Train.o
HEADERST = Layer.h Model.h DNN1.h NNt.h NNt1.h NN1.h Sequence.h 

CC	= g++
CFLAGS  = -O3 -fopenmp

Train :	$(OBJECTST) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTST) $(LLIBS)

Train.o : Train.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx

Layer.o : Layer.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx

Model.o : Model.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx

NNt.o : NNt.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx

NNt1.o : NNt1.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx

NN1.o : NN1.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx
	
DNN1.o : DNN1.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx


clean:
	rm *.o Train
