.PHONY: solve sample clean

CFLAGS ?= -O2 -DNDEBUG

clean:
	rm -f *.o *.exe

%.o: %.cpp
	g++ -c $< -o $@ -lm $(CFLAGS) -Wall -Wextra -Werror -std=c++20

solve.exe: solve.o machine.o
	g++ -o $@ $^ -lm $(CFLAGS) -Wall -Wextra -Werror -std=c++20

sample: solve.exe
	./solve.exe < sample.txt

solve: solve.exe
	./solve.exe < input.txt
