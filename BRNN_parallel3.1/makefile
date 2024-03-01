OBJECTST = Layer.o Model.o BRNN.o NNt.o NN.o Train.o
HEADERST = Layer.h Model.h BRNN.h NNt.h NN.h Sequence.h 

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

NN.o : NN.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx
	
BRNN.o : BRNN.cxx $(HEADERST)
	$(CC) -c $(CFLAGS) $*.cxx


clean:
	rm *.o Train
