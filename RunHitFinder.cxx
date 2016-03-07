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

// CETLib includes
#include "cetlib/filepath_maker.h"

// LArSoft includes
#include "lardata/RawData/OpDetWaveform.h"

// ROOT includes
#include "TFile.h"
#include "TH1D.h"

// C++ includes
#include <iostream>
#include <string>
#include <vector>

// Function declarations
fhicl::ParameterSet GetParameterSet(std::string const& FHiCLFileName);
TH1D GetHistogram(TFile            & ROOTFile,
                  std::string const& histogramName);

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

  std::string ROOTFileName("./waveforms.root"); 
  TFile inputFile(ROOTFileName.c_str(), "READ");
  std::string histogramName("event_1_opchannel_0_waveform_0");
  TH1D histogram = GetHistogram(inputFile, histogramName);

  std::string outputFigure("test");
  std::string format("C");
  histogram.SaveAs((outputFigure + '.' + format).c_str(), format.c_str());

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

//------------------------------------------------------------------------------
// Input is a LArSoft file with waveform histograms
TH1D GetHistogram(TFile            & ROOTFile,
                  std::string const& histogramName)
{

  std::string directoryName("opdigiana");
  TH1D *histogram = 
    dynamic_cast< TH1D* >(ROOTFile.GetDirectory(directoryName.c_str())
                                 ->Get(histogramName.c_str()));

  return *histogram;

}
