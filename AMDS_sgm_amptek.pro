
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

TARGET = AcquamanDataServerSGMAmptek

HEADERS *= \
	source/application/AMDSCentralServer.h \
	source/application/AMDSCentralServerSGMAmptek.h \
	source/Detector/Amptek/AmptekEventDefinitions.h \
	source/Detector/Amptek/AmptekSDD123ConfigurationMap.h \
	source/Detector/Amptek/AmptekSDD123Packet.h \
	source/Detector/Amptek/AmptekSDD123Server.h \
	source/Detector/Amptek/AmptekSDD123ServerGroup.h \
	source/Detector/Amptek/AmptekSDD123Detector.h \
	source/Detector/Amptek/AmptekSDD123DetectorManager.h \
	source/Detector/Amptek/SGM/AmptekCommandManagerSGM.h \
	source/Detector/Amptek/SGM/AmptekSDD123DetectorGroupSGM.h \
	source/Detector/Amptek/SGM/AmptekSDD123EPICSDetectorManager.h

SOURCES *= \
	source/AMDSMainSGMAmptek.cpp \
	source/application/AMDSCentralServer.cpp \
	source/application/AMDSCentralServerSGMAmptek.cpp \
	source/Detector/Amptek/AmptekEventDefinitions.cpp \
	source/Detector/Amptek/AmptekSDD123ConfigurationMap.cpp \
	source/Detector/Amptek/AmptekSDD123Packet.cpp \
	source/Detector/Amptek/AmptekSDD123Server.cpp \
	source/Detector/Amptek/AmptekSDD123ServerGroup.cpp \
	source/Detector/Amptek/AmptekSDD123Detector.cpp \
	source/Detector/Amptek/AmptekSDD123DetectorManager.cpp \
	source/Detector/Amptek/SGM/AmptekCommandManagerSGM.cpp \
	source/Detector/Amptek/SGM/AmptekSDD123DetectorGroupSGM.cpp \
	source/Detector/Amptek/SGM/AmptekSDD123EPICSDetectorManager.cpp
