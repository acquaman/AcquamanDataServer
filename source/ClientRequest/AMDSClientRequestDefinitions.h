#ifndef AMDSCLIENTREQUESTDEFINITIONS_H
#define AMDSCLIENTREQUESTDEFINITIONS_H

namespace AMDSClientRequestDefinitions {
	enum RequestType{
		Introspection = 0,
		Statistics = 1,
		StartTimePlusCount = 2,
		RelativeCountPlusCount = 3,
		StartTimeToEndTime = 4,
		MiddleTimePlusCountBeforeAndAfter = 5,
		Continuous = 6,
		ContinuousWithBatchStreams = 7,
		InvalidRequest = 8
	};
}

#endif // AMDSCLIENTREQUESTDEFINITIONS_H
