#include "analog.h"
#include "ch.h"
#include "hal.h"
#include "port.h"

adcsample_t adc1_samples[ADC1_NUM_CHANNELS] = {0};
adcsample_t adc2_samples[ADC2_NUM_CHANNELS] = {0};

msg_t adc1Res;
msg_t adc2Res;

static const ADCConversionGroup adc1_cfg = {
    .circular = true,
    .num_channels = ADC1_NUM_CHANNELS,
    .end_cb = NULL,
    .error_cb = NULL,
    .cfgr =   ADC_CFGR_CONT | ADC_CFGR_OVRMOD | ADC_CFGR_RES_12BITS,                        //CFGR: Configuration register
    .tr1 = ADC_TR_DISABLED,                               //TR1: Watchdog threshold 1 register
    .tr2 = ADC_TR_DISABLED,                               //TR2: Watchdog threshold 2 register
    .tr3 = ADC_TR_DISABLED,                               //TR3: Watchdog threshold 3 register
    .awd2cr = 0,                                          //AWD2CR: Analog watchdog 2 configuration register
    .awd3cr = 0,                                          //AWD3CR: Analog watchdog 3 configuration register
    .smpr = { ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_601P5) |
            ADC_SMPR1_SMP_AN2(ADC_SMPR_SMP_601P5) |
            ADC_SMPR1_SMP_AN3(ADC_SMPR_SMP_601P5) |
            ADC_SMPR1_SMP_AN4(ADC_SMPR_SMP_601P5), 
            ADC_SMPR2_SMP_AN16(ADC_SMPR_SMP_601P5) },   //SMPR: Sampling time register
    .sqr = {  ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1) |         //SQR: Regular sequence register
            ADC_SQR1_SQ2_N(ADC_CHANNEL_IN2) | 
            ADC_SQR1_SQ3_N(ADC_CHANNEL_IN3) | 
            ADC_SQR1_SQ4_N(ADC_CHANNEL_IN4), 
            ADC_SQR2_SQ5_N(ADC_CHANNEL_IN16), 
            0, 
            0 }
};

static const ADCConversionGroup adc2_cfg = {
  .circular = true,
  .num_channels = ADC2_NUM_CHANNELS,
  .end_cb = NULL,
  .error_cb = NULL,
  .cfgr =   ADC_CFGR_CONT | ADC_CFGR_OVRMOD | ADC_CFGR_RES_12BITS,                              //CFGR: Configuration register
  .tr1 = ADC_TR_DISABLED,                               //TR1: Watchdog threshold 1 register
  .tr2 = ADC_TR_DISABLED,                               //TR2: Watchdog threshold 2 register
  .tr3 = ADC_TR_DISABLED,                               //TR3: Watchdog threshold 3 register
  .awd2cr = 0,                                          //AWD2CR: Analog watchdog 2 configuration register
  .awd3cr = 0,                                          //AWD3CR: Analog watchdog 3 configuration register
  .smpr = { ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_601P5), 
            0 },                                        //SMPR: Sampling time register
  .sqr = {  ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1), 
            0, 
            0, 
            0 }                                         //SQR: Regular sequence register
};

static THD_WORKING_AREA(waAdcThread, 128);
static THD_FUNCTION(AdcThread, p)
{
    (void)p;
    chRegSetThreadName("ADC");

    while (true)
    {   
        adc1Res = adcConvert(&ADCD1, &adc1_cfg, adc1_samples, ADC1_BUF_DEPTH);
        adc2Res = adcConvert(&ADCD2, &adc2_cfg, adc2_samples, ADC2_BUF_DEPTH);
        chThdSleepMilliseconds(250);
    }
}

void InitAdc()
{
    adcStart(&ADCD1, NULL);
    adcSTM32EnableTS(&ADCD1); // Enable temperature sensor

    adcStart(&ADCD2, NULL);

    chThdCreateStatic(waAdcThread, sizeof(waAdcThread), NORMALPRIO + 2, AdcThread, NULL);
}

adcsample_t ReadAdc(AnalogChannel channel)
{
    switch (channel)
    {
        case AnalogInput1:
            return adc1_samples[0];
        case AnalogInput2:
            return adc1_samples[1];
        case AnalogInput3:
            return adc1_samples[2];
        case AnalogInput4:
            return adc1_samples[3];
        case AnalogInput5:
            return adc2_samples[0];
        case AnalogTempSensor:
            return adc1_samples[4];
        default:
            return 0; // Invalid channel
    }
}