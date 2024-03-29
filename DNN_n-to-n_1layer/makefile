OBJECTST = Layer.o Model.o DNN1.o NNt.o NNt1.o NN1.o Train.o
HEADERST = Layer.h Model.h DNN1.h NNt.h NNt1.h NN1.h Sequence.h 

CC	= g++
CFLAGS  = -O3 -fopenmp

Train : $(OBJECTST)
	$(CC) $(CFLAGS) -o $@ $(OBJECTST)

%.o : %.cxx $(HEADERST)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o Train
