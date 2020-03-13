/*
 * zumo_rf.c
 *
 *  Created on: Mar 1, 2020
 *      Author: jambox
 */
/* TI Drivers */
#include <drivers/rf/RF.h>
#include <drivers/power/PowerCC26XX.h>

/* Application Header files */
#include "RFQueue.h"
#include "smartrf_settings/smartrf_settings.h"
#include "application_settings.h"

#include <devices/cc13x0/driverlib/gpio.h>

#include "zumo_rf.h"
#include "uart.h"
#include "CC1310_LAUNCHXL.h"
#include "gpio.h"

#include <stdlib.h>

/***** Defines *****/
#define PAYLOAD_LENGTH          30
#define PACKET_INTERVAL_US      200000
/* Number of times the CS command should run when the channel is BUSY */
#define CS_RETRIES_WHEN_BUSY    10
/* The channel is reported BUSY is the RSSI is above this threshold */
#define RSSI_THRESHOLD_DBM      -80
#define IDLE_TIME_US            5000
/* Proprietary Radio Operation Status Codes Number: Operation ended normally */
#define PROP_DONE_OK            0x3400
#define PROP_DONE_IDLE          0x3407
#define PROP_DONE_BUSY          0x3408 //from rf_prop_mailbox
#define PROP_DONE_IDLETIMEOUT   0x3409  ///< Carrier sense operation ended because of timeout with csConf.timeoutRes = 1
#define PROP_DONE_BUSYTIMEOUT   0x340A
#define TX_DELAY             (uint32_t)(4000000*0.1f)

/***** Variable declarations *****/
static RF_Object rfObject;
static RF_Handle rfHandle;

static uint8_t packet[PAYLOAD_LENGTH];
static uint16_t seqNumber;

static uint32_t time;

/* Receive Statistics */
static rfc_propRxOutput_t rxStatistics;

/* Receive dataQueue for RF Core to fill in data */
static dataQueue_t dataQueue;
static rfc_dataEntryGeneral_t* currentDataEntry;
static uint8_t packetLength;
static uint8_t* packetDataPointer;


/***** Prototypes *****/
static void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

/*Globals for biosynchronicity*/
uint32_t curr_time;
int wait_time = -1;
uint32_t message_time;
uint32_t prev_message_time;
uint32_t delta_message_time = 0;
uint32_t idle_count = 0;
void rf_setup()
{
    RF_Params rfParams;
    RF_Params_init(&rfParams);

    /* Customize the CMD_PROP_TX command for this application */
   RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
   RF_cmdPropTx.pPkt = packet;
   RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;
   //my addition
   RF_cmdPropTx.startTrigger.triggerType = TRIG_REL_PREVEND;
   RF_cmdPropTx.startTime = TX_DELAY;

   /* Modify CMD_PROP_TX and CMD_PROP_RX commands for application needs */
   /* Set the Data Entity queue for received data */
   RF_cmdPropRxSniff.pQueue = &dataQueue;
   /* Discard ignored packets from Rx queue */
   RF_cmdPropRxSniff.rxConf.bAutoFlushIgnored = 1;
   /* Discard packets with CRC error from Rx queue */
   RF_cmdPropRxSniff.rxConf.bAutoFlushCrcErr = 1;
   /* Implement packet length filtering to avoid PROP_ERROR_RXBUF */
   RF_cmdPropRxSniff.maxPktLen = PAYLOAD_LENGTH;
   /* End RX operation when a packet is received correctly and move on to the
    * next command in the chain */
   RF_cmdPropRxSniff.pktConf.bRepeatOk = 0;
   RF_cmdPropRxSniff.pktConf.bRepeatNok = 1; //just keeps listening after failed receive
   RF_cmdPropRxSniff.startTrigger.triggerType = TRIG_NOW;
   RF_cmdPropRxSniff.pNextOp = (rfc_radioOp_t *)&RF_cmdPropTx;
   /* Only run the TX command if RX is successful */
   RF_cmdPropRxSniff.condition.rule = COND_STOP_ON_FALSE;
   RF_cmdPropRxSniff.pOutput = (uint8_t *)&rxStatistics;

   //dont set this as next ok, do control flow differently
//   RF_cmdPropRxSniff.pNextOp = (rfc_radioOp_t*)&RF_cmdPropTx;

   /*Set Carrier Sense Properties*/
   RF_cmdPropRxSniff.rssiThr  = RSSI_THRESHOLD_DBM;
   RF_cmdPropRxSniff.csEndTime = (IDLE_TIME_US + 150) * 4;



   rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);

   /* Set the frequency */
   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

   /* Get current time */
   time = RF_getCurrentTime();
   curr_time = time;

}

void rf_main()
{
    /* Create packet with incrementing sequence number & random payload */
    packet[0] = (uint8_t)(seqNumber >> 8);
    packet[1] = (uint8_t)(seqNumber);
    uint8_t i;
    for (i = 2; i < PAYLOAD_LENGTH; i++)
    {
        packet[i] = rand();
    }

//    /* Set absolute TX time to utilize automatic power management */
//    time += (PACKET_INTERVAL_US * 4);
//    RF_cmdNop.startTime = time;

    /* Sniff */
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRxSniff, RF_PriorityNormal,
              &callback, (RF_EventCmdDone |
                      RF_EventLastCmdDone | RF_EventRxEntryDone));

    if (curr_time > wait_time && !(wait_time < 0))
    {


        curr_time = 0;
    }

    if((idle_count > 12) && (wait_time < 0))
    {
        idle_count = 0;
        RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
                  &callback, (RF_EventCmdDone | RF_EventLastCmdDone));
        WriteUART0("sup  bithc\n\r");
    }

}



void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
//    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
    if (RF_cmdPropTx.status == PROP_DONE_OK)
    {
        seqNumber++;
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);
//        WriteUART0("call back in if\n\r");
    }

//    if ((RF_cmdPropCs.status == PROP_DONE_BUSY) || (RF_cmdPropCs.status == PROP_DONE_BUSYTIMEOUT))
//    {
//        message_time = RF_getCurrentTime();
//        delta_message_time = (message_time - prev_message_time) * 3;
//        prev_message_time = message_time;
//        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);
//    }

    if(RF_cmdPropRxSniff.status == PROP_DONE_IDLE)
    {
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
        idle_count++;
    }

    if (e & RF_EventRxEntryDone)
    {
        message_time = RF_getCurrentTime();
        delta_message_time = (message_time - prev_message_time) * 3;
        prev_message_time = message_time;
        /* Get current unhandled data entry */
        currentDataEntry = RFQueue_getDataEntry();

        /* Handle the packet data, located at &currentDataEntry->data:
         * - Length is the first byte with the current configuration
         * - Data starts from the second byte */
        packetLength      = *(uint8_t *)(&(currentDataEntry->data));
        packetDataPointer = (uint8_t *)(&(currentDataEntry->data) + 1);
        RFQueue_nextEntry();
//        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);
    }


//    WriteUART0("call back made binch\r\n");
}
