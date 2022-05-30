#ifndef CAMERASETTINGSDIALOG_H
#define CAMERASETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include "CameraStageController.h"

namespace Ui {
class CameraSettingsDialog;
}

class CameraSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CameraSettingsDialog(CameraStageController &camerastagecontroller_, QWidget *parent = nullptr);
    ~CameraSettingsDialog();

public slots:
    void setCameraSettings();
    void browseDir();

public:
    void setSaveDirectoryText(QString directory);
    QString getSaveDirectoryText();
    QString getISOspeedText();
    void setISOspeedText(int32_t ISOspeed); //Use hex value
    void setAvailableISO(); //Gets the available ISO settings for the connected camera and adds them to the combobox

    void setShutterSpeedText(int32_t shutterSpeed); //Use hex value
    void setAvailableShutter(); //Gets the available shutter settings for the connected camera and adds them to the combobox

    void clear();

private:
    Ui::CameraSettingsDialog *ui;
    CameraStageController &camerastagecontroller;

    std::map<int, QString> ISOspeedMap {
        {0x00000000 , "ISO Auto"}, {0x00000028, "ISO 6"}, {0x00000030 , "ISO 12"}, {0x00000038, "ISO 25"}, {0x00000040 , "ISO 50"},
        {0x00000048, "ISO 100"}, {0x0000004b, "ISO 125"}, {0x0000004d, "ISO 160"}, {0x00000050, "ISO 200"}, {0x00000053, "ISO 250"},
        {0x00000053, "ISO 250"}, {0x00000055, "ISO 320"}, {0x00000058, "ISO 400"}, {0x0000005b, "ISO 500"}, {0x0000005d, "ISO 640"},
        {0x00000060, "ISO 800"}, {0x00000063, "ISO 1000"}, {0x00000065, "ISO 1250"}, {0x00000068, "ISO 1600"}, {0x0000006b, "ISO 2000"},
        {0x0000006d, "ISO 2500"}, {0x00000070, "ISO 3200"}, {0x00000073, "ISO 4000"}, {0x00000075, "ISO 5000"}, {0x00000078, "ISO 6400"},
        {0x0000007b, "ISO 8000"}, {0x0000007d, "ISO 10000"}, {0x00000080, "ISO 12800"}, {0x00000083, "ISO 16000"}, {0x00000085, "ISO 20000"},
        {0x00000088, "ISO 25600"}, {0x0000008b, "ISO 32000"}, {0x0000008d, "ISO 40000"}, {0x00000090, "ISO 51200"}, {0x00000093, "ISO 64000"},
        {0x00000095, "ISO 80000"}, {0x00000098, "ISO 102400"}, {0x000000a0, "ISO 204800"}, {0x000000a8, "ISO 409600"}, {0x000000b0, "ISO 819200"} };

    std::map<int, QString> ShutterSpeedMap {
        {0x0C, "Bulb"}, {0x5D, "1/25"}, {0x10, "30\""}, {0x60, "1/30"}, {0x13, "25\""}, {0x63, "1/40"}, {0x14, "20\""}, {0x64, "1/45"},
        {0x15, "20\" (1/3)"}, {0x65, "1/50"}, {0x18, "15\""}, {0x68, "1/60"}, {0x1B, "13\""}, {0x6B, "1/80"}, {0x1C, "10\""}, {0x6C, "1/90"},
        {0x1D, "10\" (1/3)"}, {0x6D, "1/100"}, {0x20, "8\""}, {0x70, "1/125"}, {0x23, "6\" (1/3)"}, {0x73, "1/160"}, {0x24, "6\""}, {0x74, "1/180"},
        {0x25, "5\""}, {0x75, "1/200"}, {0x28, "4\""}, {0x78, "1/250"}, {0x2B, "3\"2"}, {0x7B, "1/320"}, {0x2C, "3\""}, {0x7C, "1/350"},
        {0x2D, "2\"5"}, {0x7D, "1/400"}, {0x30, "2\""}, {0x80, "1/500"}, {0x33, "1\"6"}, {0x83, "1/640"}, {0x34, "1\"5"}, {0x84, "1/750"},
        {0x35, "1\"3"}, {0x85, "1/800"}, {0x38, "1\""}, {0x88, "1/1000"}, {0x3B, "0\"8"}, {0x8B, "1/1250"}, {0x3C, "0\"7"}, {0x8C, "1/1500"},
        {0x3C, "0\"7"}, {0x8C, "1/1500"}, {0x3D, "0\"6"}, {0x8D, "1/1600"}, {0x40, "0\"5"}, {0x90, "1/2000"}, {0x43, "0\"4"}, {0x93, "1/2500"},
        {0x44, "0\"3"}, {0x94, "1/3000"}, {0x45, "0\"3 (1/3)"}, {0x95, "1/3200"}, {0x48, "1/4"}, {0x98, "1/4000"}, {0x4B, "1/5"}, {0x9B, "1/5000"},
        {0x4C, "1/6"}, {0x9C, "1/6000"}, {0x4D, "1/6 (1/3)"}, {0x9D, "1/6400"}, {0x50, "1/8"}, {0xA0, "1/8000"}, {0x53, "1/10 (1/3)"}, {0xA3, "1/10000"},
        {0x54, "1/10"}, {0xA5, "1/12800"}, {0x55, "1/13"}, {0xA8, "1/16000"}, {0x58, "1/15"}, {0xffffffff, "Not valid"}, {0x5B, "1/20 (1/3)"}, {0x5C, "1/20"} };
};

#endif // CAMERASETTINGSDIALOG_H
