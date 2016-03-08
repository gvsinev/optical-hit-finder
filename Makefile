#
# Makefile for the optical hit finder testing package 
# By Gleb Sinev, gleb.v.sinev@gmail.com
#

OBJS     := RunHitFinder.o
CXX      := g++
CXXFLAGS := -g -std=c++14 -Wall
LDFLAGS  := -g -Wall
RM       := rm


FHICL_INCS   := -I${FHICLCPP_INC} -I${CETLIB_INC} -I${CPP0X_INC} -I${BOOST_INC}
LARSOFT_INCS := -I${LARDATA_INC} -I${LARANA_INC} -I${LARCORE_INC} \
	              -I${CLHEP_INC} -I${MESSAGEFACILITY_INC} -I${DUNETPC_INC}
INCLUDES     := $(FHICL_INCS) $(LARSOFT_INCS) -I${ROOT_INC}

FHICL_LIBS   := -L${FHICLCPP_LIB} -lfhiclcpp -L${CETLIB_LIB} -lcetlib
LARSOFT_LIBS := -L${LARDATA_LIB} -llardata_RawData      \
                                 -llardata_DetectorInfo \
																 -llardata_RecoBase     \
							  -L${LARCORE_LIB} -llarcore_Geometry     \
								-L${DUNETPC_LIB} -ldune_Geometry        \
	              -L${LARANA_LIB} -llarana_OpticalDetector_OpHitFinder
LIBS         := $(FHICL_LIBS) $(LARSOFT_LIBS) $(shell root-config --libs)

run-hit-finder: $(OBJS)
	$(CXX) $(LDFLAGS) -o run-hit-finder $(OBJS) $(LIBS)

RunHitFinder.o: RunHitFinder.cxx
	$(CXX) -c $(CXXFLAGS) RunHitFinder.cxx $(INCLUDES)

clean:
	$(RM) $(OBJS) run-hit-finder
