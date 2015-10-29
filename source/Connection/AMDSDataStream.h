#ifndef AMDSDATASTREAM_H
#define AMDSDATASTREAM_H

#include <QDataStream>

#include "source/Connection/AMDSPacketStats.h"
#include "source/DataElement/AMDSFlatArray.h"
#include "source/DataElement/AMDSBufferGroup.h"
#include "source/ClientRequest/AMDSClientRequest.h"

#define AMDS_SERVER_ERR_DATA_STREAM 20500
#define AMDS_SERVER_INFO_DS_FAILED_TO_PARSE_REQUEST 20501

class AMDSDataStream : public QDataStream
{
public:
	AMDSDataStream();
	explicit AMDSDataStream(QIODevice *device);
	explicit AMDSDataStream(QByteArray *array, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMDSDataStream(const QByteArray &array);
	virtual ~AMDSDataStream();


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
