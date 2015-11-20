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

TARGET = AcquamanDataServerSGMPV

HEADERS *= \
	source/application/AMDSCentralServer.h \
        source/application/SGM/AMDSCentralServerSGMPV.h \
        source/Detector/AMDSDetectorServer.h \
        source/Detector/PVController/AMDSPVConfigurationMap.h \
        source/Detector/PVController/AMDSPVCommandManager.h \
        source/Detector/PVController/AMDSPVControllerServer.h \
        source/Detector/PVController/AMDSPVController.h

SOURCES *= \
        source/AMDSMainSGMPV.cpp \
	source/application/AMDSCentralServer.cpp \
        source/application/SGM/AMDSCentralServerSGMPV.cpp \
        source/Detector/AMDSDetectorServer.cpp \
        source/Detector/PVController/AMDSPVCommandManager.cpp \
        source/Detector/PVController/AMDSPVControllerServer.cpp \
        source/Detector/PVController/AMDSPVController.cpp
