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

#PATH_TO_ACQUAMAN = ../acquaman
#INCLUDEPATH *= $$PATH_TO_ACQUAMAN/source/

# Set standard level of compiler warnings for everyone. (Otherwise the warnings shown will be system-dependent.)
QMAKE_CXXFLAGS *= -Wextra -g

HEADERS *= \
	AMDSBuffer.h \
	AMDSTcpDataServer.h \
	AMDSThreadedTcpDataServer.h \
	AMDSCentralServer.h \
	AMDSBufferGroup.h \
	AMDSThreadedBufferGroup.h \
	AMDSAxisInfo.h \
	AMDSnDIndex.h \
    AMDSLikely.h \
    AMDSClientDataRequestV1.h \
    AMDSDataStream.h \
    AMDSEventData.h \
    AMDSBufferGroupInfo.h \
    AMDSDataHolder.h \
    AMDSScalarDataHolder.h \
    AMDSClientRequest.h \
    AMDSPacketStats.h

SOURCES *= main.cpp \
	AMDSTcpDataServer.cpp \
	AMDSThreadedTcpDataServer.cpp \
	AMDSCentralServer.cpp \
	AMDSBufferGroup.cpp \
	AMDSThreadedBufferGroup.cpp \
	AMDSAxisInfo.cpp \
	AMDSnDIndex.cpp \
    AMDSClientDataRequestV1.cpp \
    AMDSDataStream.cpp \
    AMDSEventData.cpp \
    AMDSBufferGroupInfo.cpp \
    AMDSDataHolder.cpp \
    AMDSScalarDataHolder.cpp \
    AMDSClientRequest.cpp \
    AMDSPacketStats.cpp
