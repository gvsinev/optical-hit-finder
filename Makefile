#
# Makefile for the optical hit finder testing package 
# By Gleb Sinev, gleb.v.sinev@gmail.com
#

OBJS     := RunHitFinder.o
CXX      := g++
CXXFLAGS := -std=c++14 -Wall
LDFLAGS  := -Wall
RM       := rm

run-hit-finder: $(OBJS)
	$(CXX) $(LDFLAGS) -o run-hit-finder $(OBJS)

RunHitFinder.o: RunHitFinder.cxx
	$(CXX) -c $(CXXFLAGS) RunHitFinder.cxx

clean:
	$(RM) $(OBJS) run-hit-finder
