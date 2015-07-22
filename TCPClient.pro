#-------------------------------------------------
#
# Project created by QtCreator 2014-08-07T15:39:20
#
#-------------------------------------------------

QT	   += core gui
QT	 += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = TCPClient
TEMPLATE = app

DESTDIR = build

OBJECTS_DIR=build_files
MOC_DIR=build_files
UI_DIR=build_files
RCC_DIR=build_files

HEADERS  += \
	source/AMDSClient.h \
	source/AMDSAxisInfo.h \
	source/AMDSnDIndex.h \
	source/AMDSBufferGroupInfo.h \
	source/AMDSDataStream.h \
	source/AMDSEventData.h \
	source/AMDSEventDataSupport.h \
	source/AMDSPacketStats.h \
	source/AMDSDataTypeDefinitions.h \
	source/AMDSFlatArray.h \
	source/ClientRequest/AMDSClientRequest.h \
	source/ClientRequest/AMDSClientRequestSupport.h \
	source/ClientRequest/AMDSClientRequestDefinitions.h \
	source/ClientRequest/AMDSClientDataRequest.h \
	source/ClientRequest/AMDSClientIntrospectionRequest.h \
	source/ClientRequest/AMDSClientStatisticsRequest.h \
	source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h \
	source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h \
	source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h \
	source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h \
	source/ClientRequest/AMDSClientContinuousDataRequest.h \
	source/DataHolder/AMDSDataHolder.h \
	source/DataHolder/AMDSDataHolderSupport.h \
	source/DataHolder/AMDSGenericFlatArrayDataHolder.h \
	source/DataHolder/AMDSScalarDataHolder.h \
	source/DataHolder/AMDSSpectralDataHolder.h \
	source/AMDSMetaObjectSupport.h

SOURCES += \
	source/AMDSClientMain.cpp \
	source/AMDSClient.cpp \
	source/AMDSAxisInfo.cpp \
	source/AMDSnDIndex.cpp \
	source/AMDSBufferGroupInfo.cpp \
	source/AMDSDataStream.cpp \
	source/AMDSEventData.cpp \
	source/AMDSEventDataSupport.cpp \
	source/AMDSPacketStats.cpp \
	source/AMDSFlatArray.cpp \
	source/ClientRequest/AMDSClientRequest.cpp \
	source/ClientRequest/AMDSClientRequestSupport.cpp \
	source/ClientRequest/AMDSClientDataRequest.cpp \
	source/ClientRequest/AMDSClientIntrospectionRequest.cpp \
	source/ClientRequest/AMDSClientStatisticsRequest.cpp \
	source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.cpp \
	source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.cpp \
	source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.cpp \
	source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.cpp \
	source/ClientRequest/AMDSClientContinuousDataRequest.cpp \
	source/DataHolder/AMDSDataHolder.cpp \
	source/DataHolder/AMDSDataHolderSupport.cpp \
	source/DataHolder/AMDSGenericFlatArrayDataHolder.cpp \
	source/DataHolder/AMDSScalarDataHolder.cpp \
	source/DataHolder/AMDSSpectralDataHolder.cpp \
	source/AMDSMetaObjectSupport.cpp

