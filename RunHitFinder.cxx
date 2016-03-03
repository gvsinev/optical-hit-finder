//==============================================================================
// RunHitFinder.cxx
// This program runs an optical hit finder on waveforms from
// the DUNE photon detector system
//
// Gleb Sinev, 2016, gleb.v.sinev@gmail.com
//==============================================================================

// FHiCL-CPP -- input parameters for the hit finder
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

// Cetlib includes
#include "cetlib/filepath_maker.h"

// C++ includes
#include <iostream>
#include <string>

// Function declarations
fhicl::ParameterSet GetParameterSet(std::string const& FHiCLFileName);

//------------------------------------------------------------------------------
//int main(int argc, char *argv[])
int main()
{

//  int numberOfArguments = 2;
//  if (argc < numberOfArguments) 
//  {
//    std::cout << "Usage: run-hit-finder FHiCL-file.fcl.\n";
//    return 1;
//  }
//
  std::cout << "Starting hit finder.\n";

  std::string FHiCLFileName("./pdreco.fcl"); 
  fhicl::ParameterSet parameterSet = GetParameterSet(FHiCLFileName);

  return 0;

}

//------------------------------------------------------------------------------
fhicl::ParameterSet GetParameterSet(std::string const& FHiCLFileName)
{

  // I think this is the way to read a FHiCL file
  fhicl::ParameterSet parameterSet;
  cet::filepath_maker filepathMaker;
  fhicl::make_ParameterSet(FHiCLFileName, filepathMaker, parameterSet);

  return parameterSet;

}
