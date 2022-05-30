#include "CanonCamera.h"

CanonCamera::CanonCamera() {}
CanonCamera::~CanonCamera() {}

bool CanonCamera::init()
{
    if (!connected)
    {
        err = EdsInitializeSDK();

        if (err == EDS_ERR_OK)
            getFirstCamera();

        if (err == EDS_ERR_OK)
            setEventHandles();

        if (err == EDS_ERR_OK)
            err = EdsOpenSession(camera);

        if (err != EDS_ERR_OK)
        {
            err = EdsCloseSession(camera);
            err = EdsRelease(camera);
            err = EdsTerminateSDK();
            connected = false;
        }
        else
            connected = true;
    }
    return connected;
}

void CanonCamera::getDeviceInfo()
{
    err = EdsGetDeviceInfo(camera, &DeviceInfo);
    //std::cout << info->szDeviceDescription << std::endl;
}

void CanonCamera::setSaveToComputer()
{
    err = EDS_ERR_OK;

	EdsUInt32 saveTarget = kEdsSaveTo_Host;
	EdsCapacity capacity = {0x7FFFFFFF, 0x1000, true};

	if (err == EDS_ERR_OK)
		err = EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(saveTarget), &saveTarget);
	if (err == EDS_ERR_OK)
		err = EdsSetCapacity(camera, capacity);
}

void CanonCamera::setSaveToCamera()
{
    EdsUInt32 saveTarget = kEdsSaveTo_Camera;

    err = EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(saveTarget) , &saveTarget);
}

void CanonCamera::takePicture()
{

    err = EdsSendCommand(camera, kEdsCameraCommand_TakePicture, 0);
}

void CanonCamera::waitForObjectEvent()
{
	while (!objectEventFired)
	{
		EdsGetEvent();
	}
	//Reset event flag
	objectEventFired = false;
}

void CanonCamera::waitForPropertyEvent()
{
	while (!propertyEventFired)
	{
		EdsGetEvent();
	}
	//Reset event flag
	propertyEventFired = false;
}

void CanonCamera::close()
{
	err = EdsCloseSession(camera);
	err = EdsRelease(camera);
	err = EdsTerminateSDK();
    connected = false;
}

void CanonCamera::setSaveDirectory(const char directory[300])
{
	strcpy_s(saveDirectory, directory);
}

void CanonCamera::startLiveView()
{
	EdsUInt32 device;

	device = kEdsEvfOutputDevice_TFT | kEdsEvfOutputDevice_PC;
    err = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	// A property change event notification is issued from the camera if property settings are made successfully.
	// Start downloading of the live view image once the property change notification arrives.
}

void CanonCamera::displayLiveView()
{
	cv::Mat image;
	cv::namedWindow("main", cv::WINDOW_NORMAL);

	int key;

	while (true)
	{
		downloadEvfData(image);
		if (!image.empty())
			imshow("main", image);

		key = cv::waitKey(10);
		if (key == 32)
			takePicture();

		else if (key >= 0)
			break;
	}
}

void CanonCamera::getLiveView(cv::Mat& image)
{
    downloadEvfData(image);
}

void CanonCamera::stopLiveView()
{
	EdsUInt32 device;

	device = kEdsEvfOutputDevice_TFT;
    err = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
}

void CanonCamera::setJPGImageQuality()
{
	EdsUInt32 quality = 0;
	quality = 0x13FF0F;
    err = EdsSetPropertyData(camera, kEdsPropID_ImageQuality, 0, sizeof(quality), &quality);
}

void CanonCamera::setISOspeed(EdsUInt32 isoSpeed)
{
    err = EdsSetPropertyData(camera, kEdsPropID_ISOSpeed, 0, sizeof(isoSpeed), &isoSpeed);
}

void CanonCamera::listISOspeed()
{
    err = EdsGetPropertyDesc(camera, kEdsPropID_ISOSpeed, &ISOSpeedList);
}

EdsUInt32 CanonCamera::getISOspeed()
{
    EdsUInt32 isoSpeed = 0xffffffff;
    err = EdsGetPropertyData(camera, kEdsPropID_ISOSpeed, 0, sizeof(isoSpeed), &isoSpeed);
    return isoSpeed;
}

void CanonCamera::setShutterSpeed(EdsUInt32 shutterSpeed)
{
    err = EdsSetPropertyData(camera, kEdsPropID_Tv, 0, sizeof(shutterSpeed), &shutterSpeed);
}

EdsUInt32 CanonCamera::getShutterSpeed()
{
    EdsUInt32 shutterSpeed = 0xffffffff;
    err = EdsGetPropertyData(camera, kEdsPropID_Tv, 0, sizeof(shutterSpeed), &shutterSpeed);
    return shutterSpeed;
}

void CanonCamera::listShutterSpeed()
{
     err = EdsGetPropertyDesc(camera, kEdsPropID_Tv, &ShutterSpeedList);
}

void CanonCamera::setAperture(EdsUInt32 aperture)
{
    err = EdsSetPropertyData(camera, kEdsPropID_Av, 0, sizeof(aperture), &aperture);
}


void CanonCamera::getFirstCamera()
{
	EdsCameraListRef camera_list = NULL;
	EdsUInt32 count = 0;

	if (err == EDS_ERR_OK)
		err = EdsGetCameraList(&camera_list);

	if (err == EDS_ERR_OK)
	{
		err = EdsGetChildCount(camera_list, &count);
		if (count == 0) 
			err = EDS_ERR_DEVICE_NOT_FOUND;
	}

	if (err == EDS_ERR_OK)
		err = EdsGetChildAtIndex(camera_list, 0, &camera);

	//Release camera_list
	EdsRelease(camera_list);
	camera_list = NULL;
}

void CanonCamera::setEventHandles()
{
	if (err == EDS_ERR_OK)
		err = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handleObjectEvent, this);
	// Set Property event handler
	if (err == EDS_ERR_OK)
		err = EdsSetPropertyEventHandler(camera, kEdsPropertyEvent_All, handlePropertyEvent, this);
	// Set State event handler
	if (err == EDS_ERR_OK)
		err = EdsSetCameraStateEventHandler(camera, kEdsStateEvent_All, handleStateEvent, this);
}

void CanonCamera::downloadImages(EdsDirectoryItemRef directoryItem)
{
    err = EDS_ERR_OK;

	EdsStreamRef stream = NULL;
    char directory[300];

    struct tm newtime;
    time_t now = time(NULL);
    char time_string[32];

    char *token[32];

    localtime_s(&newtime, &now);
    strftime(time_string, 32, "_%H%M%S.", &newtime);

	// Get directory item information
	EdsDirectoryItemInfo dirItemInfo;
	err = EdsGetDirectoryItemInfo(directoryItem, &dirItemInfo);

	// Create file stream for transfer destination
	if (err == EDS_ERR_OK)
	{
		strcpy_s(directory, saveDirectory);
        //strcat_s(directory, dirItemInfo.szFileName);
        strcat_s(directory, strtok_s(dirItemInfo.szFileName, ".", token));
        strcat_s(directory, time_string);
        strcat_s(directory, strtok_s(NULL, ".", token));

		//err = EdsCreateFileStream(dirItemInfo.szFileName, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
		err = EdsCreateFileStream(directory, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
	}

	// Download image
	if (err == EDS_ERR_OK)
	{
		err = EdsDownload(directoryItem, dirItemInfo.size, stream);
	}

	// Issue notification that download is complete
	if (err == EDS_ERR_OK)
	{
		err = EdsDownloadComplete(directoryItem);
	}

	// Release stream
	if (stream != NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}
}

void CanonCamera::downloadEvfData(cv::Mat& image)
{
    err = EDS_ERR_OK;

	EdsStreamRef stream = NULL;
	EdsEvfImageRef evfImage = NULL;
	unsigned char* data = NULL;
	EdsUInt64 size = 0;
	EdsSize coords;

	// Create memory stream.
	err = EdsCreateMemoryStream(0, &stream);
	// Create EvfImageRef.
	if (err == EDS_ERR_OK)
		err = EdsCreateEvfImageRef(stream, &evfImage);

	// Download live view image data.
	if (err == EDS_ERR_OK)
		err = EdsDownloadEvfImage(camera, evfImage);

	// Get the incidental data of the image.
	
	// get image coordinates
	EdsGetPropertyData(evfImage, kEdsPropID_Evf_CoordinateSystem, 0, sizeof(coords), &coords);

	// get buffer pointer and size
	EdsGetPointer(stream, (EdsVoid**)&data);
	EdsGetLength(stream, &size);

	// create mat object
	std::vector<unsigned char> buffer(data, data + size);
	if (!buffer.empty())
		image = cv::imdecode(buffer, cv::IMREAD_COLOR);

	// Release stream
	if (stream != NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}
	// Release evfImage
	if (evfImage != NULL)
	{
		EdsRelease(evfImage);
		evfImage = NULL;
	}
}

EdsError EDSCALLBACK CanonCamera::handleObjectEvent(EdsObjectEvent event, EdsBaseRef object, EdsVoid* context)
{
	EdsError err = EDS_ERR_OK;
	CanonCamera* canonCamera = (CanonCamera*)context;

	switch (event)
	{
		case kEdsObjectEvent_DirItemRequestTransfer:
			canonCamera->downloadImages(object);
			canonCamera->objectEventFired = true;
			break;
	}

	return err;
}

EdsError EDSCALLBACK CanonCamera::handlePropertyEvent(EdsPropertyEvent event, EdsPropertyID property, EdsUInt32 param, EdsVoid* context)
{
	EdsError err = EDS_ERR_OK;
	CanonCamera* canonCamera = (CanonCamera*)context;

	switch (event)
	{
		case kEdsPropertyEvent_PropertyChanged:
			canonCamera->propertyEventFired = true;
			break;
	}

	return err;
}

EdsError EDSCALLBACK CanonCamera::handleStateEvent(EdsStateEvent event, EdsUInt32 parameter, EdsVoid* context)
{
	EdsError err = EDS_ERR_OK;
	CanonCamera* canonCamera = (CanonCamera*)context;

	return err;
}

bool CanonCamera::isConnected()
{
    return connected;
}
