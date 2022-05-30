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
    bool isReady(); //Checks if message has been received
    void waitForReady(); //Only use if console app
    bool isConnected();

private:


public:
    char dataReceived[256]; //Variable containing the received message.


private:
    Serial* SP = nullptr;
	bool dataSent = false;
	std::clock_t c_start = 0;
	std::clock_t c_end = 0;
};

