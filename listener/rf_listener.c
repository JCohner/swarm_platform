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
#include <rf_listener.h>
#include "uart.h"
#include "CC1310_LAUNCHXL.h"
#include "gpio.h"

#include "print_info.h"

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
#define TX_DELAY             (uint32_t)(4000000*0.1f)
#define IDLE_MAX 50

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
#endif //defined(__TI_COMPILER_VERSION__)

/***** Prototypes *****/
//static void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
//void sneeze_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
void listen_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
void TX_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
/*Globals for biosynchronicity*/
uint32_t curr_time;
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

    /* Modify CMD_PROP_TX and CMD_PROP_RX commands for application needs */
    /* Set the Data Entity queue for received data */
    RF_cmdPropRx.pQueue = &dataQueue;
    /* Discard ignored packets from Rx queue */
    RF_cmdPropRx.rxConf.bAutoFlushIgnored = 1;
    /* Discard packets with CRC error from Rx queue */
    RF_cmdPropRx.rxConf.bAutoFlushCrcErr = 1;
    /* Implement packet length filtering to avoid PROP_ERROR_RXBUF */
    RF_cmdPropRx.maxPktLen = PAYLOAD_LENGTH;
    /* End RX operation when a packet is received correctly and move on to the
     * next command in the chain */
    RF_cmdPropRx.pktConf.bRepeatOk = 0;
    RF_cmdPropRx.pktConf.bRepeatNok = 1;
    RF_cmdPropRx.startTrigger.triggerType = TRIG_NOW;
//    RF_cmdPropRx.pNextOp = (rfc_radioOp_t *)&RF_cmdPropTx;
    /* Only run the TX command if RX is successful */
    RF_cmdPropRx.condition.rule = COND_STOP_ON_FALSE;
    RF_cmdPropRx.pOutput = (uint8_t *)&rxStatistics;
//
    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;
//    RF_cmdPropTx.startTime = TX_DELAY;

   //request acces to radio
   rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);

   /* Set the frequency */
   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

   /*init listen*/


   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx, RF_PriorityNormal,
                         &listen_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
}
RF_CmdHandle rxCommandHandle;
RF_CmdHandle txCommandHandle;
RF_EventMask events;

uint32_t curr_count = 0;
static uint8_t resp_flag = 0;


char trigNo = 0;
char buffer[50];
void rf_main()
{
    //if response received issue new RxSniff
//    WriteUART0("sup\r\n");
    if (resp_flag)
    {
        rxCommandHandle =
                RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx, RF_PriorityNormal,
                                      &listen_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
        resp_flag = 0;
    }
//    WriteUART0(buffer);
    //if the idle_count is greater than halg the delta and you have heard from someone chirp //TODO: this is wrong

    if (rxCommandHandle < 0)
    {
        WriteUART0("queue full\r\n");
    }


//    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);
}

void listen_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
//    sprintf(buffer, "sniff status code is: %X\r\n", RF_cmdPropRxSniff.status);
//    WriteUART0(buffer);
    //if we successfully recevied
    if (e & RF_EventRxEntryDone || e & RF_EventCmdDone)
    {
        /* Get current unhandled data entry */
        currentDataEntry = RFQueue_getDataEntry();

        /* Handle the packet data, located at &currentDataEntry->data:
         * - Length is the first byte with the current configuration
         * - Data starts from the second byte */
        packetLength      = *(uint8_t *)(&(currentDataEntry->data));
        packetDataPointer = (uint8_t *)(&(currentDataEntry->data) + 1);
        RFQueue_nextEntry();

//        sprintf(buffer, "seq: %d\r\n",((*(packetDataPointer) << 8) | *(packetDataPointer + 1)));
//        WriteUART0(buffer);

//        sprintf(buffer, "policy: %u\r\ntarget flag: %u\r\nxc_state: %u\r\nret_state: %u\r\n", *(packetDataPointer + 2),
//                        *(packetDataPointer + 3), *(packetDataPointer + 4), *(packetDataPointer + 5));
//        WriteUART0(buffer);
//        WriteUART0("me guy: ");
//        sprintf(buffer, "%u %u %u %u %u %u\r\n", *(packetDataPointer + 2), *(packetDataPointer + 3),
//                                                    *(packetDataPointer + 4), *(packetDataPointer + 5),
//                                                    *(packetDataPointer + 6), *(packetDataPointer + 7));
//
//        WriteUART0(buffer);

        //Standard Communication Packet
        uint32_t data = (*(packetDataPointer + 2) << 24) | (*(packetDataPointer + 3) << 16) | (*(packetDataPointer +4) << 8) | (*(packetDataPointer +5));
        print_info(data);



        resp_flag = 1;

    }
}

void rf_post_message(uint32_t data)
{
//    sprintf(buffer, "di: %X\r\n", data);
//    WriteUART0(buffer);
    packet[2] = (data & 0xFF000000) >> 24;
    packet[3] = (data & 0x00FF0000) >> 16; //writes high byte first
    packet[4] = (data & 0x0000FF00) >> 8; //low byte second
    packet[5] = (data & 0x000000FF);

//    sprintf(buffer, "sending: %X\r\n", packet[2] << 24 | packet[3] << 16 | packet[4] << 8 | packet[5]);
//    WriteUART0(buffer);
    int i;
    for (i = 0; i < 5; i++)
    {
        RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
                   &TX_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
    }

}



void TX_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    if ((RF_cmdPropTx.status == PROP_DONE_OK) && (e & RF_EventLastCmdDone))
    {
        seqNumber++;
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);

    }

}
