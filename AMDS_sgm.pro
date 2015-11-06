
HOME_FOLDER = $$system(echo $HOME)

PATH_TO_AMDS_WORKSPACE = beamline/programming
PATH_TO_ACQUAMAN_WORKSPACE = beamline/programming

USERNAME = $$system(whoami)
contains(USERNAME, liux0) {
	HOME_FOLDER = "/home/liux0"
}

contains(USERNAME, sgm) {
	PATH_TO_AMDS_WORKSPACE = .
	PATH_TO_ACQUAMAN_WORKSPACE = beamline/programming
}

PATH_TO_SOURCE = $$HOME_FOLDER/$$PATH_TO_AMDS_WORKSPACE/AcquamanDataServer/source
#PATH_TO_ACQUAMAN = $$HOME_FOLDER/$$PATH_TO_ACQUAMAN_WORKSPACE/acquaman/source
#PATH_TO_MPLOT = $$HOME_FOLDER/$$PATH_TO_ACQUAMAN_WORKSPACE/MPlot/src
PATH_TO_ACQUAMAN = /home/liux0/beamline/programming/acquaman/source
PATH_TO_MPLOT = /home/liux0/beamline/programming/MPlot/src

INCLUDEPATH += $$PATH_TO_SOURCE
INCLUDEPATH += $$PATH_TO_ACQUAMAN
INCLUDEPATH += $$PATH_TO_MPLOT

include ( compositCommon/AcquamanMPlot.pri )
include ( compositCommon/AMDSCommon.pri )

TARGET = AcquamanDataServerSGM

HEADERS *= \
	source/application/AMDSCentralServer.h \
	source/application/AMDSCentralServerSGM.h \
	source/Detector/Amptek/AmptekEventDefinitions.h \
	source/Detector/Amptek/AmptekSDD123ConfigurationMap.h \
	source/Detector/Amptek/AmptekCommandManager.h \
	source/Detector/Amptek/AmptekSDD123Packet.h \
	source/Detector/Amptek/AmptekSDD123Server.h \
	source/Detector/Amptek/AmptekSDD123ServerGroup.h \
	source/Detector/Amptek/AmptekSDD123Detector.h \
	source/Detector/Amptek/AmptekSDD123DetectorManager.h \
	source/Detector/Amptek/SGM/AmptekCommandManagerSGM.h \
	source/Detector/Amptek/SGM/AmptekSDD123DetectorGroupSGM.h \
	source/Detector/Amptek/SGM/AmptekSDD123EPICSDetectorManager.h

SOURCES *= \
	source/AMDSMainSGM.cpp \
	source/application/AMDSCentralServer.cpp \
	source/application/AMDSCentralServerSGM.cpp \
	source/Detector/Amptek/AmptekEventDefinitions.cpp \
	source/Detector/Amptek/AmptekSDD123ConfigurationMap.cpp \
	source/Detector/Amptek/AmptekCommandManager.cpp \
	source/Detector/Amptek/AmptekSDD123Packet.cpp \
	source/Detector/Amptek/AmptekSDD123Server.cpp \
	source/Detector/Amptek/AmptekSDD123ServerGroup.cpp \
	source/Detector/Amptek/AmptekSDD123Detector.cpp \
	source/Detector/Amptek/AmptekSDD123DetectorManager.cpp \
	source/Detector/Amptek/SGM/AmptekCommandManagerSGM.cpp \
	source/Detector/Amptek/SGM/AmptekSDD123DetectorGroupSGM.cpp \
	source/Detector/Amptek/SGM/AmptekSDD123EPICSDetectorManager.cpp


