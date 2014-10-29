CFLAGS = -Wall -Werror -g

all: msh.o argparse.o builtin.o
	gcc $(CFLAGS) -o msh msh.o argparse.o builtin.o
msh.o: msh.c
	gcc $(CFLAGS) -c msh.c
argparse.o: argparse.c
	gcc $(CFLAGS) -c argparse.c
builtin.o: builtin.c
	gcc $(CFLAGS) -c builtin.c
clean:
	rm *.o msh
