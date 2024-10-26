#include "can.h"
#include "hal.h"
#include "port.h"


static THD_WORKING_AREA(waCanTxThread, 256);
void CanTxThread(void*)
{
    chRegSetThreadName("CAN Tx");

    CANTxFrame canTxMsg;

    while(1)
    {
        canTxMsg.IDE = CAN_IDE_STD;
        canTxMsg.SID = 0x123;
        canTxMsg.DLC = 8;
        canTxMsg.data8[0] = 0x01;
        canTxMsg.data8[1] = 0x02;
        canTxMsg.data8[2] = 0x03;
        canTxMsg.data8[3] = 0x04;
        canTxMsg.data8[4] = 0x05;
        canTxMsg.data8[5] = 0x06;
        canTxMsg.data8[6] = 0x07;
        canTxMsg.data8[7] = 0x08;

        canTransmit(&CAND1, CAN_ANY_MAILBOX, &canTxMsg, TIME_INFINITE);

        chThdSleepMilliseconds(100);
    }
}

void InitCan()
{
    canStart(&CAND1, &GetCanConfig());
    chThdCreateStatic(waCanTxThread, sizeof(waCanTxThread), NORMALPRIO, CanTxThread, nullptr);
}