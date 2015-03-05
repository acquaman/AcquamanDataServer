#include "AMDSFlatArray.h"

AMDSFlatArray::AMDSFlatArray(AMDSDataTypeDefinitions::DataType dataType, quint32 size)
{
	dataType_ = dataType;
	resizeType(dataType_, size);
}

AMDSFlatArray::~AMDSFlatArray()
{
}
