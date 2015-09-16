
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
        source/Detector/Amptek/Buffer.h \
        source/Detector/Amptek/SGM/AmptekSDD123Detector.h \
        source/Detector/Amptek/SGM/AmptekSDD123Histogram.h

SOURCES *= \
        source/AMDSMain_SGM.cpp \
		source/application/AMDSCentralServer.cpp \
        source/application/AMDSCentralServerSGM.cpp \
        source/Detector/Amptek/SGM/AmptekSDD123Detector.cpp \
        source/Detector/Amptek/SGM/AmptekSDD123Histogram.cpp
