XX = g++
CFLAGS = -std=c++14 -g -Wall

declination.exe : declination.cpp compartment.hpp player.hpp
	$(CXX) $(CFLAGS) -o $@ $<
