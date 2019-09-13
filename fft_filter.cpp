/*
 * FogLAMP "fft" filter plugin.
 *
 * Copyright (c) 2019 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch           
 */     
#include <reading.h>              
#include <reading_set.h>
#include <utility>                
#include <logger.h>
#include <fft_filter.h>

using namespace std;
using namespace rapidjson;

/**
 * Construct a FFTFilter, call the base class constructor and handle the
 * parsing of the configuration category the required fft
 */
FFTFilter::FFTFilter(const std::string& filterName,
			       ConfigCategory& filterConfig,
                               OUTPUT_HANDLE *outHandle,
                               OUTPUT_STREAM out) :
                                  FogLampFilter(filterName, filterConfig,
                                                outHandle, out)
{
	handleConfig(filterConfig);
}

/**
 * Destructor for the filter.
 */
FFTFilter::~FFTFilter()
{
}

/**
 * Called with a set of readings, iterates over the readings applying
 * the fft filter to create the output readings
 *
 * @param readings	The readings to process
 * @param out		The output readings
 */
void FFTFilter::ingest(vector<Reading *> *readings, vector<Reading *>& out)
{
	lock_guard<mutex> guard(m_configMutex);

	for (vector<Reading *>::const_iterator reading = readings->begin();
						      reading != readings->end();
						      ++reading)
	{
		if ((*reading)->getAssetName().compare(m_asset) == 0)
		{
			addFFTAsset(*reading);
			processFFT(out);
		}
		else
		{
			out.push_back(*reading);
		}
	}
	readings->clear();
}


/**
 * Add a reading to the buffer of data required for an FFT
 *
 * @param asset	The asset to buffer
 */
void FFTFilter::addFFTAsset(Reading *reading)
{
	vector<Datapoint *> datapoints = reading->getReadingData();
	for (int i = 0; i < datapoints.size(); i++)
	{
		string name = datapoints[i]->getName();
		DatapointValue dpv = datapoints[i]->getData();
		double value;
	       	if (dpv.getType() == DatapointValue::T_FLOAT)
			value = dpv.toDouble();
		else if (dpv.getType() == DatapointValue::T_INTEGER)
			value = dpv.toInt();
		else
			continue;
		map<string, vector<double>* >::iterator it = m_buffer.find(name);
		if (it != m_buffer.end())
		{
			it->second->push_back(value);
		}
		else
		{
			vector<double> *vec = new vector<double>;
			vec->push_back(value);
			m_buffer.insert(pair<string, vector<double>* >(name, vec));
		}
	}
}

/**
 * Process FFT buffer. If we have the right number of samples for an FFT
 * run against the particular data point then run an FFT against that
 * data set.
 *
 * @param out		The output array for FFT data
 */
void FFTFilter::processFFT(vector<Reading *>& out)
{
	for (map<string, vector<double>* >::iterator it = m_buffer.begin();
					it != m_buffer.end(); it++)
	{
		if (it->second->size() == m_samples)
		{
			runFFT(out, it->first, it->second);
			it->second->clear();
		}
	}
}

/**
 * Perform the actual FFT conversion on the sample data. Create the frequency bands
 * data and insert an asset in the output array with the frequency band amplitudes
 * and the peak frequency.
 *
 * @param out		Vector of readings to be sent onwards from the filter
 * @param dpName	The name of the datapoint the FFT is beign executed on
 * @param values	The vector of samples on which we will run the FFT
 */
void FFTFilter::runFFT(vector<Reading *>& out, const string& dpName, vector<double> *values)
{
float 	*data = new float[values->size() * 2];

	/*
	 * Turn the sampled data points into complex numbers in an
	 * array.
	 */
	for (int i = 0; i < values->size(); i++)
	{
		data[i * 2] = (*values)[i];
		data[(i * 2) + 1] = 0.0;
	}

	/*
	 * Execute the FFT algorithm across the sample
	 */
	FFT(data, values->size(), 1);


	/*
	 * Process the results of the FFT algorithm, divide the
	 * contents of that array into a number of buckets that
	 * represent a range of frequencies. Average the readings
	 * across that band to create an amplitude for the band.
	 *
	 * Also determine the peak frequency in the data.
	 */
	vector<Datapoint *> datapoints;
	int	n_outputs = values->size() / 2;
	int	first = (m_lowPass * n_outputs) / 100;
	int	last = n_outputs - ((m_highPass * n_outputs) / 100);
	int	aveSamples = (last - first) / m_bands;
	int	band = 0;
	double	sum = 0.0;
	int	cnt = 0;
	double	peak = 0.0;
	double	square = 0.0;
	vector<double> spectrum;

	enum ResultsType {AVERAGE, PEAK, SUM, RMS, SPECTRUM};
	ResultsType results = AVERAGE;
	if (m_results.compare("average") == 0)
		results = AVERAGE;
	else if (m_results.compare("peak") == 0)
		results = PEAK;
	else if (m_results.compare("sum") == 0)
		results = SUM;
	else if (m_results.compare("rms") == 0)
		results = RMS;
	else if (m_results.compare("spectrum") == 0)
		results = SPECTRUM;
	/*
	 * Now take the absolute values of the complex numbers to
	 * get the amplitudes for each frequency
	 */
	for (int i = first; i < last; i++)
	{
		/*
		 * The absolute value of a complex number is the square
		 * root of the sum of the squares of the real and imaginary
		 * parts of the complex number.
		 */
		double absValue = sqrt((data[i * 2] * data[i * 2])
			       	+ (data[(i * 2) + 1] * data[(i * 2) + 1]));
		if (results != SPECTRUM)
		{
			if (absValue > peak)
			{
				peak = absValue;
			}
			sum += absValue;
			square += (absValue * absValue);
			cnt++;
			if (cnt == aveSamples)
			{
				char buf[40];
				snprintf(buf, sizeof(buf), "%s%02d", m_prefix.c_str(), band);
				double value;
				switch (results)
				{
					case AVERAGE:
						value = sum / aveSamples;
						break;
					case PEAK:
						value = peak;
						break;
					case SUM:
						value = sum;
						break;
					case RMS:
						value = sqrt(square / cnt);
						break;
				}
				DatapointValue bdpv(value);
				datapoints.push_back(new Datapoint(buf, bdpv));
				peak = 0.0;
				sum = 0.0;
				square = 0.0;
				cnt = 0;
				band++;
			}
		}
		else
		{
			spectrum.push_back(absValue);
		}
	}
	if (results == SPECTRUM)
	{
		DatapointValue sdpv(spectrum);
		datapoints.push_back(new Datapoint("spectrum", sdpv));
	}
	out.push_back(new Reading(m_asset + " FFT", datapoints));

	delete[] data;
}

/**
 * Handle a reconfiguration request
 *
 * @param newConfig	The new configuration
 */
void FFTFilter::reconfigure(const string& newConfig)
{
	lock_guard<mutex> guard(m_configMutex);
	setConfig(newConfig);
	handleConfig(m_config);
}


/**
 * Handle the configuration of the plugin.
 *
 *
 * @param conf	The configuration category for the filter.
 */
void FFTFilter::handleConfig(const ConfigCategory& config)
{
int n_samples = 0;

	if (config.itemExists("asset"))
		setAsset(config.getValue("asset"));
	if (config.itemExists("prefix"))
		setPrefix(config.getValue("prefix"));
	if (config.itemExists("bands"))
		m_bands = strtol(config.getValue("bands").c_str(), NULL, 10);
	if (config.itemExists("samples"))
		n_samples = strtol(config.getValue("samples").c_str(), NULL, 10);
	if (config.itemExists("result"))
		setResults(config.getValue("result"));
	if (config.itemExists("lowPass"))
		m_lowPass = strtol(config.getValue("lowPass").c_str(), NULL, 10);
	if (config.itemExists("highPass"))
		m_highPass = strtol(config.getValue("highPass").c_str(), NULL, 10);

	if (n_samples != 0 && (n_samples & (n_samples - 1)) == 0)
	{
		m_samples = n_samples;
	}
	else
	{
		Logger::getLogger()->fatal("The value of samples must be a power of 2, FFT filter disabled");
		m_enabled = false;
	}
}
