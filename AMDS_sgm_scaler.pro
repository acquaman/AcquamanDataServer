
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
	source/application/SGM/AMDSCentralServerSGMScaler.h \
	source/Detector/AMDSDetectorServer.h \
	source/Detector/Scaler/AMDSScalerCommandManager.h \
	source/Detector/Scaler/AMDSScalerDetector.h \
	source/Detector/Scaler/AMDSScalerDetectorServer.h \
	source/Detector/Scaler/AMDSScalerConfigurationMap.h \
    source/Detector/Scaler/AMDSScalerDetectorManager.h \
    source/Detector/Scaler/AMDSDwellScalerDetector.h

SOURCES *= \
	source/AMDSMainSGMScaler.cpp \
	source/application/AMDSCentralServer.cpp \
	source/application/SGM/AMDSCentralServerSGMScaler.cpp \
	source/Detector/AMDSDetectorServer.cpp \
	source/Detector/Scaler/AMDSScalerCommandManager.cpp \
	source/Detector/Scaler/AMDSScalerDetector.cpp \
	source/Detector/Scaler/AMDSScalerDetectorServer.cpp \
	source/Detector/Scaler/AMDSScalerConfigurationMap.cpp \
    source/Detector/Scaler/AMDSScalerDetectorManager.cpp \
    source/Detector/Scaler/AMDSDwellScalerDetector.cpp

OTHER_FILES *= \
	db/AMDSScaler.db




