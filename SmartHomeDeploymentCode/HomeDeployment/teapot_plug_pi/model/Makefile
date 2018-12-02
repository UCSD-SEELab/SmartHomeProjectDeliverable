CC=g++
CFLAGS=-I -Wall
DEPS = NeuralNetowork.h weights.h shape.h
OBJ = NNCompute.o NeuralNetwork.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

compute: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o
