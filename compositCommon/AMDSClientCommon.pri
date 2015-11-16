#-------------------------------------------------
#
# Project created by QtCreator 2014-08-07T15:39:20
#
#-------------------------------------------------

QT	   += core gui
QT	 += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TEMPLATE = app

DESTDIR = build

OBJECTS_DIR=build_files
MOC_DIR=build_files
UI_DIR=build_files
RCC_DIR=build_files


HEADERS  += \
	$$PATH_TO_ACQUAMAN/util/AMErrorMonitor.h \
	$$PATH_TO_ACQUAMAN/ui/util/AMMessageBoxWTimeout.h \
	source/appController/AMDSAppController.h \
	source/appController/AMDSClientAppController.h \
	source/ClientRequest/AMDSClientRequest.h \
	source/ClientRequest/AMDSClientRequestSupport.h \
	source/ClientRequest/AMDSClientRequestDefinitions.h \
	source/ClientRequest/AMDSClientDataRequest.h \
	source/ClientRequest/AMDSClientIntrospectionRequest.h \
	source/ClientRequest/AMDSClientStatisticsRequest.h \
	source/ClientRequest/AMDSClientConfigurationRequest.h \
	source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h \
	source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h \
	source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h \
	source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h \
	source/ClientRequest/AMDSClientContinuousDataRequest.h \
	source/Connection/AMDSPacketStats.h \
	source/Connection/AMDSServer.h \
	source/Connection/AMDSClientTCPSocket.h \
	source/DataElement/AMDSDataTypeDefinitions.h \
	source/DataElement/AMDSConfigurationDef.h \
	source/DataElement/AMDSCommandManager.h \
	source/DataElement/AMDSAxisInfo.h \
	source/DataElement/AMDSnDIndex.h \
	source/DataElement/AMDSEventData.h \
	source/DataElement/AMDSEventDataSupport.h \
	source/DataElement/AMDSFlatArray.h \
	source/DataElement/AMDSBufferGroupInfo.h \
	source/DataElement/AMDSDwellStatusData.h \
	source/DataHolder/AMDSDataHolder.h \
	source/DataHolder/AMDSDataHolderSupport.h \
	source/DataHolder/AMDSGenericFlatArrayDataHolder.h \
	source/DataHolder/AMDSScalarDataHolder.h \
	source/DataHolder/AMDSSpectralDataHolder.h \
	source/util/AMDSMetaObjectSupport.h \
	source/util/AMDSRunTimeSupport.h

SOURCES += \
	$$PATH_TO_ACQUAMAN/util/AMErrorMonitor.cpp \
	$$PATH_TO_ACQUAMAN/ui/util/AMMessageBoxWTimeout.cpp \
	source/appController/AMDSAppController.cpp \
	source/appController/AMDSClientAppController.cpp \
	source/ClientRequest/AMDSClientRequest.cpp \
	source/ClientRequest/AMDSClientRequestSupport.cpp \
	source/ClientRequest/AMDSClientDataRequest.cpp \
	source/ClientRequest/AMDSClientIntrospectionRequest.cpp \
	source/ClientRequest/AMDSClientStatisticsRequest.cpp \
	source/ClientRequest/AMDSClientConfigurationRequest.cpp \
	source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.cpp \
	source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.cpp \
	source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.cpp \
	source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.cpp \
	source/ClientRequest/AMDSClientContinuousDataRequest.cpp \
	source/Connection/AMDSPacketStats.cpp \
	source/Connection/AMDSServer.cpp \
	source/Connection/AMDSClientTCPSocket.cpp \
	source/DataElement/AMDSCommandManager.cpp \
	source/DataElement/AMDSAxisInfo.cpp \
	source/DataElement/AMDSnDIndex.cpp \
	source/DataElement/AMDSEventData.cpp \
	source/DataElement/AMDSEventDataSupport.cpp \
	source/DataElement/AMDSFlatArray.cpp \
	source/DataElement/AMDSBufferGroupInfo.cpp \
	source/DataElement/AMDSDwellStatusData.cpp \
	source/DataHolder/AMDSDataHolder.cpp \
	source/DataHolder/AMDSDataHolderSupport.cpp \
	source/DataHolder/AMDSGenericFlatArrayDataHolder.cpp \
	source/DataHolder/AMDSScalarDataHolder.cpp \
	source/DataHolder/AMDSSpectralDataHolder.cpp \
	source/util/AMDSMetaObjectSupport.cpp \
	source/util/AMDSRunTimeSupport.cpp
