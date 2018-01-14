PROG = s0
CFLAGS = -Wall -g
LDFLAGS = -lwiringPi

OBJ = udp_broadcast.o time.o s0.o gpio.o

SRC = $(OBJ:%.o=%.c)
HDR = $(OBJ:%.o=%.h)




all: $(OBJ) $(PROG)


clean:
	rm -f $(PROG) $(OBJ)


fresh: clean all


$(PROG): $(OBJ)
	gcc $(LDFLAGS) -o $(PROG) $(OBJ)


%.o: %.c %.h
	gcc -c $(CFLAGS) $<

