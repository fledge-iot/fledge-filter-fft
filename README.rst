=====================================
Fledge "FFT" C++ Filter plugin
=====================================

A filter that applies a FFT algorithm to a set of readings.

Data is return in a single reading with a set of datapoints that each
represent the average amplitude for a band of frequencies. These bands are
created by dividing the frequency space into a number of equal ranges after
first applying a low and high frequency filter to discard a percentage of
the low and high frequency results.

E.g. if the lowPass filter is set to 15% and the highPass filter is set
to 10%, with the number of bands set to 5, the lower 15% of results are
discarded and the upper 10% are discarded. The remaining 75% of readings
are then divided into 5 equal bands, each of which representing 15% of the
original result space. The results within each of the 15% bands are then
averaged to produce a result for the frequecy band.

Configuration
-------------

The filter supports the following set of configuration options

asset
  The asset to apply the FFT filter to

bands
  The number of frequency bands to define the resultant FFT output into

result
  The data that should be returned for each band. This may be one of
  average, sum, peak or rms. Selecting average will return the average
  amplitude within the band, sum returns the sum of all amplitudes within
  the frequency band, peak the greatest amplitude and rms the root mean
  square of the amplitudes within the band. Setting the output type to
  be spectrum will result in the full FFT spectrum data begn written.
  Spectrum data however can not be sent to all north destinations as it
  is not supported natively on all the systems Fledge can send data to.

samples
  The number of input samples to use. This must be a power of 2.

lowPass
  A percentage of low frequencies to discard, effectively reducing the range of frequencies to examine

highPass
  A percentage of low frequencies to discard, effectively reducing the range of frequencies to examine

Build
-----
To build Fledge "FFT" C++ filter plugin:

.. code-block:: console

  $ mkdir build
  $ cd build
  $ cmake ..
  $ make

- By default the Fledge develop package header files and libraries
  are expected to be located in /usr/include/fledge and /usr/lib/fledge
- If **FLEDGE_ROOT** env var is set and no -D options are set,
  the header files and libraries paths are pulled from the ones under the
  FLEDGE_ROOT directory.
  Please note that you must first run 'make' in the FLEDGE_ROOT directory.

You may also pass one or more of the following options to cmake to override 
this default behaviour:

- **FLEDGE_SRC** sets the path of a Fledge source tree
- **FLEDGE_INCLUDE** sets the path to Fledge header files
- **FLEDGE_LIB sets** the path to Fledge libraries
- **FLEDGE_INSTALL** sets the installation path of Random plugin

NOTE:
 - The **FLEDGE_INCLUDE** option should point to a location where all the Fledge 
   header files have been installed in a single directory.
 - The **FLEDGE_LIB** option should point to a location where all the Fledge
   libraries have been installed in a single directory.
 - 'make install' target is defined only when **FLEDGE_INSTALL** is set

Examples:

- no options

  $ cmake ..

- no options and FLEDGE_ROOT set

  $ export FLEDGE_ROOT=/some_fledge_setup

  $ cmake ..

- set FLEDGE_SRC

  $ cmake -DFLEDGE_SRC=/home/source/develop/Fledge  ..

- set FLEDGE_INCLUDE

  $ cmake -DFLEDGE_INCLUDE=/dev-package/include ..
- set FLEDGE_LIB

  $ cmake -DFLEDGE_LIB=/home/dev/package/lib ..
- set FLEDGE_INSTALL

  $ cmake -DFLEDGE_INSTALL=/home/source/develop/Fledge ..

  $ cmake -DFLEDGE_INSTALL=/usr/local/fledge ..
