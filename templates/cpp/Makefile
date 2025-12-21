.PHONY: solve sample

CFLAGS ?= -O2 -DNDEBUG

solve.exe: solve.cpp
	g++ -o solve.exe $< -lm $(CFLAGS) -Wall -Wextra -Werror -std=c++20

sample: solve.exe
	./solve.exe < sample.txt

solve: solve.exe
	./solve.exe < input.txt
