#include "HandJointData.h"



HandJointData::HandJointData()
{
}


HandJointData::~HandJointData()
{
}

void HandJointData::getJointLength(bool isUnitInMilliMerter)
{
	for (int i = 0; i < NUM_OF_JOINT_DST; i++)
	{
		if (i % 4 == 0)
		{
			jointDistance[i] = getJoint2JointLength(i + 2, 1);
		}
		else
		{
			jointDistance[i] = getJoint2JointLength(i + 2, i + 1);
		}
		if (isUnitInMilliMerter)
		{
			jointDistance[i] *= 1000;
		}

	}
}

bool HandJointData::isHandSurfaceVartical(float verticalThreshold)
{
	float zsum = 0.0f;
	float zStdDev = 0.0f;
	float zStdDevTemp = 0.0;
	for (int i = 0; i < PXCHandData::NUMBER_OF_JOINTS; i++)
	{
		zsum += z[i];
	}
	for (int i = 0; i < PXCHandData::NUMBER_OF_JOINTS; i++)
	{
		zStdDevTemp += powf2(z[i] - zsum / PXCHandData::NUMBER_OF_JOINTS) / PXCHandData::NUMBER_OF_JOINTS;
	}
	zStdDev = sqrtf(zStdDevTemp);

	if (zStdDev < verticalThreshold)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float HandJointData::getJoint2JointLength(int startJointName, int endJointName)
{
	return sqrtf(powf2((x[startJointName] - x[endJointName])) + powf2((y[startJointName] - y[endJointName])) + powf2((z[startJointName] - z[endJointName])));
}

float HandJointData::powf2(float _X)
{
	return powf(_X, 2.0);
}
