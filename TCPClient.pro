HOME_FOLDER = $$system(echo $HOME)
PROJECT_ROOT = $$PWD/..

PATH_TO_SOURCE = $$PWD/source
PATH_TO_ACQUAMAN = $$PROJECT_ROOT/acquaman/source
PATH_TO_MPLOT = $$PROJECT_ROOT/MPlot/src

INCLUDEPATH *= $$PATH_TO_SOURCE
INCLUDEPATH *= $$PATH_TO_ACQUAMAN
INCLUDEPATH *= $$PATH_TO_MPLOT

include ( compositCommon/AMDSClientCommon.pri )

TARGET = TCPClient

HEADERS  += \
	source/ui/AMDSClientUi.h

SOURCES += \
	source/AMDSClientMain.cpp \
	source/ui/AMDSClientUi.cpp
