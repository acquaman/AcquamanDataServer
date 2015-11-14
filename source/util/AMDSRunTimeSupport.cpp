#include "AMDSRunTimeSupport.h"

namespace AMDSRunTimeSupport{
	quint8 debugLevel_ = 0;

	quint8 debugLevel(){
		return debugLevel_;
	}

	void setDebugLevel(quint8 debugLevel){
		debugLevel_ = debugLevel;
	}

	bool debugAtLevel(quint8 requestedLevel){
		if(requestedLevel <= debugLevel_)
			return true;
		return false;
	}
}
