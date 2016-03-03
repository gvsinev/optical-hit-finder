#
# Makefile for the optical hit finder testing package 
# By Gleb Sinev, gleb.v.sinev@gmail.com
#

OBJS     := RunHitFinder.o
CXX      := g++
CXXFLAGS := -std=c++14 -Wall
INCLUDES := -I${FHICLCPP_INC} -I${CETLIB_INC} -I${CPP0X_INC} -I${BOOST_INC}
LDFLAGS  := -Wall
LIBS     := -L${FHICLCPP_LIB} -lfhiclcpp -L${CETLIB_LIB} -lcetlib
RM       := rm

run-hit-finder: $(OBJS)
	$(CXX) $(LDFLAGS) -o run-hit-finder $(OBJS) $(LIBS)

RunHitFinder.o: RunHitFinder.cxx
	$(CXX) -c $(CXXFLAGS) RunHitFinder.cxx $(INCLUDES)

clean:
	$(RM) $(OBJS) run-hit-finder
