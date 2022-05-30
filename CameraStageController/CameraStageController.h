// This class handles the camera and the motor controller

#ifndef CAMERASTAGECONTROLLER_H
#define CAMERASTAGECONTROLLER_H

#include "qcoreapplication.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\core\core.hpp"
#include "CanonCamera.h"
#include "MotorController.h"
#include <QImage>
#include <QDate>
#include <QDir>
#include <QSerialPortInfo>
#include <QDebug>

class CameraStageController
{
public:
    CameraStageController();

public:
    QImage getCameraImage(); //Gets the liveview image from the camera and converts it to a QImage
    bool connectCamera();
    void disconnectCamera();
    void captureImage();
    void setSaveDirectory(QString directory);

    void sendGCode(QString gcode);
    bool stageMessageRec(); //Returns true if a message has been recieved from the motor controller
    QString getStageMessage(); //Returns the message recieved
    bool connectToCOM(QString com);
    void disconnectCOM();

    void getListofISO(); //Gets a list of available ISO settings for the connected camera
    void setISOspeed(int32_t ISOspeed);

    void getListofShutter(); //Gets a list of available shutter settings for the connected camera
    void setShutterSpeed(int32_t shutterSpeed);

    bool isCameraConnected();
    bool isSerialConnected();

private:

public:
    //Struct of camera settings
    struct {
      char saveDirectory[300];
      char device[300];

      uint32_t ISOspeed;
      int32_t listISOspeed[128];
      int32_t arraySizeISO;

      uint32_t shutterSpeed;
      int32_t listShutterSpeed[128];
      int32_t arraySizeShutter;
    } settings;

private:
    bool cameraConnected = false;

    QImage imdisplay;
    CanonCamera camera;
    cv::Mat matImage;
    MotorController controller;


};

#endif // CAMERASTAGECONTROLLER_H
