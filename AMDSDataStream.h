#ifndef AMDSDATASTREAM_H
#define AMDSDATASTREAM_H

#include <QDataStream>

#include "AMDSAxisInfo.h"
#include "AMDSBufferGroup.h"
#include "AMDSClientDataRequest.h"

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
	void write(const AMDSClientDataRequest &clientDataRequest);

	/// If the axis info was not readable, then the passed in reference is left unchanged.
	void read(AMDSAxisInfo &axisInfo);
	void read(AMDSBufferGroupInfo &bufferGroupInfo);
	void read(AMDSClientDataRequest &clientDataRequest);
};

#endif // AMDSDATASTREAM_H
