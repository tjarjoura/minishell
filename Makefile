CFLAGS = -Wall -Werror

all: msh.c
	gcc $(CFLAGS) -o msh msh.c

clean:
	rm *.o msh
