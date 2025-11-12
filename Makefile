COMP := g++
FLAGS := -Wall

bndm.exe: main.cpp bitvector.o bndm.o
	$(COMP) $(FLAGS) $^ -o $@

bitvector.o: src/bitvector.cpp inc/bitvector.hpp
	$(COMP) $(FLAGS) -c $< -o $@

bndm.o: src/bndm.cpp inc/bndm.hpp
	$(COMP) $(FLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm *.o
