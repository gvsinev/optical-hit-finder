#
# Makefile for the optical hit finder testing package 
# By Gleb Sinev, gleb.v.sinev@gmail.com
#

OBJS     := RunHitFinder.o
CXX      := g++
CXXFLAGS := -std=c++14 -Wall
LDFLAGS  := -Wall
RM       := rm


FHICL_INCS   := -I${FHICLCPP_INC} -I${CETLIB_INC} -I${CPP0X_INC} -I${BOOST_INC}
LARSOFT_INCS := -I${LARDATA_INC}
INCLUDES     := $(FHICL_INCS) $(LARSOFT_INCS) -I${ROOT_INC}

FHICL_LIBS   := -L${FHICLCPP_LIB} -lfhiclcpp -L${CETLIB_LIB} -lcetlib
LARSOFT_LIBS := -L${LARDATA_LIB} -llardata_RawData
LIBS         := $(FHICL_LIBS) $(LARSOFT_LIBS) $(shell root-config --libs)

run-hit-finder: $(OBJS)
	$(CXX) $(LDFLAGS) -o run-hit-finder $(OBJS) $(LIBS)

RunHitFinder.o: RunHitFinder.cxx
	$(CXX) -c $(CXXFLAGS) RunHitFinder.cxx $(INCLUDES)

clean:
	$(RM) $(OBJS) run-hit-finder
