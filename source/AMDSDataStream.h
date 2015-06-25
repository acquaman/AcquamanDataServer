#ifndef AMDSDATASTREAM_H
#define AMDSDATASTREAM_H

#include <QDataStream>

#include "source/AMDSAxisInfo.h"
#include "source/AMDSBufferGroup.h"
#include "source/AMDSPacketStats.h"
#include "source/AMDSFlatArray.h"
#include "source/ClientRequest/AMDSClientRequest.h"

class AMDSDataStream : public QDataStream
{
public:
	AMDSDataStream();
	explicit AMDSDataStream(QIODevice *device);
	explicit AMDSDataStream(QByteArray *array, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMDSDataStream(const QByteArray &array);
	virtual ~AMDSDataStream();

	void write(const AMDSAxisInfo &axisInfo);
	/// If the axis info was not readable, then the passed in reference is left unchanged.
	void read(AMDSAxisInfo &axisInfo);

	void write(const AMDSBufferGroupInfo &bufferGroupInfo);
	void read(AMDSBufferGroupInfo &bufferGroupInfo);

	void write(const AMDSPacketStats &packetStat);
	void read(AMDSPacketStats &packetStat);

	void encodeEventDataType(const AMDSEventData &eventData);
	void write(const AMDSEventData &eventData);
	QString decodeEventDataType();
	AMDSEventData* decodeAndInstantiateEventData();
	void read(AMDSEventData &eventData);

	void encodeDataHolderType(const AMDSDataHolder &dataHolder);
	void write(const AMDSDataHolder &dataHolder, bool encodeDataType = true);
	QString decodeDataHolderType();
	AMDSDataHolder* decodeAndInstantiateDataHolder();
	void read(AMDSDataHolder &dataHolder, AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::InvalidType);

	void encodeDataType(AMDSDataTypeDefinitions::DataType dataType);
	void write(const AMDSFlatArray &flatArray);
	AMDSDataTypeDefinitions::DataType decodeDataType();
	void read(AMDSFlatArray &flatArray);

	void encodeClientRequestType(const AMDSClientRequest &clientRequest);
	void write(const AMDSClientRequest &clientRequest);
	AMDSClientRequestDefinitions::RequestType decodeRequestType();
	AMDSClientRequest* decodeAndInstantiateClientRequestType();
	void read(AMDSClientRequest &clientRequest);
};

#endif // AMDSDATASTREAM_H
