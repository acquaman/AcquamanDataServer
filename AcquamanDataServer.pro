#-------------------------------------------------
#
# Project created by QtCreator 2015-01-21T14:07:03
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = AcquamanDataServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
	AMDSBuffer.h \
	AMDSTcpDataServer.h \
	AMDSThreadedTcpDataServer.h \
    AMDSCentralServer.h \
    AMDSBufferGroup.h

SOURCES += main.cpp \
	AMDSTcpDataServer.cpp \
	AMDSThreadedTcpDataServer.cpp \
    AMDSCentralServer.cpp \
    AMDSBufferGroup.cpp
