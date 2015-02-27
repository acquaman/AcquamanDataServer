#ifndef AMDSDATASTREAM_H
#define AMDSDATASTREAM_H

#include <QDataStream>

#include "AMDSAxisInfo.h"
#include "AMDSBufferGroup.h"
//#include "AMDSClientDataRequestV1.h"
#include "AMDSPacketStats.h"
#include "ClientRequest/AMDSClientRequest.h"

class AMDSDataStream : public QDataStream
{
public:
	AMDSDataStream();
	explicit AMDSDataStream(QIODevice *device);
	explicit AMDSDataStream(QByteArray *array, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMDSDataStream(const QByteArray &array);
	virtual ~AMDSDataStream();

	void write(const AMDSAxisInfo &axisInfo);
	void write(const AMDSBufferGroupInfo &bufferGroupInfo);
	void write(const AMDSPacketStats &packetStat);
//	void write(const AMDSClientDataRequestV1 &clientDataRequest);

	void encodeClientRequestType(const AMDSClientRequest &clientRequest);
	void write(const AMDSClientRequest &clientRequest);

	/// If the axis info was not readable, then the passed in reference is left unchanged.
	void read(AMDSAxisInfo &axisInfo);
	void read(AMDSBufferGroupInfo &bufferGroupInfo);
	void read(AMDSPacketStats &packetStat);
//	void read(AMDSClientDataRequestV1 &clientDataRequest);

	AMDSClientRequestDefinitions::RequestType decodeRequestType();
	AMDSClientRequest* decodeAndInstantiateClientRequestType();
	void read(AMDSClientRequest &clientRequest);
};

#endif // AMDSDATASTREAM_H
