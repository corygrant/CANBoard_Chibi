#include "can.h"
#include "hal.h"
#include "port.h"
#include "analog.h"
#include "canboard_config.h"

static const CANFilter filters[1] = {
    {
        .filter = 0,
        .mode = 0,                                         // Identifier mask mode (0 for mask mode)
        .scale = 1,                                        // 32-bit scale (1 for 32-bit)
        .assignment = 0,                                   // Assign to FIFO0
        .register1 = ((uint32_t)CAN_BASE_ID << 21),         // ID 0x666 shifted as unsigned 32-bit value
        .register2 = ((uint32_t)0x7FFU << 21) | (1U << 2), // Standard ID mask, reject extended frames
    },
};

static THD_WORKING_AREA(waCanTxThread, 256);
void CanTxThread(void*)
{
    chRegSetThreadName("CAN Tx");

    CANTxFrame canTxMsg;

    while(1)
    {
        canTxMsg.IDE = CAN_IDE_STD;
        canTxMsg.SID = CAN_BASE_ID;
        canTxMsg.DLC = 8;
        canTxMsg.data16[0] = GetAdcRaw(AnalogInput1);
        canTxMsg.data16[1] = GetAdcRaw(AnalogInput2);
        canTxMsg.data16[2] = GetAdcRaw(AnalogInput3);
        canTxMsg.data16[3] = GetAdcRaw(AnalogInput4);

        canTransmitTimeout(&CAND1, CAN_ANY_MAILBOX, &canTxMsg, TIME_INFINITE);

        chThdSleepMilliseconds(CAN_TX_MSG_SPLIT);

        canTxMsg.IDE = CAN_IDE_STD;
        canTxMsg.SID = CAN_BASE_ID + 1;
        canTxMsg.DLC = 8;
        canTxMsg.data16[0] = GetAdcRaw(AnalogInput5);
        canTxMsg.data16[1] = GetTemperature();
        canTxMsg.data16[2] = 0;
        canTxMsg.data16[3] = 0;

        canTransmitTimeout(&CAND1, CAN_ANY_MAILBOX, &canTxMsg, TIME_INFINITE);

        chThdSleepMilliseconds(CAN_TX_MSG_DELAY);
    }
}

static THD_WORKING_AREA(waCanRxThread, 128);
static THD_FUNCTION(CanRxThread, p)
{
    (void)p;

    CANRxFrame canRxMsg;
    CANTxFrame canTxMsg;

    chRegSetThreadName("CAN Rx");

    while (true)
    {
        msg_t msg = canReceiveTimeout(&CAND1, CAN_ANY_MAILBOX, &canRxMsg, TIME_INFINITE);
        if (msg != MSG_OK)
            continue;
        if (canRxMsg.DLC > 0)
        {
            canTxMsg.IDE = CAN_IDE_STD;
            canTxMsg.SID = CAN_BASE_ID + 10;
            canTxMsg.DLC = 2;
            canTxMsg.data8[0] = 0x10;
            canTxMsg.data8[1] = 0x20;
            canTransmitTimeout(&CAND1, CAN_ANY_MAILBOX, &canTxMsg, TIME_INFINITE);
        }
        chThdSleepMilliseconds(100);
    }
}

void InitCan()
{
    canSTM32SetFilters(&CAND1, 0, 1, &filters[0]);
    canStart(&CAND1, &GetCanConfig());
    chThdCreateStatic(waCanTxThread, sizeof(waCanTxThread), NORMALPRIO + 1, CanTxThread, nullptr);
    chThdCreateStatic(waCanRxThread, sizeof(waCanRxThread), NORMALPRIO + 1, CanRxThread, nullptr);
}