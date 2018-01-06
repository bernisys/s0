PROG = s0
CFLAGS = -Wall -g
LDFLAGS = -lwiringPi

OBJ = udp_broadcast.o s0.o

all: $(OBJ) $(PROG)


clean:
	rm -f s0 *.o

$(PROG): $(OBJ) main.c
	gcc $(LDFLAGS) -o $(PROG) $(OBJ)


%.o: %.c
	gcc -c $<

