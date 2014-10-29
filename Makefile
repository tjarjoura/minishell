CFLAGS = -Wall -Werror -g

all: msh.o argparse.o builtin.o expand.o
	gcc $(CFLAGS) -o msh msh.o argparse.o builtin.o expand.o
msh.o: msh.c
	gcc $(CFLAGS) -c msh.c
argparse.o: argparse.c
	gcc $(CFLAGS) -c argparse.c
expand.o: expand.c
	gcc $(CFLAGS) -c expand.c
builtin.o: builtin.c
	gcc $(CFLAGS) -c builtin.c
clean:
	rm *.o msh
