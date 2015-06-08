.PHONY: all clean

EXECUTABLE := stats
FRAMEWORKS := -framework IOKit -framework CoreFoundation

all: $(EXECUTABLE)

$(EXECUTABLE): utils.o smc.o main.o
	gcc $(FRAMEWORKS) -lncurses $^ -o $(EXECUTABLE)

%.o: %.c
	gcc -o $@ -c $<

clean:
	rm -rf *.o stats
