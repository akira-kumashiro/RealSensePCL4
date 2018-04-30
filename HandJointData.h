#pragma once

#include "pxchanddata.h"
#include <math.h>

class HandJointData
{
public:
	HandJointData();
	~HandJointData();

	void getJointLength(bool isUnitInMilliMerter = true);
	bool isHandSurfaceVartical(float verticalThreshold);
	

	/*float x;
	float y;
	float z;

	int jointName;*/

	float x[PXCHandData::NUMBER_OF_JOINTS];
	float y[PXCHandData::NUMBER_OF_JOINTS];
	float z[PXCHandData::NUMBER_OF_JOINTS];

	enum DistanceName
	{
		DST_BETWEEN_THUMB_BASE_AND_CENTER,
		DST_BETWEEN_THUMB_BASE_AND_JT1,
		DST_BETWEEM_THUMB_JT1_AND_JT2,
		DST_BETWEEN_THUMB_JT2_AND_TIP,
		DST_BETWEEN_INDEX_BASE_AND_CENTER,
		DST_BETWEEN_INDEX_BASE_AND_JT1,
		DST_BETWEEM_INDEX_JT1_AND_JT2,
		DST_BETWEEN_INDEX_JT2_AND_TIP,
		DST_BETWEEN_MIDDLE_BASE_AND_CENTER,
		DST_BETWEEN_MIDDLE_BASE_AND_JT1,
		DST_BETWEEM_MIDDLE_JT1_AND_JT2,
		DST_BETWEEN_MIDDLE_JT2_AND_TIP,
		DST_BETWEEN_RING_BASE_AND_CENTER,
		DST_BETWEEN_RING_BASE_AND_JT1,
		DST_BETWEEM_RING_JT1_AND_JT2,
		DST_BETWEEN_RING_JT2_AND_TIP,
		DST_BETWEEN_PINKY_BASE_AND_CENTER,
		DST_BETWEEN_PINKY_BASE_AND_JT1,
		DST_BETWEEM_PINKY_JT1_AND_JT2,
		DST_BETWEEN_PINKY_JT2_AND_TIP,
		NUM_OF_JOINT_DST,
	};

	float jointDistance[NUM_OF_JOINT_DST];

	//ÉÅÉÇ:HandData.hÇÃì‡óeî≤êà
	/*enum JointType
	{
		/// The center of the wrist
		JOINT_WRIST = 0
		, JOINT_CENTER            /// The center of the palm
		, JOINT_THUMB_BASE        /// Thumb finger joint 1 (base)
		, JOINT_THUMB_JT1         /// Thumb finger joint 2
		, JOINT_THUMB_JT2         /// Thumb finger joint 3
		, JOINT_THUMB_TIP         /// Thumb finger joint 4 (fingertip)
		, JOINT_INDEX_BASE        /// Index finger joint 1 (base)
		, JOINT_INDEX_JT1         /// Index finger joint 2
		, JOINT_INDEX_JT2         /// Index finger joint 3
		, JOINT_INDEX_TIP         /// Index finger joint 4 (fingertip)
		, JOINT_MIDDLE_BASE       /// Middle finger joint 1 (base)
		, JOINT_MIDDLE_JT1        /// Middle finger joint 2
		, JOINT_MIDDLE_JT2        /// Middle finger joint 3
		, JOINT_MIDDLE_TIP        /// Middle finger joint 4 (fingertip)
		, JOINT_RING_BASE         /// Ring finger joint 1 (base)
		, JOINT_RING_JT1          /// Ring finger joint 2
		, JOINT_RING_JT2          /// Ring finger joint 3
		, JOINT_RING_TIP          /// Ring finger joint 4 (fingertip)
		, JOINT_PINKY_BASE        /// Pinky finger joint 1 (base)
		, JOINT_PINKY_JT1         /// Pinky finger joint 2
		, JOINT_PINKY_JT2         /// Pinky finger joint 3
		, JOINT_PINKY_TIP         /// Pinky finger joint 4 (fingertip)        
	};*/
private:
	float getJoint2JointLength(int startJointName,int endJointName);
	float powf2(float _X);
};

