#ifndef AMDSERROR_H
#define AMDSERROR_H

#include <QDebug>

namespace AMDSError {
	enum ErrorCategory {
		DEBUG = 0,
		INFORMATION = 1,
		WARNING = 2,
		ERROR = 3,
		FATAL =4
	};

	inline void printError(quint8 errLevel, quint16 errCode, QString errMsg) {
		qDebug() << QString("AMDS error tracking: %1 - (%2) %3").arg(errLevel).arg(errCode).arg(errMsg);
	}
}

#endif // AMDSERROR_H
