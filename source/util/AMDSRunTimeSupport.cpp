#include "AMDSRunTimeSupport.h"

#include "util/AMErrorMonitor.h"

namespace AMDSRunTimeSupport{

	DebugLevel debugLevel_ = None;

	DebugLevel debugLevel(){
		return debugLevel_;
	}

	void setDebugLevel(DebugLevel debugLevel){
		debugLevel_ = debugLevel;
	}

	bool debugAtLevel(DebugLevel requestedLevel){
		if(requestedLevel <= debugLevel_)
			return true;
		return false;
	}

	void debugMessage(DebugMsgType type, const QObject *src, int code, const QString &desc) {
		switch (type) {
		case DebugMsg:
			if (debugAtLevel(Debug))
				AMErrorMon::debug(src, code, desc);
			break;
		case InformationMsg:
			if (debugAtLevel(Information))
				AMErrorMon::debug(src, code, desc);
			break;
		case AlertMsg:
			if (debugAtLevel(Alert))
				AMErrorMon::alert(src, code, desc);
			break;
		case ErrorMsg:
			AMErrorMon::error(src, code, desc);
			break;
		default:
			break;
		}
	}

}
