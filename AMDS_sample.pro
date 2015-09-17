
PATH_TO_SOURCE = /home/liux0/beamline/programming/AcquamanDataServer/source
PATH_TO_ACQUAMAN = /home/liux0/beamline/programming/acquaman/source
PATH_TO_MPLOT = /home/liux0/beamline/programming/MPlot/src

INCLUDEPATH += $$PATH_TO_SOURCE
INCLUDEPATH += $$PATH_TO_ACQUAMAN
INCLUDEPATH += $$PATH_TO_MPLOT

include ( compositCommon/AcquamanMPlot.pri )
include ( compositCommon/AMDSCommon.pri )

TARGET = AcquamanDataServerSample

HEADERS *= \
	source/application/AMDSCentralServer.h \
	source/application/AMDSCentralServerSample.h
	
SOURCES *= \
	source/AMDSMain_Sample.cpp \
	source/application/AMDSCentralServer.cpp \
	source/application/AMDSCentralServerSample.cpp
