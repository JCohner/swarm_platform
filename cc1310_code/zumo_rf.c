/*
 * zumo_rf.c
 *
 *  Created on: Mar 1, 2020
 *      Author: jambox
 */
/* TI Drivers */
#include "zumo_rf.h"

/***** Prototypes *****/
//static void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
void RX_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
void TX_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
void RX_setup_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

/*Globals for biosynchronicity*/
static uint32_t message_time;
static  uint32_t prev_message_time = 0;
static  uint32_t delta_message_time = 50;
static  uint32_t idle_count = 0;
static  uint32_t delta_message_time_buff[DELTA_TIME_BUFF_SIZE];
static  bool receive_buff_full_flag = false;

static uint32_t delta_message_time_average = 35;
static uint32_t num_receives = 0;

static uint8_t resp_flag;
static uint8_t heard_since_last;
static uint32_t curr_count = 0;
static uint8_t period_flag = 1;
static uint8_t sent_flag;


char buffer[50];
uint8_t uniq_offset;
uint8_t rando_wait;
uint8_t offset_lookup()
{

    uint16_t mach_id = get_mach_id();
    switch(mach_id)
    {
    case 0xC464:
        uniq_offset = 2;
        break;
    case 0xC219:
        uniq_offset = 3;
        break;
    case 0xA171:
        uniq_offset = 5;
        break;
    case 0xC683:
        uniq_offset = 7;
        break;
    case 0x20CE:
        uniq_offset = 11;
        break;
    case 0xB5A8:
        uniq_offset = 13;
        break;
    case 0xC262:
        uniq_offset = 17;
        break;
    case 0xA3EB:
        uniq_offset = 19;
        break;
    case 0xC718:
        uniq_offset = 23;
        break;
    default:
        sprintf(buffer, "no match for %X\r\n", mach_id);
        WriteUART0(buffer);
//        while(1);
    }

    return uniq_offset;
}



void rf_setup()
{

    offset_lookup();

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
    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
//    RF_cmdPropTx.startTrigger.triggerType = TRIG_REL_SUBMIT;
//    RF_cmdPropTx.startTime = offset_lookup();

    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;
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
    RF_cmdPropRx.pktConf.bRepeatNok = 0; //just keeps listening after failed receive
    RF_cmdPropRx.startTrigger.triggerType = TRIG_NOW;
 //   RF_cmdPropRxSniff.pNextOp = (rfc_radioOp_t *)&RF_cmdPropTx;
    /* Only run the TX command if RX is successful */
 //   RF_cmdPropRxSniff.condition.rule = COND_SKIP_ON_FALSE;
 //   RF_cmdPropRxSniff.condition.nSkip = 0x0; //repeat the rxsniff if it retruns idle
    RF_cmdPropRx.pOutput = (uint8_t *)&rxStatistics;

    RF_cmdFlush.pQueue = &dataQueue;
    resp_flag = 1;

   rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);

   /* Set the frequency */
   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

   RF_cmdFlush.pQueue = &dataQueue;
   sent_flag = 1;

   /* Get current time */
//   time = RF_getCurrentTime();
   RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
                         &TX_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
}

RF_CmdHandle rxCommandHandle;
RF_CmdHandle txCommandHandle;
RF_EventMask events;

//uint32_t curr_count = 0;
char trigNo = 0;

void rf_main()
{
    /* Create packet with incrementing sequence number & random payload */
    packet[0] = (uint8_t)(seqNumber >> 8);
    packet[1] = (uint8_t)(seqNumber);

    curr_count++;

    //if response received issue new RxSniff
    if (resp_flag)
    {
        rxCommandHandle =
            RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx, RF_PriorityNormal,
              &RX_callback, (RF_EventCmdDone |
                      RF_EventLastCmdDone | RF_EventRxEntryDone));
        resp_flag = 0;
        rando_wait = get_random_num(11);
    }
    //if no response increment idle counter
    if (resp_flag == 0)
    {
        idle_count++;
    }
//    sprintf(buffer, "idle count is: %u\n\uniq_off is: %u\r\ndelta message time: %u\r\n", idle_count, uniq_offset, delta_message_time);
//    sprintf(buffer, "dmt: %u\r\n", delta_message_time + rando_wait);
//    WriteUART0(buffer);
    //if the idle_count is greater than halg the delta and you have heard from someone chirp //TODO: this is wrong
    if ((idle_count > (delta_message_time/2 + rando_wait)) && (heard_since_last == 1))
    {
        sprintf(buffer,"broadcasting at %u", idle_count);
        WriteUART0(buffer);

        RF_runImmediateCmd(rfHandle, (uint32_t*)&triggerCmd); //kill our listen
        RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
                                 &TX_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
        resp_flag = 1;
        idle_count = 0;
        heard_since_last = 0;
        return;
    }

    //if you havent heard from anyone and idle count is greater than idle max, chirp
    if (idle_count >= IDLE_MAX)
    {
        RF_runImmediateCmd(rfHandle, (uint32_t*)&triggerCmd); //kill our listen
        RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal,
                                 &TX_callback, (RF_EventCmdDone | RF_EventLastCmdDone));
        resp_flag = 1;
        idle_count = 0;
    }


    if (rxCommandHandle < 0)
    {
//        WriteUART0("quue full\r\n");
//        WriteUART0("          \r"); //needs this to work??????
//        RF_cmdFlush.pFirstEntry = dataQueue.pLastEntry;
//        RF_runImmediateCmd(rfHandle, (uint32_t*)&RF_cmdFlush);

    }

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

void WriteRFPacket(uint32_t comm_packet)
{
    //Big Endian
    packet[2] = (comm_packet & 0xFF000000) >> 24;
    packet[3] = (comm_packet & 0x00FF0000) >> 16; //writes high byte first
    packet[4] = (comm_packet & 0x0000FF00) >> 8; //low byte second
    packet[5] = (comm_packet & 0x000000FF);
}

void TX_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    if ((RF_cmdPropTx.status == PROP_DONE_OK) && (e & RF_EventLastCmdDone))
    {
        seqNumber++;
//        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED); //TODO CAUSE IM DUMB I MAPPED THE SAME PIN AS 2nd row leds, needs pcb rework
//        WriteUART0("ah shouting\r\n");
    }

}

void RX_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
//    sprintf(buffer, "sniff status code is: %X\r\n", RF_cmdPropRx.status);
//    WriteUART0(buffer);
    //if we successfully recevied
    if (e & RF_EventRxEntryDone)
    {
//        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED); //TODO CAUSE IM DUMB I MAPPED THE SAME PIN AS 2nd row leds, needs pcb rework

        message_time = curr_count;
        delta_message_time = message_time - prev_message_time;
        prev_message_time = message_time;

        if(delta_message_time < IDLE_MIN)
        {
            delta_message_time = IDLE_MIN;
        }

        if(delta_message_time > IDLE_MAX)
        {
            delta_message_time = IDLE_MAX;
        }


        /* Get current unhandled data entry */
        currentDataEntry = RFQueue_getDataEntry();

        /* Handle the packet data, located at &currentDataEntry->data:
         * - Length is the first byte with the current configuration
         * - Data starts from the second byte */
        packetLength      = *(uint8_t *)(&(currentDataEntry->data));
        packetDataPointer = (uint8_t *)(&(currentDataEntry->data) + 1);
        RFQueue_nextEntry();

        sprintf(buffer, "seq: %d\r\n",((*(packetDataPointer) << 8) | *(packetDataPointer + 1)));
        WriteUART0(buffer);
//        WriteUART0((char *) (packetDataPointer + 2));

//        sprintf(buffer, "delta time: %u\r\n", delta_message_time);
//        WriteUART0(buffer);
        uint32_t info = (*(packetDataPointer + 2) << 24) | (*(packetDataPointer + 3) << 16) | (*(packetDataPointer + 4) << 8) | (*(packetDataPointer + 5));
        sprintf(buffer, "info: %X\r\n", info);
        WriteUART0(buffer);
        evaluate_packet(info);
        //on successful rx set resp flag high
        idle_count = 0;
        resp_flag = 1;
        heard_since_last = 1;


//        WriteUART0("heard from someone resp flag set high\r\n");
    }

//    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
}

//void RX_setup_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
//{
//    if (e & RF_EventRxEntryDone)
//    {
//        currentDataEntry = RFQueue_getDataEntry();
//
//        /* Handle the packet data, located at &currentDataEntry->data:
//         * - Length is the first byte with the current configuration
//         * - Data starts from the second byte */
//        packetLength      = *(uint8_t *)(&(currentDataEntry->data));
//        packetDataPointer = (uint8_t *)(&(currentDataEntry->data) + 1);
//        RFQueue_nextEntry();
//        uint16_t info = (*(packetDataPointer + 2) << 8) | *(packetDataPointer + 3);
//
//
//
//
//    }
//
//}
