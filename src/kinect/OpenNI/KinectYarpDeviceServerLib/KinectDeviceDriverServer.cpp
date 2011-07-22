// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "KinectSkeletonTracker.h"
#include "KinectDeviceDriverServer.h"

yarp::dev::KinectDeviceDriverServer::KinectDeviceDriverServer(bool openPorts, bool userDetection){
	//when this is used as a local Yarp Device the ports are only opened later
	_openPorts = openPorts;
	_userDetection = userDetection;
}

yarp::dev::KinectDeviceDriverServer::KinectDeviceDriverServer(string portPrefix, bool userDetection):GenericYarpDriver(portPrefix+":i", portPrefix+PORTNAME_SKELETON+":o")
{
	openPorts(portPrefix);
	_skeleton = new KinectSkeletonTracker(userDetection);
}

yarp::dev::KinectDeviceDriverServer::~KinectDeviceDriverServer(void)
{
}

bool yarp::dev::KinectDeviceDriverServer::open(yarp::os::Searchable& config){
	//this function is used in case of the Yarp Device being used as local
	std::cout << "Starting Kinect Yarp Device please wait..." << endl;
	string portPrefix;
	if(config.check("portPrefix")) portPrefix = config.find("portPrefix").asString();
	else {
		std::cout << "ERROR: portPrefix not specified!" << endl;
		return false;
	}
	if(config.check("openPorts")) _openPorts = true;
	if(config.check("userDetection")) _userDetection = true;
	if(_openPorts) openPorts(portPrefix);
	_skeleton = new KinectSkeletonTracker(_userDetection);
	std::cout << "Kinect Yarp Device started. Enjoy!" << endl;
	return true;
}

void yarp::dev::KinectDeviceDriverServer::onRead(Bottle &bot){
}

bool yarp::dev::KinectDeviceDriverServer::updateInterface(){
	//std::cout << "updateInterface()" << endl;
	//update kinect data
	_skeleton->updateKinect();
	//send kinect data to ports
	if(_openPorts) 
		sendKinectData(_sendingPort);
	return true;
}

bool yarp::dev::KinectDeviceDriverServer::shellRespond(const Bottle& command, Bottle& reply){
	printf("echo: %s",command.toString().c_str());
	return true;
}

bool yarp::dev::KinectDeviceDriverServer::close(){
	_skeleton->close();
	return true;
}

void yarp::dev::KinectDeviceDriverServer::openPorts(string portPrefix){
	_openPorts = true;
	_depthMapPort = new BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt>>();
	string strTemp = portPrefix+PORTNAME_DEPTHMAP+":o";
	_depthMapPort->open(strTemp.c_str());
	_imgMapPort = new BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>>();
	strTemp = portPrefix+PORTNAME_IMAGEMAP+":o";
	_imgMapPort->open(strTemp.c_str());
}

void yarp::dev::KinectDeviceDriverServer::sendKinectData(BufferedPort<Bottle> *mainBottle){
	KinectSkeletonTracker::UserSkeleton *userSkeleton = KinectSkeletonTracker::getKinect()->userSkeleton;
	double *joint;
	int index = 0;
	//image map data
	_imgMapPort->prepare() = KinectSkeletonTracker::getKinect()->imgMap;
	_imgMapPort->write();

	//image depth map data
	_depthMapPort->prepare() = KinectSkeletonTracker::getKinect()->depthMap;
	_depthMapPort->write();

	//skeleton data
	if(_userDetection)
		for(int i = 0; i < MAX_USERS; i++){
			if(userSkeleton[i].skeletonState == SKELETON_TRACKING){
				Bottle &botSkeleton = mainBottle->prepare();
				botSkeleton.clear();
				//user number
				Bottle &userBot = botSkeleton.addList();
				userBot.addVocab(USER_VOCAB);
				userBot.addInt(i);
				for(int jointIndex = 0; jointIndex < TOTAL_JOINTS; jointIndex++){
					//position
					botSkeleton.addVocab(POSITION_VOCAB);
					joint = userSkeleton[i].skeletonPointsPos[jointIndex].data();
					Bottle &botList = botSkeleton.addList();
					botList.addDouble(joint[0]);
					botList.addDouble(joint[1]);
					botList.addDouble(joint[2]);
					botSkeleton.addDouble(userSkeleton[i].skeletonPosConfidence[jointIndex]);
					//orientation
					joint = userSkeleton[i].skeletonPointsOri[jointIndex].data();
					botSkeleton.addVocab(ORIENTATION_VOCAB);
					Bottle &botList2 = botSkeleton.addList();
					botList2.addDouble(joint[0]);
					botList2.addDouble(joint[1]);
					botList2.addDouble(joint[2]);
					botList2.addDouble(joint[3]);
					botList2.addDouble(joint[4]);
					botList2.addDouble(joint[5]);
					botList2.addDouble(joint[6]);
					botList2.addDouble(joint[7]);
					botList2.addDouble(joint[8]);
					botSkeleton.addDouble(userSkeleton[i].skeletonOriConfidence[jointIndex]);
				}
				mainBottle->write();
			}else if(userSkeleton[i].skeletonState == USER_DETECTED){
				Bottle &botDetected = mainBottle->prepare();
				botDetected.clear();
				botDetected.addString(USER_DETECTED_MSG);
				botDetected.addInt(i);
				mainBottle->write();
			}else if(userSkeleton[i].skeletonState == CALIBRATING){
				Bottle &botCalib = mainBottle->prepare();
				botCalib.clear();
				botCalib.addString(USER_CALIBRATING_MSG);
				botCalib.addInt(i);
				mainBottle->write();
			}else if(userSkeleton[i].skeletonState == USER_LOST){
				userSkeleton[i].skeletonState = NO_USER;
				Bottle &botCalib = mainBottle->prepare();
				botCalib.clear();
				botCalib.addString(USER_LOST_MSG);
				botCalib.addInt(i);
				mainBottle->write();
			}
		}
}


/*************************************************************************************************************
**************************************************************************************************************
**IKinectDeviceDriverClient****IKinectDeviceDriverClient****IKinectDeviceDriverClient****IKinectDeviceDriver**
**************************************************************************************************************
*************************************************************************************************************/

//returns false if the user skeleton is not being tracked
bool yarp::dev::KinectDeviceDriverServer::getSkeletonOrientation(Matrix *matrixArray, double *confidence,  int userID){
	updateInterface();
	if(KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState != SKELETON_TRACKING)
		return false;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		matrixArray[i].resize(3,3);
		matrixArray[i].zero();
		matrixArray[i] = KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonPointsOri[i];
		confidence[i] = (double)KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonPosConfidence[i];
	}
	return true;
}

//returns false if the user skeleton is not being tracked
bool yarp::dev::KinectDeviceDriverServer::getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID){
	updateInterface();
	if(KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState != SKELETON_TRACKING)
		return false;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		vectorArray[i].resize(3);
		vectorArray[i].zero();
		vectorArray[i] = KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonPointsPos[i];
		confidence[i] = (double)KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonPosConfidence[i];
	}
	return true;
}

int yarp::dev::KinectDeviceDriverServer::getSkeletonState(int userID){
	updateInterface();
	return KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState;
}

ImageOf<PixelRgb> yarp::dev::KinectDeviceDriverServer::getImageMap(){
	updateInterface();
	return KinectSkeletonTracker::getKinect()->imgMap;
}

ImageOf<PixelInt> yarp::dev::KinectDeviceDriverServer::getDepthMap(){
	updateInterface();
	return KinectSkeletonTracker::getKinect()->depthMap;
}