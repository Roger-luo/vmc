.PHONY: all clean

.DEFAULT: all


CC = mpicc

CXXFLAGS = -lm -O3 -I/usr/lib/openmpi/include

TARGETS = test
OBJS = vmc.o model.o randlib.o test.o statistics.o

all: $(TARGETS) general.h

clean:
	rm -f $(TARGETS) $(OBJS)

$(TARGETS) : %: $(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $(OBJS)
vmc.o: vmc.c vmc.h
	$(CC) $(CXXFLAGS) -c $^
model.o: model.c model.h
	$(CC) $(CXXFLAGS) -c $^
random.o: randlib.c randlib.h
	$(CC) $(CXXFLAGS) -c $^
statistics.o: statistics.c
	$(CC) $(CXXFLAGS) -c $^
