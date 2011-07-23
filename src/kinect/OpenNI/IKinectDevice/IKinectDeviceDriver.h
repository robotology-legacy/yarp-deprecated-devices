// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>

//Constants from the KinectSkeletonData
#define MAX_USERS 10
#define TOTAL_JOINTS 24
#define KINECT_IMG_WIDTH 640
#define KINECT_IMG_HEIGHT 480

typedef enum
{
	NO_USER,
	USER_LOST,
	USER_DETECTED,
	CALIBRATING,
	SKELETON_TRACKING
} SKELETON_STATE;

namespace yarp {
	namespace dev {
		class IKinectDeviceDriver;
	}
}
/**
* Interface for the KinectDeviceDriverLocal and KinectDeviceDiverServer 
*/
class yarp::dev::IKinectDeviceDriver {
public:
    /**
     * Gets kinect skeleton joints orientation 3x3 matrix
     * @param matrixArray Array of matrices that is filled with the new orientation matrices
	 * @param matrixArray Array of doubles that is filled with the confidence level of each matrice
	 * @param userID id of the user skeleton to be retrieved
     * @return false if the user skeleton is not being tracked
     */
	virtual bool getSkeletonOrientation(yarp::sig::Matrix *matrixArray, double *confidence, int userID) = 0;
	    /**
     * Gets kinect skeleton joints position 3D vector (in millimeters)
     * @param matrixArray Array of vectors that is filled with the new position vector
	 * @param matrixArray Array of doubles that is filled with the confidence level of each vector
	 * @param userID id of the user skeleton to be retrieved
     * @return false if the user skeleton is not being tracked
     */
	virtual bool getSkeletonPosition(yarp::sig::Vector *vectorArray, double *confidence, int userID) = 0;
	/**
	* Gets the user sekelton state
	* 0 - NO_USER 
	* 1 - USER_LOST 
	* 2 - USER_DETECTED 
	* 3 - CALIBRATING 
	* 4 - SKELETON_TRACKING
	* @param userID id of the user skeleton state to be retrieved
	* @return int value of the user skeleton state
	*/
	virtual int getSkeletonState(int userID) = 0;
	/**
	* Get RGB camera image
	* @return camera image
	*/
	virtual yarp::sig::ImageOf<yarp::sig::PixelRgb> getImageMap() = 0;
	/**
	* Get depth camera image (in millimeters)
	* @return camera image
	*/
	virtual yarp::sig::ImageOf<yarp::sig::PixelInt> getDepthMap() = 0;
};