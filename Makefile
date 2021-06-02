BINARIES = benchmark check depth_map
CC = gcc
CFLAGS = -O3 -DNDEBUG -g0 -std=c99 -Wall
 
OMP_NESTED = TRUE

default: clean check benchmark

debug: clean all_debug

queue: clean benchmark
	qsub queue.sh

all: $(BINARIES)

all_debug: CFLAGS += -DDEBUG -ggdb3
all_debug: all

benchmark: benchmark.o calc_depth_naive.o calc_depth_optimized.o utils.o
	$(CC) $(CFLAGS) -o benchmark benchmark.o calc_depth_naive.o calc_depth_optimized.o utils.o -lm 

check: calc_depth_naive.o calc_depth_optimized.o check.o utils.o
	$(CC) $(CFLAGS) -o check calc_depth_naive.o calc_depth_optimized.o check.o utils.o -lm

depth_map: calc_depth_naive.o calc_depth_optimized.o depth_map.o utils.o
	$(CC) $(CFLAGS) -o calc_depth calc_depth_naive.o calc_depth_optimized.o depth_map.o utils.o -lm

clean:
	rm -rf *.o
	rm -rf $(BINARIES)
