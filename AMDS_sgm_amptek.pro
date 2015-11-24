
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

TARGET = AcquamanDataServerSGMAmptek

HEADERS *= \
	source/application/AMDSCentralServer.h \
	source/application/SGM/AMDSCentralServerSGMAmptek.h \
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
	source/application/SGM/AMDSCentralServerSGMAmptek.cpp \
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

OTHER_FILES *= \
	db/AMDSPVConfiguration.txt
