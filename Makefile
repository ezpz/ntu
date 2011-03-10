# Trivial makefile

CPP=g++
FLAGS=-W -Wall -Wextra -Werror -pedantic -ansi
BIN=ntu_test

default: 
	$(CPP) $(FLAGS) ntu.cc -o $(BIN)

clean:
	rm -f $(BIN)
