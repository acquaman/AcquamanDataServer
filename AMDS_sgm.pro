
PATH_TO_SOURCE = /home/liux0/beamline/programming/AcquamanDataServer/source
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
	source/Detector/Amptek/AmptekSDD123Application.h \
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
	source/AMDSMain_SGM.cpp \
	source/application/AMDSCentralServer.cpp \
	source/application/AMDSCentralServerSGM.cpp \
	source/Detector/Amptek/AmptekEventDefinitions.cpp \
	source/Detector/Amptek/AmptekSDD123Application.cpp \
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
