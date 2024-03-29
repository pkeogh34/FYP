OBJECTST = Layer.o Model.o BRNN.o NNt.o NN.o Train.o
HEADERST = Layer.h Model.h BRNN.h NNt.h NN.h Sequence.h 

CC      = g++
CFLAGS  = -O3 -fopenmp

Train : $(OBJECTST)
	$(CC) $(CFLAGS) -o $@ $(OBJECTST)

%.o : %.cxx $(HEADERST)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f *.o Train

