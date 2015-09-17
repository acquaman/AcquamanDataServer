#-------------------------------------------------
#
# Project created by QtCreator 2014-08-07T15:39:20
#
#-------------------------------------------------

PATH_TO_ACQUAMAN = /home/liux0/beamline/programming/acquaman/source
INCLUDEPATH += $$PATH_TO_ACQUAMAN

include ( compositCommon/AMDSClientCommon.pri )

TARGET = TCPClient

HEADERS  += \
	source/ui/AMDSClientUi.h

SOURCES += \
	source/AMDSClientMain.cpp \
	source/ui/AMDSClientUi.cpp
