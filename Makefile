PROG = s0

all: clean $(PROG)

s0 s0.c:
	gcc -lwiringPi -o s0 s0.c

clean:
	rm -f s0
