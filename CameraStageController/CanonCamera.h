#pragma once

#include "EDSDK/Header/EDSDK.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <map>
#include <string>

class CanonCamera
{
public:
	CanonCamera();
	~CanonCamera();

public:
	//Methods
    bool init();
    void getDeviceInfo();
	void setSaveToComputer();
    void setSaveToCamera();
    void takePicture(); //Must use waitForObjectEvent() to wait for picture to download to PC if setSaveToComputer() (if using console app)
    void waitForObjectEvent(); //Only use if console app
    void waitForPropertyEvent(); //Only use if console app
	void close();
    void setSaveDirectory(const char directory[300]);
	void startLiveView(); //Must use waitForPropertyEvent() if console app
    void getLiveView(cv::Mat& image);
	void displayLiveView();
	void stopLiveView();
	void setJPGImageQuality();
	void setISOspeed(EdsUInt32 isospeed);
    EdsUInt32 getISOspeed();
	void listISOspeed();
	void setShutterSpeed(EdsUInt32 shutterSpeed);
    EdsUInt32 getShutterSpeed();
    void listShutterSpeed();
	void setAperture(EdsUInt32 aperture);

    bool isConnected();

private:
	//Methods
    void getFirstCamera(); //Gets the reference to the first connected camera
	void setEventHandles();
    void downloadImages(EdsDirectoryItemRef directoryItem);
	void downloadEvfData(cv::Mat& image);

	static EdsError EDSCALLBACK handleObjectEvent(EdsObjectEvent event, EdsBaseRef object, EdsVoid* context);
	static EdsError EDSCALLBACK handlePropertyEvent(EdsPropertyEvent event, EdsPropertyID property, EdsUInt32 param, EdsVoid* context);
	static EdsError EDSCALLBACK handleStateEvent(EdsStateEvent event, EdsUInt32 parameter, EdsVoid* context);

public:
	//Variables
	EdsPropertyDesc ISOSpeedList;
    EdsPropertyDesc ShutterSpeedList;
    EdsDeviceInfo DeviceInfo;

private:
	//Variables
	EdsError err = EDS_ERR_OK;
	EdsCameraRef camera = NULL;
	bool objectEventFired = false;
	bool propertyEventFired = false;
    char saveDirectory[300] = {};

    bool connected = false;
};

