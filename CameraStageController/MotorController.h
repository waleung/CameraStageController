#pragma once

#include "SerialClass.h"
#include <iostream>
#include <ctime>

class MotorController
{
public:
	MotorController();
	~MotorController();

public:
    bool connectPort(const wchar_t* portName);
    void disconnectPort();
	void sendGCode(const char GCode[50]);
	bool readData();
	bool isReady();
	void waitForReady();
    bool isConnected();

private:


public:
	char dataReceived[256];


private:
    Serial* SP = nullptr;
	bool dataSent = false;
	std::clock_t c_start = 0;
	std::clock_t c_end = 0;
};

