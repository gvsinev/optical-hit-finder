# optical-hit-finder

A program to test and tune an optical hit finder algorithm for DUNE.

## Requirements

* FHiCL-CPP
* ROOT
* LArData
* LArCore
* DUNETPC

Having LArSoft (v05\_00\_00 or later) installed should satisfy these requirements.

## Configuration
Hit finder parameters can be modified in `pdreco.fcl`.

## Input
The input is a ROOT file with waveforms (in TH1D histogram format) `waveforms.root`.
Command line arguments: `eventNumber`, `opChannel`, `waveformNumber`.

## Output
The output is a PDF file with hit times plotted on top of the input waveform.
