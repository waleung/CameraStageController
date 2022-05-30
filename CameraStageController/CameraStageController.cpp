#include "CameraStageController.h"

CameraStageController::CameraStageController()
{
    //controller.connectPort(L"\\\\.\\COM4");
}

QImage CameraStageController::getCameraImage()
{
    camera.getLiveView(matImage);
    imdisplay = QImage((uchar*)matImage.data, matImage.cols, matImage.rows, matImage.step, QImage::Format_BGR888); //Converts the CV image into Qt standard format
    return imdisplay;
}

bool CameraStageController::connectCamera()
{
    if (!camera.isConnected())
    {
        if (camera.init())
        {
            QString temp_dir = QCoreApplication::applicationDirPath() + "/Pictures/" + QDate::currentDate().toString("yyyy_MM_dd");
            //QString temp_dir = QCoreApplication::applicationDirPath() + "/Pictures";

            QDir dir(temp_dir);
            if (!dir.exists())
                dir.mkpath(temp_dir);

            strcpy_s(settings.saveDirectory, temp_dir.toStdString().c_str());
            //strcat_s(settings.saveDirectory, "/");
            temp_dir = temp_dir + "/";

            camera.setSaveToComputer();
            camera.setSaveDirectory(temp_dir.toStdString().c_str());

            settings.ISOspeed = camera.getISOspeed();
            getListofISO();

            settings.shutterSpeed = camera.getShutterSpeed();
            getListofShutter();

            camera.getDeviceInfo();
            strcpy_s(settings.device, camera.DeviceInfo.szDeviceDescription);

            camera.startLiveView();

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void CameraStageController::disconnectCamera()
{
    camera.setSaveToCamera();
    camera.stopLiveView();
    camera.close();
}

void CameraStageController::captureImage()
{
    QDir dir(settings.saveDirectory);
    if (!dir.exists())
        dir.mkpath(settings.saveDirectory);

    camera.takePicture();
}

void CameraStageController::setSaveDirectory(QString directory)
{
    QString temp;

    QDir dir(directory);
    if (!dir.exists())
        dir.mkpath(directory);

    strcpy_s(settings.saveDirectory, directory.toStdString().c_str());

    if (directory.back() != QChar('/'))
        temp = directory + "/";
    else
        temp = directory;

    camera.setSaveDirectory(temp.toStdString().c_str());
}

void CameraStageController::sendGCode(QString gcode)
{
    if (controller.isConnected())
        controller.sendGCode(gcode.toStdString().c_str());
}

bool CameraStageController::stageMessageRec()
{
    return controller.isReady();
}

QString CameraStageController::getStageMessage()
{
    return controller.dataReceived;
}

bool CameraStageController::connectToCOM(QString com)
{
    QString port = "\\\\.\\" + com;
    //controller.disconnectPort();
    return controller.connectPort(port.toStdWString().c_str());
}

void CameraStageController::disconnectCOM()
{
    controller.disconnectPort();
}

void CameraStageController::getListofISO()
{
    camera.listISOspeed();
    std::copy_n(camera.ISOSpeedList.propDesc, sizeof(settings.listISOspeed) / sizeof(settings.listISOspeed[0]), settings.listISOspeed);
    settings.arraySizeISO = camera.ISOSpeedList.numElements;
}

void CameraStageController::setISOspeed(int32_t ISOspeed)
{
    settings.ISOspeed = ISOspeed;
    camera.setISOspeed(ISOspeed);
}

void CameraStageController::getListofShutter()
{
    camera.listShutterSpeed();
    std::copy_n(camera.ShutterSpeedList.propDesc, sizeof(settings.listShutterSpeed) / sizeof(settings.listShutterSpeed[0]), settings.listShutterSpeed);
    settings.arraySizeShutter = camera.ShutterSpeedList.numElements;
}

void CameraStageController::setShutterSpeed(int32_t shutterSpeed)
{
    settings.shutterSpeed = shutterSpeed;
    camera.setShutterSpeed(shutterSpeed);
}

bool CameraStageController::isCameraConnected()
{
    return camera.isConnected();
}

bool CameraStageController::isSerialConnected()
{
    return controller.isConnected();
}
