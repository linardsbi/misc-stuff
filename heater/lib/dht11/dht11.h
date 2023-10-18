#pragma once

#include <Arduino.h>

#define DHTLIB_OK				0
#define DHTLIB_ERROR_CHECKSUM	-1
#define DHTLIB_ERROR_TIMEOUT	-2

struct dht11
{
    int read(int pin);
	int humidity;
	float temperature;
};
