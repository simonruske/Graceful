CXX = g++
CXXFLAGS = -O2

GracefulConsole: GracefulConsole.o Graceful.o Reader.o Logger.o Initialise.o
	mpicxx -O2 -o program GracefulConsole.o Graceful.o Reader.o Logger.o Initialise.o

GracefulConsole.o: GracefulConsole/GracefulConsole.cpp
	mpicxx -c GracefulConsole/GracefulConsole.cpp

Graceful.o: Graceful/Graceful.h Graceful/Initialise.h Graceful/SolutionState.h
	$(CXX) $(CXXFLAGS) -c Graceful/Graceful.cpp

Reader.o: Graceful/Reader.cpp Graceful/Reader.h
	$(CXX) $(CXXFLAGS) -c Graceful/Reader.cpp

Logger.o: Graceful/Logger.cpp Graceful/Logger.h
	$(CXX) $(CXXFLAGS) -c Graceful/Logger.cpp

Initialise.o: Graceful/Initialise.h Graceful/SolutionState.h
	$(CXX) $(CXXFLAGS) -c Graceful/Initialise.cpp

SolutionState.o: Graceful/SolutionState.h
