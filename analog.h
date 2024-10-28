#pragma once

#include "ch.h"
#include "hal.h"

#define ADC1_NUM_CHANNELS   5
#define ADC1_BUF_DEPTH      1

#define ADC2_NUM_CHANNELS   1
#define ADC2_BUF_DEPTH      1

enum AnalogChannel
{
    AnalogInput1 = 0,
    AnalogInput2,
    AnalogInput3,
    AnalogInput4,
    AnalogInput5,
    AnalogTempSensor
};

void InitAdc();
adcsample_t ReadAdc(AnalogChannel channel);