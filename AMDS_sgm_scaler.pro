HOME_FOLDER = $$system(echo $HOME)
PROJECT_ROOT = $$PWD/..

PATH_TO_SOURCE = $$PWD/source
PATH_TO_ACQUAMAN = $$PROJECT_ROOT/acquaman/source
PATH_TO_MPLOT = $$PROJECT_ROOT/MPlot/src

INCLUDEPATH *= $$PATH_TO_SOURCE
INCLUDEPATH *= $$PATH_TO_ACQUAMAN
INCLUDEPATH *= $$PATH_TO_MPLOT

include ( compositCommon/AcquamanMPlot.pri )
include ( compositCommon/AMDSCommon.pri )

TARGET = AcquamanDataServerSGMScaler

HEADERS *= \
	source/application/AMDSCentralServer.h \
	source/application/AMDSCentralServerSGMScaler.h \
	source/Detector/AMDSDetectorServer.h \
	source/Detector/Scaler/AMDSScalerCommandManager.h \
	source/Detector/Scaler/AMDSScalerDetector.h \
	source/Detector/Scaler/AMDSScalerDetectorServer.h \
	source/Detector/Scaler/AMDSScalerConfigurationMap.h

SOURCES *= \
	source/AMDSMainSGMScaler.cpp \
	source/application/AMDSCentralServer.cpp \
	source/application/AMDSCentralServerSGMScaler.cpp \
	source/Detector/AMDSDetectorServer.cpp \
	source/Detector/Scaler/AMDSScalerCommandManager.cpp \
	source/Detector/Scaler/AMDSScalerDetector.cpp \
	source/Detector/Scaler/AMDSScalerDetectorServer.cpp \
	source/Detector/Scaler/AMDSScalerConfigurationMap.cpp



OTHER_FILES *= \
	AMDSScaler.db
