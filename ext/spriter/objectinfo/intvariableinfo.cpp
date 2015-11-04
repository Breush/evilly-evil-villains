#include "objectinfo/intvariableinfo.h"

namespace SpriterEngine
{

	IntVariableInfo::IntVariableInfo() :
		value(0)
	{
	}

	int IntVariableInfo::getIntValue()
	{
		return value;
	}

	void IntVariableInfo::setIntValue(int newValue)
	{
		value = newValue;
	}

	void IntVariableInfo::setObjectTolinear(UniversalObjectInterface *bObject, real t, UniversalObjectInterface *resultObject)
	{
		resultObject->setIntValue(int(linear(value, bObject->getIntValue(), t)));
	}

}
