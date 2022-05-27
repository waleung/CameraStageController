#include "MotorController.h"

MotorController::MotorController() {}

MotorController::~MotorController() {}

bool MotorController::connectPort(const wchar_t* portName)
{
    //delete SP;
	SP = new Serial(portName);

    return SP->IsConnected();
}

void MotorController::disconnectPort()
{
    if (SP->IsConnected())
        delete SP;
}

bool MotorController::isConnected()
{
    if (SP != nullptr)
        return SP->IsConnected();
    else
        return false;

}

void MotorController::sendGCode(const char GCode[50])
{
    if (SP != nullptr)
    {
        int char_len = 0;
        char sendData[50];

        strcpy_s(sendData, GCode);
        strcat_s(sendData, "\n");
        char_len = strlen(sendData);

        if (SP->WriteData(sendData, char_len))
            dataSent = true;
    }
}

bool MotorController::readData()
{
	int readResult = 0;
	char incomingData[256] = "";
	int dataLength = 255;

	readResult = SP->ReadData(incomingData, dataLength);

	if (incomingData[strlen(incomingData) - 1] == '\n')
	{
		incomingData[readResult] = 0;
		strcpy_s(dataReceived, incomingData);
		dataSent = false;
		return true;
	}
	return false;
}

bool MotorController::isReady()
{
	long double time_elapsed_ms = 0;

	if (!dataSent)
		return true;

	c_end = std::clock();
	time_elapsed_ms = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;

	if (time_elapsed_ms >= 50)
	{
		c_start = std::clock();
		return readData();
	}
	return false;
}

void MotorController::waitForReady()
{
	bool ready = false;

	while (!ready)
	{
		ready = isReady();
	}
}
