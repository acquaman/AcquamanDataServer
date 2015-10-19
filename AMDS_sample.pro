
HOME_FOLDER = $$system(echo $HOME)

USERNAME = $$system(whoami)
contains(USERNAME, liux0) {
	HOME_FOLDER = "/home/liux0"
}

PATH_TO_WORKSPACE = beamline/programming

PATH_TO_SOURCE = $$HOME_FOLDER/$$PATH_TO_WORKSPACE/AcquamanDataServer/source
PATH_TO_ACQUAMAN = $$HOME_FOLDER/$$PATH_TO_WORKSPACE/acquaman/source
PATH_TO_MPLOT = $$HOME_FOLDER/$$PATH_TO_WORKSPACE/MPlot/src

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
	source/AMDSMainSample.cpp \
	source/application/AMDSCentralServer.cpp \
	source/application/AMDSCentralServerSample.cpp
