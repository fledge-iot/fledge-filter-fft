#ifndef _FFT_FILTER_H
#define _FFT_FILTER_H

/*
 * FogLAMP "fft" filter plugin.
 *
 * Copyright (c) 2019 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch           
 */     
#include <filter.h>               
#include <reading_set.h>
#include <config_category.h>
#include <string>                 
#include <logger.h>
#include <list>
#include <vector>
#include <map>
#include <mutex>

extern "C" void FFT (float data[], unsigned long number_of_complex_samples, int isign);

/**
 */
class FFTFilter : public FogLampFilter {
	public:
		FFTFilter(const std::string& filterName,
                        ConfigCategory& filterConfig,
                        OUTPUT_HANDLE *outHandle,
                        OUTPUT_STREAM out);
		~FFTFilter();
		void	ingest(std::vector<Reading *> *readings, std::vector<Reading *>& out);
		void	reconfigure(const std::string& newConfig);
	private:
		void 			handleConfig(const ConfigCategory& conf);
		void			addFFTAsset(Reading *);
		void			processFFT(std::vector<Reading *>& out);
		void			runFFT(std::vector<Reading *>& out, const std::string& dpName, std::vector<double> *values);
		void			setAsset(const std::string& asset) { m_asset = asset; };
		void			setPrefix(const std::string& prefix) { m_prefix = prefix; };
		void			setResults(const std::string& results) { m_results = results; };
		std::string		m_asset;
		std::string		m_prefix;
		int			m_samples;
		int			m_bands;
		int			m_lowPass;
		int			m_highPass;
		std::string		m_results;
		std::map<std::string, std::vector<double>* >
					m_buffer;
		std::mutex		m_configMutex;
};



#endif
