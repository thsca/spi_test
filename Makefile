CXX ?= g++
NAVIO = ../../Navio

all:
	$(CXX) spi_test.cpp -o spi_test

clean:
	rm -f spi_test
