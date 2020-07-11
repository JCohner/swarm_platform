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
#include <devices/cc13x0/driverlib/rf_common_cmd.h>
#include "zumo_rf.h"
#include "uart.h"
#include "CC1310_LAUNCHXL.h"
#include "gpio.h"

#include "state_track.h"

#include <stdlib.h>

/***** Defines *****/
#define PAYLOAD_LENGTH          50
#define PACKET_INTERVAL_US      200000
/* Number of times the CS command should run when the channel is BUSY */
#define CS_RETRIES_WHEN_BUSY    100
/* The channel is reported BUSY is the RSSI is above this threshold */
#define RSSI_THRESHOLD_DBM      -80
#define IDLE_TIME_US            5000
/* Proprietary Radio Operation Status Codes Number: Operation ended normally */
#define PROP_DONE_OK            0x3400
#define PROP_DONE_IDLE          0x3407
#define PROP_DONE_BUSY          0x3408 //from rf_prop_mailbox
#define PROP_DONE_IDLETIMEOUT   0x3409  ///< Carrier sense operation ended because of timeout with csConf.timeoutRes = 1
#define PROP_DONE_BUSYTIMEOUT   0x340A
#define TX_DELAY             (uint32_t)(4000000*0.1f) //TODO: add this delay
#define IDLE_MAX 50
#define DELTA_TIME_BUFF_SIZE 10

/* NOTE: Only two data entries supported at the moment */
#define NUM_DATA_ENTRIES       2
/* The Data Entries data field will contain:
 * 1 Header byte (RF_cmdPropRx.rxConf.bIncludeHdr = 0x1)
 * Max 30 payload bytes
 * 1 status byte (RF_cmdPropRx.rxConf.bAppendStatus = 0x1) */
#define NUM_APPENDED_BYTES     2

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

#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(rxDataEntryBuffer, 4)
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                  PAYLOAD_LENGTH,
                                                  NUM_APPENDED_BYTES)];
#elif defined(__GNUC__)
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                  PAYLOAD_LENGTH,
                                                  NUM_APPENDED_BYTES)]
                                                  __attribute__((aligned(4)));
#else
#error This compiler is not supported
#endif //defined(__TI_COMPILER_VERSION__)

/***** Prototypes *****/
//static void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
void sniff_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
/*Globals for biosynchronicity*/
//uint32_t curr_time;

//uint32_t message_time;
//uint32_t prev_message_time = 0;
//uint32_t delta_message_time = 10;
//uint32_t idle_count = 0;
//uint32_t delta_message_time_buff[DELTA_TIME_BUFF_SIZE];
//bool receive_buff_full_flag = false;
//
//uint32_t delta_message_time_average = 10;
//uint32_t num_receives = 0;

static uint8_t resp_flag;
//uint8_t heard_since_last;
//uint32_t curr_count = 0;
//uint8_t period_flag = 1;

static uint8_t sent_flag;

void rf_setup()
{
    RF_Params rfParams;
    RF_Params_init(&rfParams);

    if (RFQueue_defineQueue(&dataQueue,
                               rxDataEntryBuffer,
                               sizeof(rxDataEntryBuffer),
                               NUM_DATA_ENTRIES,
                               PAYLOAD_LENGTH + NUM_APPENDED_BYTES))
    {
        WriteUART0("queue not set up\r\n");
        while(1);
    }

    triggerCmd.commandNo = CMD_TRIGGER;
    triggerCmd.triggerNo = 1;

    /* Customize the CMD_PROP_TX command for this application */
//    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
//    RF_cmdPropTx.pPkt = packet;
//    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;

    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdNop.startTrigger.triggerType = TRIG_ABSTIME;
    RF_cmdNop.startTrigger.pastTrig = 1;


    /* Set up the next pointers for the command chain */
    RF_cmdNop.pNextOp = (rfc_radioOp_t*)&RF_cmdPropCs;
    RF_cmdPropCs.pNextOp = (rfc_radioOp_t*)&RF_cmdCountBranch;
    RF_cmdCountBranch.pNextOp = (rfc_radioOp_t*)&RF_cmdPropTx;
    RF_cmdCountBranch.pNextOpIfOk = (rfc_radioOp_t*)&RF_cmdPropCs;

    /* Customize the API commands with application specific defines */
    RF_cmdPropCs.rssiThr = RSSI_THRESHOLD_DBM;
    RF_cmdPropCs.csEndTime = (IDLE_TIME_US + 150) * 4; /* Add some margin */
    RF_cmdCountBranch.counter = CS_RETRIES_WHEN_BUSY;


   rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);

   /* Set the frequency */
   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

   RF_cmdFlush.pQueue = &dataQueue;
   sent_flag = 1;

   /* Get current time */
   time = RF_getCurrentTime();
   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdNop, RF_PriorityNormal,
             &callback, 0);
   //MIGHT WANNA SWITCH THIS INITAL TX TO AN INITAL NOP CALL
//   sprintf((packet + 2), "MACH: 0\r\n"); //TODO: would be nice tie to serial number of guy
   /*init shout*/
//   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
//                         &callback, (RF_EventCmdDone | RF_EventLastCmdDone));
}
RF_CmdHandle rxCommandHandle;
RF_CmdHandle txCommandHandle;
RF_EventMask events;




char buffer[50];
void rf_main()
{
    /* Create packet with incrementing sequence number & random payload */
    if (sent_flag)
    {
        /* Set absolute TX time to utilize automatic power management */
        time += (PACKET_INTERVAL_US);
        RF_cmdNop.startTime = time;

        packet[0] = (uint8_t)(seqNumber >> 8);
        packet[1] = (uint8_t)(seqNumber);

        RF_postCmd(rfHandle, (RF_Op*)&RF_cmdNop, RF_PriorityNormal,
                  &callback, 0);

//        WriteUART0("posting\r\n");




//        sprintf((packet + 2), "%u %u %u\r\n", get_policy(), get_target_flag(), (get_return_flag() << 2 | get_xc_state()));

//        RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
//                                             &sneeze_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
        sent_flag = 0;
    }
}



void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
//    if ((RF_cmdPropTx.status == PROP_DONE_OK) && (e & RF_EventLastCmdDone))
    if ((e & RF_EventLastCmdDone) && (RF_cmdPropTx.status == PROP_DONE_OK))
    {
        seqNumber++;
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
//        WriteUART0("AH SHOUTING\r\n");


    }

    //encode logic for the other states (ie when the carrier sense detects the line is busy

    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);

    RF_cmdNop.status = IDLE;
    RF_cmdPropCs.status = IDLE;
    RF_cmdCountBranch.status = IDLE;
    RF_cmdPropTx.status = IDLE;
    RF_cmdCountBranch.counter = CS_RETRIES_WHEN_BUSY;
    sent_flag = 1;
}


void WriteRF(const char * string)
{
    sprintf(packet+2, string);
//    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
//                                                 &sneeze_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
//    sent_flag = 0;
}

//bit writes states
void WriteRFState(uint8_t state)
{
    packet[2] = state;
}


//void sniff_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
//{
//}

