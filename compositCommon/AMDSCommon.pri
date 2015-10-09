#-------------------------------------------------
#
# Project created by QtCreator 2015-01-21T14:07:03
#
#-------------------------------------------------

QT       += core network gui sql

CONFIG   += console
CONFIG   -= app_bundle

DESTDIR = build

OBJECTS_DIR=build_files
MOC_DIR=build_files
UI_DIR=build_files
RCC_DIR=build_files

# Set standard level of compiler warnings for everyone. (Otherwise the warnings shown will be system-dependent.)
QMAKE_CXXFLAGS *= -Wextra -g

HEADERS *= \
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
	source/Connection/AMDSTCPDataServer.h \
	source/Connection/AMDSThreadedTCPDataServer.h \
	source/Connection/AMDSPacketStats.h \
	source/Connection/AMDSDataStream.h \
	source/DataElement/AMDSLikely.h \
	source/DataElement/AMDSDataTypeDefinitions.h \
	source/DataElement/AMDSBuffer.h \
	source/DataElement/AMDSAxisInfo.h \
	source/DataElement/AMDSnDIndex.h \
	source/DataElement/AMDSEventData.h \
	source/DataElement/AMDSEventDataSupport.h \
	source/DataElement/AMDSFlatArray.h \
	source/DataElement/AMDSBufferGroup.h \
	source/DataElement/AMDSBufferGroupInfo.h \
	source/DataElement/AMDSThreadedBufferGroup.h \
	source/DataHolder/AMDSDataHolder.h \
	source/DataHolder/AMDSDataHolderSupport.h \
	source/DataHolder/AMDSGenericFlatArrayDataHolder.h \
	source/DataHolder/AMDSScalarDataHolder.h \
	source/DataHolder/AMDSSpectralDataHolder.h \
	source/DataHolder/AMDSDwellSpectralDataHolder.h \
	source/DataHolder/AMDSImageDataHolder.h  \
	source/util/AMDSMetaObjectSupport.h \
    source/DataElement/AMDSDwellStatusData.h

SOURCES *= \
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
	source/Connection/AMDSTCPDataServer.cpp \
	source/Connection/AMDSThreadedTCPDataServer.cpp \
	source/Connection/AMDSPacketStats.cpp \
	source/Connection/AMDSDataStream.cpp \
	source/DataElement/AMDSAxisInfo.cpp \
	source/DataElement/AMDSnDIndex.cpp \
	source/DataElement/AMDSEventData.cpp \
	source/DataElement/AMDSEventDataSupport.cpp \
	source/DataElement/AMDSFlatArray.cpp \
	source/DataElement/AMDSBufferGroupInfo.cpp \
	source/DataElement/AMDSBufferGroup.cpp \
	source/DataElement/AMDSThreadedBufferGroup.cpp \
	source/DataHolder/AMDSDataHolder.cpp \
	source/DataHolder/AMDSDataHolderSupport.cpp \
	source/DataHolder/AMDSGenericFlatArrayDataHolder.cpp \
	source/DataHolder/AMDSScalarDataHolder.cpp \
	source/DataHolder/AMDSSpectralDataHolder.cpp \
	source/DataHolder/AMDSDwellSpectralDataHolder.cpp \
	source/DataHolder/AMDSImageDataHolder.cpp \
	source/util/AMDSMetaObjectSupport.cpp \
    source/DataElement/AMDSDwellStatusData.cpp




