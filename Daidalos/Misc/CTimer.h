#ifndef _CHIGHRESOLUTIONTIMER
#define _CHIGHRESOLUTIONTIMER

/**
* CHighResolutionTimer class.
* Manages time.
*/
class CHighResolutionTimer 
{
protected:
	double m_start;
	double m_stop;
	double m_corr;
	double m_freq;
	double m_last;

public:
	
	CHighResolutionTimer() 
	{
		LARGE_INTEGER timer;
		this->m_corr = 0.0;
		QueryPerformanceFrequency(&timer); 
		this->m_freq = double(timer.QuadPart); 
		this->m_start = Get(); 
		this->m_stop  = Get(); 
		this->m_corr  = this->m_stop - this->m_start;
		this->m_last = Get();
	};

	double Get(double * pt = NULL) 
	{
		LARGE_INTEGER timer;
		QueryPerformanceCounter(&timer);
		double	t = (double)timer.QuadPart / this->m_freq;
		if (pt != NULL) * pt = t;
		return t;
	};

	double Elapsed() 
	{
		double	tcur = Get();
		return tcur - this->m_last - this->m_corr;
	};

	double Delta() 
	{
		double	tsave = this->m_last;
		this->m_last = Get();
		return this->m_last - tsave - this->m_corr;
	};
};

#endif
