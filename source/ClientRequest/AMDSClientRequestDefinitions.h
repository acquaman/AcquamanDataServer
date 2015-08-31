#ifndef AMDSCLIENTREQUESTDEFINITIONS_H
#define AMDSCLIENTREQUESTDEFINITIONS_H

#include <QString>

namespace AMDSClientRequestDefinitions {
	#define AMDS_CLIENTREQUEST_SUCCESS 0
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_SOCKET_KEY 20100  // 2xxxx error codes for client request
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_STATUS 20101  // 2xxxx error codes for client request
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_ERROR_MESSAGE 20102
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RESPONSE_TYPE 20103
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME 20104

	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS_COUNT 20110 // AMDSClientStatisticsRequest
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS 20111

	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_GROUP_INFO_COUNT 20120 // AMDSClientIntrospectionRequest
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_GROUP_INFO 20121

	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_STATUS 20130 // AMDSClientDataRequest
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_DATA 20131
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UNIFORM_DATA_TYPE 20132
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_COUNT 20133
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DECODE_DATA_TYPE 20134
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DECODE_DATA_TYPE_IN_DATA_HOLDER 20135
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_TYPE 20136
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INSTANTIATE_DATA_HOLDER 20137
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_DATA 20138
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_FLATTEN_RESULT_DATA 20139

	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_START_TIME 20140
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_END_TIME 20141
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_MIDDLE_TIME 20142
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT 20143
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RELATIVE_COUNT 20144
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT_BEFORE 20145
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT_AFTER 20146
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UPDATE_INTERVAL 20147
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_HANDSHAKE_SOCKET_KEY 20148
	#define AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAMES 20149

	enum OperationType{
		Read = 0,
		Write = 1
	};
	enum RequestType{
		Introspection = 0,
		Statistics = 1,
		StartTimePlusCount = 2,
		RelativeCountPlusCount = 3,
		StartTimeToEndTime = 4,
		MiddleTimePlusCountBeforeAndAfter = 5,
		Continuous = 6,
		InvalidRequest = 7 // NOTE: this must be the last item of the RequestType definition
	};

	static QString errorMessage(int errorCode, OperationType operType, RequestType msgType) {
		QString fieldName;

		switch (errorCode) {
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_SOCKET_KEY:
			fieldName = "SocketKey";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_STATUS:
			fieldName = "Status";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_ERROR_MESSAGE:
			fieldName = "ErrorMessage";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RESPONSE_TYPE:
			fieldName = "ResponseType";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME:
			fieldName = "BufferName";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS_COUNT:
			fieldName = "PacketStatsCount";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS:
			fieldName = "PacketStats";
			break;

		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_GROUP_INFO_COUNT:
			fieldName = "BufferGroupInfoCount";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_GROUP_INFO:
			fieldName = "BufferGroupInfo";
			break;

		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_STATUS:
			fieldName = "IncludeStatus";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_DATA:
			fieldName = "IncludeData";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UNIFORM_DATA_TYPE:
			fieldName = "UniformDataType";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_COUNT:
			fieldName = "DataCount";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DECODE_DATA_TYPE:
			fieldName = "DecodeDataType";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DECODE_DATA_TYPE_IN_DATA_HOLDER:
			fieldName = "DecodeDataTypeInDataHolder";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_TYPE:
			fieldName = "DataHolderType";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INSTANTIATE_DATA_HOLDER:
			fieldName = "InstantiateDataHolder";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_DATA:
			fieldName = "DataHolderData";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_FLATTEN_RESULT_DATA:
			fieldName = "FlattenResultData";
			break;

		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_START_TIME:
			fieldName = "StartTime";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_END_TIME:
			fieldName = "EndTime";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_MIDDLE_TIME:
			fieldName = "MiddleTime";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT:
			fieldName = "Count";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RELATIVE_COUNT:
			fieldName = "RelativeCount";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT_BEFORE:
			fieldName = "CountBefore";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT_AFTER:
			fieldName = "CountAfter";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UPDATE_INTERVAL:
			fieldName = "UpdateInterval";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_HANDSHAKE_SOCKET_KEY:
			fieldName = "HandShakeSocketKey";
			break;
		case AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAMES:
			fieldName = "BufferNames";
			break;
		default:
			fieldName = "Unknown";
			break;
		}

		QString operation = (operType == Read ? "read" : "write");
		QString errMsg = QString("Failed to %1 `%2` for message type %3 ").arg(operation).arg(fieldName).arg(msgType);
		return errMsg;
	}
}

#endif // AMDSCLIENTREQUESTDEFINITIONS_H
