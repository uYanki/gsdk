#ifndef __TFORMAT_H__
#define __TFORMAT_H__

/**
 * @brief TAMAGAWA
 *
 */

#include "gsdk.h"
#include "iomux.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef enum {
    TFORMAT_CF_DID_NONE = 0u,
    TFORMAT_CF_DID0     = ((0X00 << 3U) | 2U), /*Data readout: ABS */
    TFORMAT_CF_DID1     = ((0X11 << 3U) | 2U), /*Data readout: ABM */
    TFORMAT_CF_DID2     = ((0X12 << 3U) | 2U), /*Data readout: ENID */
    TFORMAT_CF_DID3     = ((0X03 << 3U) | 2U), /*Data readout: ABS, ENID, ABM, ALMC */
    TFORMAT_CF_DID6     = ((0X06 << 3U) | 2U), /* Writing to EEPROM*/
    TFORMAT_CF_DIDD     = ((0X1D << 3U) | 2U), /* Readout from EEPROM */
    // TFORMAT_CF_DID7  = ((0X17 << 3U ) | 2U), /* Reset */
    // TFORMAT_CF_DID8  = ((0X18 << 3U ) | 2U), /* Reset */
    // TFORMAT_CF_DIDC  = ((0X0C << 3U ) | 2U), /* Reset */
} tformat_cf_did_e;

typedef struct {
    USART_HandleType* huart;

    uint16_t mt_len; /* Multi-Turn bit length */
    uint16_t st_len; /* Single-Turn bit length */
    uint32_t mt;     /* Multi-Turn value */
    uint32_t st;     /* Single-Turn value */

    uint8_t u8TxSize;      /* TX data length */
    uint8_t u8RxSize;      /* RX data length */
    uint8_t au8TxData[4];  /* TX data buffer */
    uint8_t au8RxData[11]; /* RX data buffer */

    tformat_cf_did_e cf; /* Control Field of the transfer */

    uint8_t adf;    /* Address Feild of accessing EEPROM */
    uint8_t edf_wr; /* EEPROM Feild of writing EEPROM */
    uint8_t edf_rd; /* EEPROM Feild of reading EEPROM */

    uint8_t status; /* Status Feild */
    uint8_t id;     /* Encoder ID */
    uint8_t almc;   /* Encoder Error */

    bool cf_match;  /* TX CF and RX CF match in a transfer */
    bool adf_match; /* TX ADF and RX ADF match for a writing/reading EEPROM transfer */
    bool edf_match; /* TX EDF and RX EDF match for a writing EEPROM transfer */
    bool abs_busy;  /* The encoder is busy. */
    bool crc_match; /* RX CRC match*/

} tformat_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool Tformat_Init(tformat_t* pTformat);
void TFORMAT_Transfer(tformat_t* pTformat);
bool TFORMAT_Receive(tformat_t* pTformat);

void TFORMAT_SetupCmd(tformat_t* pTformat, tformat_cf_did_e cf);
void TFORMAT_ProcData(tformat_t* pTformat);

#ifdef __cplusplus
}
#endif

#endif
