CC = clang
CFLAGS = -g -pg -Ofast -Wall -Wextra -Werror -Wpedantic

all: identify

identify: identify.o text.o parser.o ht.o bf.o bv.o node.o speck.o pq.o 
	$(CC) -g -o identify identify.o text.o parser.o ht.o bf.o bv.o node.o speck.o pq.o -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f identify *.o

format:
	clang-format -i -style=file *.[c,h]

scan-build: clean
	scan-build --use-cc=$(CC) make

