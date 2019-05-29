=====================================
FogLAMP "FFT" C++ Filter plugin
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
  is not supported natively on all the systems FogLAMP can send data to.

samples
  The number of input samples to use. This must be a power of 2.

lowPass
  A percentage of low frequencies to discard, effectively reducing the range of frequencies to examine

highPass
  A percentage of low frequencies to discard, effectively reducing the range of frequencies to examine

Build
-----
To build FogLAMP "FFT" C++ filter plugin:

.. code-block:: console

  $ mkdir build
  $ cd build
  $ cmake ..
  $ make

- By default the FogLAMP develop package header files and libraries
  are expected to be located in /usr/include/foglamp and /usr/lib/foglamp
- If **FOGLAMP_ROOT** env var is set and no -D options are set,
  the header files and libraries paths are pulled from the ones under the
  FOGLAMP_ROOT directory.
  Please note that you must first run 'make' in the FOGLAMP_ROOT directory.

You may also pass one or more of the following options to cmake to override 
this default behaviour:

- **FOGLAMP_SRC** sets the path of a FogLAMP source tree
- **FOGLAMP_INCLUDE** sets the path to FogLAMP header files
- **FOGLAMP_LIB sets** the path to FogLAMP libraries
- **FOGLAMP_INSTALL** sets the installation path of Random plugin

NOTE:
 - The **FOGLAMP_INCLUDE** option should point to a location where all the FogLAMP 
   header files have been installed in a single directory.
 - The **FOGLAMP_LIB** option should point to a location where all the FogLAMP
   libraries have been installed in a single directory.
 - 'make install' target is defined only when **FOGLAMP_INSTALL** is set

Examples:

- no options

  $ cmake ..

- no options and FOGLAMP_ROOT set

  $ export FOGLAMP_ROOT=/some_foglamp_setup

  $ cmake ..

- set FOGLAMP_SRC

  $ cmake -DFOGLAMP_SRC=/home/source/develop/FogLAMP  ..

- set FOGLAMP_INCLUDE

  $ cmake -DFOGLAMP_INCLUDE=/dev-package/include ..
- set FOGLAMP_LIB

  $ cmake -DFOGLAMP_LIB=/home/dev/package/lib ..
- set FOGLAMP_INSTALL

  $ cmake -DFOGLAMP_INSTALL=/home/source/develop/FogLAMP ..

  $ cmake -DFOGLAMP_INSTALL=/usr/local/foglamp ..
