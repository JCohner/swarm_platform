/*
 * zumo_rf.c
 *
 *  Created on: Mar 1, 2020
 *      Author: jambox
 */
#include <devices/cc13x0/driverlib/rf_common_cmd.h>
#include <devices/cc13x0/driverlib/rf_prop_cmd.h>
#include <devices/cc13x0/driverlib/rf_mailbox.h>
#include <devices/cc13x0/driverlib/rf_data_entry.h>
#include <devices/cc13x0/rf_patches/rf_patch_cpe_genfsk.h>
#include <devices/cc13x0/rf_patches/rf_patch_rfe_genfsk.h>

/* TI Drivers */
#include <drivers/rf/RF.h>

//#include <devices/cc13x0/inc/hw_memmap.h>
//#include <devices/cc13x0/inc/hw_ints.h>
//#include <devices/cc13x0/inc/hw_types.h>
//#include <devices/cc13x0/inc/hw_rfc_rat.h>
//#include <devices/cc13x0/inc/hw_rfc_dbell.h>

////includes for rf open and etc
//#include <devices/cc13x0/driverlib/rfc.h>
//#include <devices/cc13x0/driverlib/sys_ctrl.h>
//#include <devices/cc13x0/driverlib/ioc.h>
//#include <devices/cc13x0/driverlib/aon_ioc.h>
////#include <devices/cc13x0/driverlib/rf_mailbox.h>
//#include <devices/cc13x0/driverlib/adi.h>
//#include <devices/cc13x0/driverlib/aon_rtc.h>
//#include <devices/cc13x0/driverlib/chipinfo.h>
//#include <devices/cc13x0/driverlib/aon_batmon.h>
//#include <devices/cc13x0/driverlib/osc.h>
//#include <devices/cc13x0/driverlib/ccfgread.h>

/* Application Header files */
#include "RFQueue.h"
#include "smartrf_settings/smartrf_settings.h"

/***** Defines *****/
/* Packet RX/TX Configuration */
/* Max length byte the radio will accept */
#define PAYLOAD_LENGTH         30
/* Set Transmit (echo) delay to 100ms */
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

/* Buffer which contains all Data Entries for receiving data.
 * Pragmas are needed to make sure this buffer is aligned to a 4 byte boundary
 * (requirement from the RF core)
 */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(rxDataEntryBuffer, 4)
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                  PAYLOAD_LENGTH,
                                                  NUM_APPENDED_BYTES)];
#endif //defined(__TI_COMPILER_VERSION__)

/* Receive Statistics */
static rfc_propRxOutput_t rxStatistics;

/* Receive dataQueue for RF Core to fill in data */
static dataQueue_t dataQueue;
static rfc_dataEntryGeneral_t* currentDataEntry;
static uint8_t packetLength;
static uint8_t* packetDataPointer;


static uint8_t txPacket[PAYLOAD_LENGTH];

uint32_t curtime;
uint32_t message_time;
uint32_t prev_message_time;
uint32_t delta_message_time = 0;
void rf_setup()
{
    RF_Params rfParams;
    RF_Params_init(&rfParams);
    if( RFQueue_defineQueue(&dataQueue,
                                rxDataEntryBuffer,
                                sizeof(rxDataEntryBuffer),
                                NUM_DATA_ENTRIES,
                                PAYLOAD_LENGTH + NUM_APPENDED_BYTES)){
        ;
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
    RF_cmdPropRx.pktConf.bRepeatNok = 1; //just keeps listening after failed receive
    RF_cmdPropRx.startTrigger.triggerType = TRIG_NOW;
    RF_cmdPropRx.pNextOp = (rfc_radioOp_t *)&RF_cmdPropTx;
    /* Only run the TX command if RX is successful */
    RF_cmdPropRx.condition.rule = COND_STOP_ON_FALSE;
    RF_cmdPropRx.pOutput = (uint8_t *)&rxStatistics;

    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);


}


