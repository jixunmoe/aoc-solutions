.PHONY: solve sample

CFLAGS ?= -O2 -DNDEBUG

solve.exe: solve.c
	gcc -o solve.exe $< -lm $(CFLAGS) -Wall -Wextra -Werror -std=c11

sample: solve.exe
	./solve.exe < sample.txt

solve: solve.exe
	./solve.exe < input.txt
