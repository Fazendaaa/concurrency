DEPS = main.c GJ.c matrix.c
OBJS = main.o GJ.o matrix.o
EXEC = rref -lm

%.o: %.c $(DEPS)
	mpicc -c -fopenmp -lm $^

all: $(OBJS)
	mpicc -g -Wall -fopenmp -lm $^ -o $(EXEC)

clean:
	rm -rf *.o $(EXEC)

#test structure

#Homemade Tests
run:
	export OMP_NUM_THREADS=1 ;\
		mpirun -np 3 $(EXEC) 5

#2 cluster nodes
2NodesSingle1:
	export OMP_NUM_THREADS=1 ;\
		mpirun -hostfile ../hosts -np 2 $(EXEC)

2Nodes4Threads1:
	export OMP_NUM_THREADS=4 ;\
		mpirun -hostfile ../hosts -np 2 $(EXEC)

2Nodes8Threads1:
	export OMP_NUM_THREADS=8 ;\
		mpirun -hostfile ../hosts -np 2 $(EXEC)

#4 cluster nodes
4NodesSingle1:
	export OMP_NUM_THREADS=1 ;\
		mpirun -hostfile ../hosts -np 4 $(EXEC)

4Nodes4Threads1:
	export OMP_NUM_THREADS=4 ;\
		mpirun -hostfile ../hosts -np 4 $(EXEC)

4Nodes8Threads1:
	export OMP_NUM_THREADS=8 ;\
		mpirun -hostfile ../hosts -np 4 $(EXEC)

#8 cluster nodes
8NodesSingle1:
	export OMP_NUM_THREADS=1 ;\
		mpirun -hostfile ../hosts -np 8 $(EXEC)

8Nodes4Threads1:
	export OMP_NUM_THREADS=4 ;\
		mpirun -hostfile ../hosts -np 8 $(EXEC)

8Nodes8Threads1:
	export OMP_NUM_THREADS=8 ;\
		mpirun -hostfile ../hosts -np 8 $(EXEC)
