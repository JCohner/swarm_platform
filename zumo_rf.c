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
void sneeze_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
void sniff_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
/*Globals for biosynchronicity*/
uint32_t curr_time;
int wait_time = -1;
uint32_t message_time;
uint32_t prev_message_time;
uint32_t delta_message_time = 0;
uint32_t idle_count = 0;

bool neighbors = false;
uint8_t resp_flag;
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

    /* Customize the CMD_PROP_TX command for this application */
    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdNop.startTrigger.triggerType = TRIG_ABSTIME;
    RF_cmdNop.startTrigger.pastTrig = 1;

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
   RF_cmdPropRxSniff.pktConf.bRepeatNok = 0; //just keeps listening after failed receive
   RF_cmdPropRxSniff.startTrigger.triggerType = TRIG_NOW;
//   RF_cmdPropRxSniff.pNextOp = (rfc_radioOp_t *)&RF_cmdPropTx;
   /* Only run the TX command if RX is successful */
   RF_cmdPropRxSniff.condition.rule = COND_STOP_ON_FALSE;
   RF_cmdPropRxSniff.pOutput = (uint8_t *)&rxStatistics;

   //dont set this as next ok, do control flow differently
//   RF_cmdPropRxSniff.pNextOp = (rfc_radioOp_t*)&RF_cmdPropTx;

   /*Set Carrier Sense Properties*/
   RF_cmdPropRxSniff.rssiThr  = RSSI_THRESHOLD_DBM;
   RF_cmdPropRxSniff.csEndTime = (IDLE_TIME_US + 150) * 4;
//   RF_cmdPropRxSniff.numRssiIdle = 0x10;

    /*Nop gives us clean entry point*/
    RF_cmdNop.startTrigger.triggerType = TRIG_ABSTIME;
    RF_cmdNop.startTrigger.pastTrig = 1;
//    RF_cmdNop.pNextOp = (rfc_radioOp_t*)&RF_cmdPropRxSniff;
//    RF_cmdPropRxSniff.pNextOp = (rfc_radioOp_t*)&RF_cmdPropTx;
//    RF_cmdCountBranch.pNextOp = (rfc_radioOp_t*)&RF_cmdPropTx; //pointer once this counter expires
//    RF_cmdCountBranch.pNextOpIfOk = (rfc_radioOp_t*)&RF_cmdPropTx; //if the counter did not expire

    /*setup branch counter*/
//    RF_cmdCountBranch.counter = CS_RETRIES_WHEN_BUSY;

   rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);

   /* Set the frequency */
   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

   /* Get current time */
   time = RF_getCurrentTime();
   curr_time = time;

   RF_cmdFlush.pQueue = &dataQueue;
   resp_flag = 1;

   sprintf((packet + 2), "MACH: 0\r\n");
   /*init shout*/
   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
                         &sneeze_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
}
RF_CmdHandle rxCommandHandle;
RF_CmdHandle txCommandHandle;
RF_EventMask events;

char buffer[50];
void rf_main()
{
    /* Create packet with incrementing sequence number & random payload */
    packet[0] = (uint8_t)(seqNumber >> 8);
    packet[1] = (uint8_t)(seqNumber);

    rxCommandHandle =
            RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRxSniff, RF_PriorityNormal,
              &sniff_callback, (RF_EventCmdDone |
                      RF_EventLastCmdDone | RF_EventRxEntryDone));
    RF_cmdPropTx.startTime = TX_DELAY + delta_message_time/2.0;
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
                             &sneeze_callback, (RF_EventCmdDone | RF_EventLastCmdDone));

    if (rxCommandHandle < 0)
    {
        WriteUART0("queue full\r\n");
    }

}

void sneeze_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    if ((RF_cmdPropTx.status == PROP_DONE_OK) && (e & RF_EventLastCmdDone))
    {
        seqNumber++;
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);
        WriteUART0("shouting\r\n");
    }

}




void sniff_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    sprintf(buffer, "sniff status code is: %X\r\n", RF_cmdPropRxSniff.status);
    WriteUART0(buffer);

    //if sniff returned idle
    if(RF_cmdPropRxSniff.status == PROP_DONE_IDLE)
    {
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);

//        WriteUART0("theres no one out there :(\r\n");
        idle_count++;
        sprintf(buffer, "idle count at: %u\r\n", idle_count);
        WriteUART0(buffer);
        resp_flag = 0;
    }
//
    //if sniff returned bsy
    if(RF_cmdPropRxSniff.status == PROP_DONE_BUSY)
    {
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);

        WriteUART0("line busy\r\n");
        resp_flag = 1;
    }

    //if we successfully recevied
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


//        sprintf(buffer, "got packet of length: %u\r\n", packetLength);
//        WriteUART0(buffer);
        sprintf(buffer, "seq: %d\r\n",((*(packetDataPointer) << 8) | *(packetDataPointer + 1)));
        WriteUART0(buffer);
        WriteUART0((char *) (packetDataPointer + 2));

        sprintf(buffer, "delta time: %u\r\n", delta_message_time);
        WriteUART0(buffer);
//        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);

        //on successful rx set resp flag high
        idle_count = 0;
        resp_flag = 1;
        WriteUART0("heard from someone resp flag set high\r\n");
    }
}

