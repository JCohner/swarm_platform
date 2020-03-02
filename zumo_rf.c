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
