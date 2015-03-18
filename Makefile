PROGRAM_NAME := stats
FRAMEWORKS := -framework IOKit -framework CoreFoundation

all: utils.o smc.o main.o
	gcc $(FRAMEWORKS) -lncurses $^ -o $(PROGRAM_NAME)

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o stats
