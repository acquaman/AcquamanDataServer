#-------------------------------------------------
#
# Project created by QtCreator 2014-08-07T15:39:20
#
#-------------------------------------------------

HOME_FOLDER = $$system(echo $HOME)

PATH_TO_AMDS_WORKSPACE = beamline/programming
PATH_TO_ACQUAMAN_WORKSPACE = beamline/programming

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

PATH_TO_SOURCE = $$HOME_FOLDER/$$PATH_TO_AMDS_WORKSPACE/AcquamanDataServer/source
PATH_TO_ACQUAMAN = $$HOME_FOLDER/$$PATH_TO_ACQUAMAN_WORKSPACE/acquaman/source

INCLUDEPATH += $$PATH_TO_SOURCE
INCLUDEPATH += $$PATH_TO_ACQUAMAN

include ( compositCommon/AMDSClientCommon.pri )

TARGET = TCPClient

HEADERS  += \
	source/ui/AMDSClientUi.h

SOURCES += \
	source/AMDSClientMain.cpp \
	source/ui/AMDSClientUi.cpp
