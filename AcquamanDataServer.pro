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

DESTDIR = build

OBJECTS_DIR=build_files
MOC_DIR=build_files
UI_DIR=build_files
RCC_DIR=build_files

#TEMPLATE = app

#PATH_TO_ACQUAMAN = ../acquaman
#INCLUDEPATH *= $$PATH_TO_ACQUAMAN/source/

# Set standard level of compiler warnings for everyone. (Otherwise the warnings shown will be system-dependent.)
QMAKE_CXXFLAGS *= -Wextra -g

HEADERS *= \
	source/AMDSBuffer.h \
	source/AMDSTcpDataServer.h \
	source/AMDSThreadedTcpDataServer.h \
	source/AMDSCentralServer.h \
	source/AMDSBufferGroup.h \
	source/AMDSThreadedBufferGroup.h \
	source/AMDSAxisInfo.h \
	source/AMDSnDIndex.h \
	source/AMDSLikely.h \
	source/AMDSDataStream.h \
	source/AMDSEventData.h \
	source/AMDSEventDataSupport.h \
	source/AMDSBufferGroupInfo.h \
	source/AMDSPacketStats.h \
	source/AMDSDataTypeDefinitions.h \
	source/AMDSFlatArray.h \
	source/AMDSMetaObjectSupport.h \
	source/ClientRequest/AMDSClientRequest.h \
	source/ClientRequest/AMDSClientRequestSupport.h \
	source/ClientRequest/AMDSClientRequestDefinitions.h \
	source/ClientRequest/AMDSClientDataRequest.h \
	source/ClientRequest/AMDSClientIntrospectionRequest.h \
	source/ClientRequest/AMDSClientStatisticsRequest.h \
	source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h \
	source/ClientRequest/AMDSClientContinuousDataRequest.h \
	source/DataHolder/AMDSDataHolder.h \
	source/DataHolder/AMDSDataHolderSupport.h \
	source/DataHolder/AMDSGenericFlatArrayDataHolder.h \
	source/DataHolder/AMDSScalarDataHolder.h \
	source/DataHolder/AMDSSpectralDataHolder.h \
	source/DataHolder/AMDSImageDataHolder.h

SOURCES *= \
	source/main.cpp \
	source/AMDSTcpDataServer.cpp \
	source/AMDSThreadedTcpDataServer.cpp \
	source/AMDSCentralServer.cpp \
	source/AMDSBufferGroup.cpp \
	source/AMDSThreadedBufferGroup.cpp \
	source/AMDSAxisInfo.cpp \
	source/AMDSnDIndex.cpp \
	source/AMDSDataStream.cpp \
	source/AMDSEventData.cpp \
	source/AMDSEventDataSupport.cpp \
	source/AMDSBufferGroupInfo.cpp \
	source/AMDSPacketStats.cpp \
	source/AMDSFlatArray.cpp \
	source/AMDSMetaObjectSupport.cpp \
	source/ClientRequest/AMDSClientRequest.cpp \
	source/ClientRequest/AMDSClientRequestSupport.cpp \
	source/ClientRequest/AMDSClientDataRequest.cpp \
	source/ClientRequest/AMDSClientIntrospectionRequest.cpp \
	source/ClientRequest/AMDSClientStatisticsRequest.cpp \
	source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.cpp \
	source/ClientRequest/AMDSClientContinuousDataRequest.cpp \
	source/DataHolder/AMDSDataHolder.cpp \
	source/DataHolder/AMDSDataHolderSupport.cpp \
	source/DataHolder/AMDSGenericFlatArrayDataHolder.cpp \
	source/DataHolder/AMDSScalarDataHolder.cpp \
	source/DataHolder/AMDSSpectralDataHolder.cpp \
	source/DataHolder/AMDSImageDataHolder.cpp
