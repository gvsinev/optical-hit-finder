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
#include "larana/OpticalDetector/OpHitFinder/OpHitAlg.h"
#include "lardata/DetectorInfo/DetectorClocksStandard.h"
#include "lardata/RecoBase/OpHit.h"
#include "larana/OpticalDetector/OpHitFinder/PMTPulseRecoBase.h"
#include "larana/OpticalDetector/OpHitFinder/AlgoThreshold.h"
#include "larana/OpticalDetector/OpHitFinder/AlgoSiPM.h"
#include "larana/OpticalDetector/OpHitFinder/AlgoSlidingWindow.h"
#include "larana/OpticalDetector/OpHitFinder/AlgoFixedWindow.h"
#include "larana/OpticalDetector/OpHitFinder/AlgoCFD.h"
#include "larana/OpticalDetector/OpHitFinder/PedAlgoEdges.h"
#include "larana/OpticalDetector/OpHitFinder/PedAlgoRollingMean.h"
#include "larana/OpticalDetector/OpHitFinder/PedAlgoUB.h"
#include "larana/OpticalDetector/OpHitFinder/PulseRecoManager.h"
#include "larcore/Geometry/ChannelMapAlg.h"
#include "dune/Geometry/ChannelMap35Alg.h"
#include "dune/Geometry/ChannelMap35OptAlg.h"
#include "dune/Geometry/ChannelMapAPAAlg.h"

// ROOT includes
#include "TFile.h"
#include "TH1D.h"

// C++ includes
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// Function declarations
fhicl::ParameterSet GetParameterSet(std::string const& FHiCLFileName);
unsigned int const GetUIntArgument(size_t const argumentNumber, 
                                        int argc, char *argv[]);
TH1D GetHistogram(TFile            & ROOTFile,
                  std::string const& histogramName);
void FillWaveformFromHistogram(raw::OpDetWaveform      & waveform, 
                               TH1D               const& histogram);
std::unique_ptr< geo::GeometryCore > 
         GetGeometry(fhicl::ParameterSet const& geometryParameterSet);
std::unique_ptr< pmtana::PMTPulseRecoBase > 
  GetThresholdAlgorithm(fhicl::ParameterSet const& opHitParameterSet);
std::unique_ptr< pmtana::PMTPedestalBase >  
  GetPedestalAlgorithm (fhicl::ParameterSet const& opHitParameterSet);

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  int numberOfArguments = 3;
  if (argc != (numberOfArguments + 1)) 
  {
    std::cout << "Usage: run-hit-finder "
                 "eventNumber opChannel waveformNumber.\n";
    return 1;
  }

  std::cout << "Starting hit finder.\n";

  std::string FHiCLFileName("./pdreco.fcl"); 
  fhicl::ParameterSet parameterSet = GetParameterSet(FHiCLFileName);

  std::string ROOTFileName("./waveforms.root"); 
  TFile inputFile(ROOTFileName.c_str(), "READ");
  unsigned int const eventNumber    = GetUIntArgument(1, argc, argv);
  unsigned int const opChannel      = GetUIntArgument(2, argc, argv);
  unsigned int const waveformNumber = GetUIntArgument(3, argc, argv);
  std::string histogramName("event_"      + std::to_string(eventNumber) +
                            "_opchannel_" + std::to_string(opChannel)   +
                            "_waveform_"  + std::to_string(waveformNumber));
  std::cout << "Reading " << histogramName << ".\n";
  TH1D const histogram = GetHistogram(inputFile, histogramName);

  raw::OpDetWaveform waveform(histogram.GetBinCenter(0), opChannel);
  FillWaveformFromHistogram(waveform, histogram);
  std::vector< raw::OpDetWaveform > waveformVector;
  waveformVector.emplace_back(waveform);

  // Initialize services
  std::unique_ptr< geo::GeometryCore > geometry = 
    GetGeometry(parameterSet.get< fhicl::ParameterSet >("Geometry"));
  detinfo::DetectorClocksStandard detectorClocks
    (parameterSet.get< fhicl::ParameterSet >("DetectorClocksService"));

  std::vector< recob::OpHit > opHitVector;

  // Get hit finder FHiCL parameters
  fhicl::ParameterSet opHitParameterSet = 
                     parameterSet.get< fhicl::ParameterSet >("ophit");

  // Auxiliary algorithm initialization
  pmtana::PulseRecoManager  pulseRecoManager; 
  std::unique_ptr< pmtana::PMTPulseRecoBase > thresholdAlgorithm = 
                             GetThresholdAlgorithm(opHitParameterSet);
  std::unique_ptr< pmtana::PMTPedestalBase >  pedestalAlgorithm  =
                              GetPedestalAlgorithm(opHitParameterSet);
  pulseRecoManager.AddRecoAlgo(thresholdAlgorithm.get());
  pulseRecoManager.SetDefaultPedAlgo(pedestalAlgorithm.get());

  // Other hit finder parameters
  float hitThreshold = opHitParameterSet.get< float >("HitThreshold");
  float areaToPE     = opHitParameterSet.get< bool  >("AreaToPE"    );
  size_t maxOpChannel = geometry->MaxOpChannel();
  std::vector< double > SPESize(maxOpChannel + 1, 
                           opHitParameterSet.get< float >("SPEArea"));

  // Run the hit finding algorithm
  opdet::RunHitFinder(waveformVector,
                      opHitVector,
                      pulseRecoManager,
                      *thresholdAlgorithm,
                      *geometry,
                      hitThreshold,
                      detectorClocks,
                      SPESize,
                      areaToPE);

//  std::string outputFigure("test");
//  std::string format("C");
//  histogram.SaveAs((outputFigure + '.' + format).c_str(), format.c_str());

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
unsigned int const GetUIntArgument(size_t const argumentNumber, 
                                        int argc, char *argv[])
{

  unsigned int argument;

  if (argumentNumber > static_cast< unsigned int >(argc - 1)) 
    throw std::invalid_argument
      ("argumentNumber (" + std::to_string(argumentNumber) +
       ") is greater than the number of arguments ("       + 
                            std::to_string(argc - 1) + ")\n");

  std::istringstream input(argv[argumentNumber]);
  if (!(input >> argument)) 
    throw std::invalid_argument("Argument #" + std::to_string(argumentNumber) +
                                " is not a number");

  return argument;

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

  if (!histogram) throw std::invalid_argument("Histogram " + histogramName +
            " does not exist in " + ROOTFile.GetName() + '/' + directoryName);

  return *histogram;

}

//------------------------------------------------------------------------------
void FillWaveformFromHistogram(raw::OpDetWaveform      & waveform, 
                               TH1D               const& histogram)
{

  for (int bin = 0; bin < histogram.GetNbinsX(); ++bin)
    waveform.emplace_back(histogram.GetBinContent(bin));

}

//------------------------------------------------------------------------------
std::unique_ptr< geo::GeometryCore > 
  GetGeometry(fhicl::ParameterSet const& geometryParameterSet)
{

  std::unique_ptr< geo::GeometryCore > geometry = 
                  std::make_unique< geo::GeometryCore >(geometryParameterSet);
  geometry->LoadGeometryFile(geometryParameterSet.get< std::string >("GDML"),
                             geometryParameterSet.get< std::string >("ROOT"));

  // Code from DUNEGeometryHelper_service 
  // (it depeneds on ART, so I don't want to link it here)
  
  std::string const detectorName = geometry->DetectorName();

  fhicl::ParameterSet const sortingParameters;

  std::shared_ptr< geo::ChannelMapAlg > channelMap;
  
  // DUNE 35t prototype
  if  ((detectorName.find("dune35t") != std::string::npos)
    || (detectorName.find("lbne35t") != std::string::npos))
  {
    std::string const detectorVersion
      = sortingParameters.get< std::string >("DetectorVersion");
    
    if  ((detectorVersion.find("v3") != std::string::npos)
      || (detectorVersion.find("v4") != std::string::npos)
      || (detectorVersion.find("v5") != std::string::npos))
      channelMap = 
        std::make_shared< geo::ChannelMap35OptAlg >(sortingParameters);
    else
      channelMap = 
        std::make_shared< geo::ChannelMap35Alg >(sortingParameters);
  }
  // DUNE 10kt
  else if ((detectorName.find("dune10kt") != std::string::npos) 
        || (detectorName.find("lbne10kt") != std::string::npos))
    channelMap = std::make_shared< geo::ChannelMapAPAAlg >(sortingParameters);
  else 
    throw std::invalid_argument("Unsupported geometry from input file: " 
                                                   + detectorName + '\n');

  if (channelMap)
    geometry->ApplyChannelMap(channelMap);

  return geometry;

}

//------------------------------------------------------------------------------
std::unique_ptr< pmtana::PMTPulseRecoBase > 
  GetThresholdAlgorithm(fhicl::ParameterSet const& opHitParameterSet)
{

  std::unique_ptr< pmtana::PMTPulseRecoBase > thresholdAlgorithm;

  fhicl::ParameterSet const hitAlgPSet = 
    opHitParameterSet.get< fhicl::ParameterSet >("HitAlgoPset");
  std::string threshAlgName = hitAlgPSet.get< std::string >("Name");
  if      (threshAlgName == "Threshold") 
    thresholdAlgorithm = std::make_unique< pmtana::AlgoThreshold >(hitAlgPSet);
  else if (threshAlgName == "SiPM") 
    thresholdAlgorithm = std::make_unique< pmtana::AlgoSiPM >(hitAlgPSet);
  else if (threshAlgName == "SlidingWindow")
    thresholdAlgorithm = 
                     std::make_unique< pmtana::AlgoSlidingWindow >(hitAlgPSet);
  else if (threshAlgName == "FixedWindow")
    thresholdAlgorithm = 
                       std::make_unique< pmtana::AlgoFixedWindow >(hitAlgPSet);
  else if (threshAlgName == "CFD" )
    thresholdAlgorithm = std::make_unique< pmtana::AlgoCFD >(hitAlgPSet);
  else throw std::invalid_argument("Cannot find implementation for " 
                                   + threshAlgName + " algorithm.\n");

  return thresholdAlgorithm;

}

//------------------------------------------------------------------------------
std::unique_ptr< pmtana::PMTPedestalBase >
  GetPedestalAlgorithm (fhicl::ParameterSet const& opHitParameterSet)
{

  std::unique_ptr< pmtana::PMTPedestalBase > pedestalAlgorithm;

  auto const pedAlgPSet = 
    opHitParameterSet.get< fhicl::ParameterSet >("PedAlgoPset");
  std::string pedAlgName = pedAlgPSet.get< std::string >("Name");
  if      (pedAlgName == "Edges")
    pedestalAlgorithm = std::make_unique< pmtana::PedAlgoEdges >(pedAlgPSet);
  else if (pedAlgName == "RollingMean")
    pedestalAlgorithm = 
                  std::make_unique< pmtana::PedAlgoRollingMean >(pedAlgPSet);
  else if (pedAlgName == "UB"   )
    pedestalAlgorithm = std::make_unique< pmtana::PedAlgoUB >(pedAlgPSet);
  else throw std::invalid_argument("Cannot find implementation for " 
                                      + pedAlgName + " algorithm.\n");

  return pedestalAlgorithm;

}
