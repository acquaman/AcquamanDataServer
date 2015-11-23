#include "AMDSRunTimeSupport.h"

#include "ClientRequest/AMDSClientRequest.h"
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

	void debugMessage(DebugMsgType type, const QObject *src, int code, const AMDSClientRequest *clientRequest) {
		DebugLevel debugLevel = AMDSRunTimeSupport::None;
		switch (type) {
		case DebugMsg:
			debugLevel = AMDSRunTimeSupport::Debug;
			break;
		case InformationMsg:
			debugLevel = AMDSRunTimeSupport::Information;
			break;
		case AlertMsg:
			debugLevel = AMDSRunTimeSupport::Alert;
			break;
		case ErrorMsg:
			debugLevel = AMDSRunTimeSupport::Error;
			break;
		default:
			break;
		}

		if (debugAtLevel(debugLevel)) {
			debugMessage(type, src, code, clientRequest->toString());
		}
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
