#ifndef __SPI_MCP2515_H__
#define __SPI_MCP2515_H__

#include "spimst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define MCP2515_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPOL_LOW | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW)

typedef struct {
    __IN spi_mst_t* hSPI;
} spi_mcp2515_t;

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000UL /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000UL /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000UL /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFUL /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFUL /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFUL /* omit EFF, RTR, ERR flags */

/*
 * Controller Area Network Identifier structure
 *
 * bit 0-28 : CAN identifier (11/29 bit)
 * bit 29   : error message frame flag (0 = data frame, 1 = error message)
 * bit 30   : remote transmission request flag (1 = rtr frame)
 * bit 31   : frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
typedef uint32_t canid_t;

#define CAN_SFF_ID_BITS 11
#define CAN_EFF_ID_BITS 29

/* CAN payload length and DLC definitions according to ISO 11898-1 */
#define CAN_MAX_DLC  8
#define CAN_MAX_DLEN 8

struct can_frame {
    canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    uint8_t can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
    uint8_t data[CAN_MAX_DLEN] __attribute__((aligned(8)));
};

typedef enum {
    MCP2515_20MHZ,
    MCP2515_16MHZ,
    MCP2515_8MHZ
} mcp2515_clock_e;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

typedef enum {
    CAN_5KBPS,
    CAN_10KBPS,
    CAN_20KBPS,
    CAN_31K25BPS,
    CAN_33KBPS,
    CAN_40KBPS,
    CAN_50KBPS,
    CAN_80KBPS,
    CAN_83K3BPS,
    CAN_95KBPS,
    CAN_100KBPS,
    CAN_125KBPS,
    CAN_200KBPS,
    CAN_250KBPS,
    CAN_500KBPS,
    CAN_1000KBPS
} can_bps_e;

typedef enum {
    CLKOUT_DISABLE = -1,
    CLKOUT_DIV1    = 0x0,
    CLKOUT_DIV2    = 0x1,
    CLKOUT_DIV4    = 0x2,
    CLKOUT_DIV8    = 0x3,
} CAN_CLKOUT;

typedef enum {
    MCP2515_ERROR_OK        = 0,
    MCP2515_ERROR_FAIL      = 1,
    MCP2515_ERROR_ALLTXBUSY = 2,
    MCP2515_ERROR_FAILINIT  = 3,
    MCP2515_ERROR_FAILTX    = 4,
    MCP2515_ERROR_NOMSG     = 5
} mcp2515_error_e;

typedef enum {
    MASK0,
    MASK1
} MASK;

typedef enum {
    RXF0 = 0,
    RXF1 = 1,
    RXF2 = 2,
    RXF3 = 3,
    RXF4 = 4,
    RXF5 = 5
} RXF;

typedef enum {
    RXB0 = 0,
    RXB1 = 1
} RXBn;

typedef enum {
    TXB0 = 0,
    TXB1 = 1,
    TXB2 = 2
} TXBn;

typedef enum {
    CANINTF_RX0IF = 0x01,
    CANINTF_RX1IF = 0x02,
    CANINTF_TX0IF = 0x04,
    CANINTF_TX1IF = 0x08,
    CANINTF_TX2IF = 0x10,
    CANINTF_ERRIF = 0x20,
    CANINTF_WAKIF = 0x40,
    CANINTF_MERRF = 0x80
} CANINTF;

typedef enum {
    EFLG_RX1OVR = (1 << 7),
    EFLG_RX0OVR = (1 << 6),
    EFLG_TXBO   = (1 << 5),
    EFLG_TXEP   = (1 << 4),
    EFLG_RXEP   = (1 << 3),
    EFLG_TXWAR  = (1 << 2),
    EFLG_RXWAR  = (1 << 1),
    EFLG_EWARN  = (1 << 0)
} EFLG;

typedef enum {
    CANCTRL_REQOP_NORMAL     = 0x00,
    CANCTRL_REQOP_SLEEP      = 0x20,
    CANCTRL_REQOP_LOOPBACK   = 0x40,
    CANCTRL_REQOP_LISTENONLY = 0x60,
    CANCTRL_REQOP_CONFIG     = 0x80,
    CANCTRL_REQOP_POWERUP    = 0xE0
} CANCTRL_REQOP_MODE;

typedef enum {
    STAT_RX0IF = (1 << 0),
    STAT_RX1IF = (1 << 1)
} STAT;

typedef enum {
    TXB_ABTF  = 0x40,
    TXB_MLOA  = 0x20,
    TXB_TXERR = 0x10,
    TXB_TXREQ = 0x08,
    TXB_TXIE  = 0x04,
    TXB_TXP   = 0x03
} TXBnCTRL;

typedef uint8_t REGISTER;

struct TXBn_REGS {
    REGISTER CTRL;
    REGISTER SIDH;
    REGISTER DATA;
};

struct RXBn_REGS {
    REGISTER CTRL;
    REGISTER SIDH;
    REGISTER DATA;
    CANINTF  CANINTF_RXnIF;
};

void MCP2515_Init(spi_mcp2515_t* pHandle);

mcp2515_error_e MCP2515_SetMode(spi_mcp2515_t* pHandle, const CANCTRL_REQOP_MODE mode);
uint8_t         MCP2515_ReadRegister(spi_mcp2515_t* pHandle, const REGISTER reg);
void            MCP2515_ReadRegisters(spi_mcp2515_t* pHandle, const REGISTER reg, uint8_t values[], const uint8_t n);
void            MCP2515_SetRegister(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t value);
void            MCP2515_SetRegisters(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t values[], const uint8_t n);
void            MCP2515_ModifyRegister(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t mask, const uint8_t data);
void            MCP2515_PrepareId(spi_mcp2515_t* pHandle, uint8_t* buffer, const bool ext, const uint32_t id);

mcp2515_error_e MCP2515_Reset(spi_mcp2515_t* pHandle);
mcp2515_error_e MCP2515_SetConfigMode(spi_mcp2515_t* pHandle);
mcp2515_error_e MCP2515_SetListenOnlyMode(spi_mcp2515_t* pHandle);
mcp2515_error_e MCP2515_SetSleepMode(spi_mcp2515_t* pHandle);
mcp2515_error_e MCP2515_SetLoopbackMode(spi_mcp2515_t* pHandle);
mcp2515_error_e MCP2515_SetNormalMode(spi_mcp2515_t* pHandle);
mcp2515_error_e MCP2515_SetClkOut(spi_mcp2515_t* pHandle, const CAN_CLKOUT divisor);
mcp2515_error_e MCP2515_SetBitrate(spi_mcp2515_t* pHandle, const can_bps_e canSpeed, const mcp2515_clock_e canClock);
mcp2515_error_e MCP2515_SetFilterMask(spi_mcp2515_t* pHandle, const MASK num, const bool ext, const uint32_t ulData);
mcp2515_error_e MCP2515_SetFilter(spi_mcp2515_t* pHandle, const RXF num, const bool ext, const uint32_t ulData);
mcp2515_error_e MCP2515_SendMessage(spi_mcp2515_t* pHandle, const TXBn txbn, const struct can_frame* frame);
mcp2515_error_e MCP2515_SendMessageEx(spi_mcp2515_t* pHandle, const struct can_frame* frame);
mcp2515_error_e MCP2515_ReadMessage(spi_mcp2515_t* pHandle, const RXBn rxbn, struct can_frame* frame);
mcp2515_error_e MCP2515_ReadMessageEx(spi_mcp2515_t* pHandle, struct can_frame* frame);
bool            MCP2515_CheckReceive(spi_mcp2515_t* pHandle);
bool            MCP2515_CheckError(spi_mcp2515_t* pHandle);
uint8_t         MCP2515_GetErrorFlags(spi_mcp2515_t* pHandle);
void            MCP2515_ClearRXnOVRFlags(spi_mcp2515_t* pHandle);
uint8_t         MCP2515_GetInterrupts(spi_mcp2515_t* pHandle);
uint8_t         MCP2515_GetInterruptMask(spi_mcp2515_t* pHandle);
void            MCP2515_ClearInterrupts(spi_mcp2515_t* pHandle);
void            MCP2515_ClearTXInterrupts(spi_mcp2515_t* pHandle);
uint8_t         MCP2515_GetStatus(spi_mcp2515_t* pHandle);
void            MCP2515_ClearRXnOVR(spi_mcp2515_t* pHandle);
void            MCP2515_ClearMERR(spi_mcp2515_t* pHandle);
void            MCP2515_ClearERRIF(spi_mcp2515_t* pHandle);
// uint8_t   MCP2515_ErrorCountRX(spi_mcp2515_t* pHandle);
// uint8_t   MCP2515_ErrorCountTX(spi_mcp2515_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void MCP2515_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
