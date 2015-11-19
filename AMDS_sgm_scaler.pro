
HOME_FOLDER = $$system(echo $HOME)

PATH_TO_AMDS_WORKSPACE = beamline/programming
PATH_TO_ACQUAMAN_WORKSPACE = beamline/programming
PATH_TO_MPLOT_WORKSPACE = beamline/programming

USERNAME = $$system(whoami)
contains(USERNAME, liux0) {
	HOME_FOLDER = "/home/liux0"
}

contains(USERNAME, sgm) {
        PATH_TO_AMDS_WORKSPACE = David/AcquamanUpgrade
        #PATH_TO_ACQUAMAN_WORKSPACE = acquaman_new
        PATH_TO_ACQUAMAN_WORKSPACE = David/AcquamanUpgrade
        #PATH_TO_MPLOT_WORKSPACE = beamline/programming
        PATH_TO_MPLOT_WORKSPACE = David/AcquamanUpgrade
}

contains(USERNAME, helfrij) {
	PATH_TO_AMDS_WORKSPACE = beamline/programming/acquaman
	PATH_TO_ACQUAMAN_WORKSPACE = beamline/programming/acquaman
	PATH_TO_MPLOT_WORKSPACE = beamline/programming/acquaman
}

contains(USERNAME, helfrij) {
	PATH_TO_AMDS_WORKSPACE = beamline/programming/acquaman
	PATH_TO_ACQUAMAN_WORKSPACE = beamline/programming/acquaman
	PATH_TO_MPLOT_WORKSPACE = beamline/programming/acquaman
}

PATH_TO_SOURCE = $$HOME_FOLDER/$$PATH_TO_AMDS_WORKSPACE/AcquamanDataServer/source

#PATH_TO_ACQUAMAN = /home/liux0/beamline/programming/acquaman/source
#PATH_TO_MPLOT = /home/liux0/beamline/programming/MPlot/src
PATH_TO_ACQUAMAN = $$HOME_FOLDER/$$PATH_TO_ACQUAMAN_WORKSPACE/acquaman/source
PATH_TO_MPLOT = $$HOME_FOLDER/$$PATH_TO_MPLOT_WORKSPACE/MPlot/src

INCLUDEPATH += $$PATH_TO_SOURCE
INCLUDEPATH += $$PATH_TO_ACQUAMAN
INCLUDEPATH += $$PATH_TO_MPLOT

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




