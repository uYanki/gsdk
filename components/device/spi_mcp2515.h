#ifndef __SPI_MCP2515_H__
#define __SPI_MCP2515_H__

#include "spimst.h"
#include "candefs.h"

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

typedef enum {
    MCP2515_20MHZ,
    MCP2515_16MHZ,
    MCP2515_8MHZ
} mcp2515_clkin_e;

typedef enum {
    CLKOUT_DISABLE = -1,
    CLKOUT_DIV1    = 0x0,
    CLKOUT_DIV2    = 0x1,
    CLKOUT_DIV4    = 0x2,
    CLKOUT_DIV8    = 0x3,
} mcp2515_clkout_e;

typedef enum {
    MCP2515_ERROR_OK        = 0,
    MCP2515_ERROR_FAIL      = 1,
    MCP2515_ERROR_ALLTXBUSY = 2,
    MCP2515_ERROR_FAILINIT  = 3,
    MCP2515_ERROR_FAILTX    = 4,
    MCP2515_ERROR_NOMSG     = 5
} mcp2515_error_e;

/**
 * @brief request operation mode @ CANCTRL bit[7,5]
 */
typedef enum {
    MCP2515_MODE_NORMAL     = 0x00,
    MCP2515_MODE_SLEEP      = 0x20,
    MCP2515_MODE_LOOPBACK   = 0x40,
    MCP2515_MODE_LISTENONLY = 0x60,
    MCP2515_MODE_CONFIG     = 0x80,
    MCP2515_MODE_POWERUP    = 0xE0
} mcp2515_mode_e;

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

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MCP2515_Init(spi_mcp2515_t* pHandle, const can_bps_e eBitrate, mcp2515_clkin_e eClock);

mcp2515_error_e MCP2515_Reset(spi_mcp2515_t* pHandle);
mcp2515_error_e MCP2515_SetMode(spi_mcp2515_t* pHandle, const mcp2515_mode_e mode);
mcp2515_error_e MCP2515_SetBitrate(spi_mcp2515_t* pHandle, const can_bps_e canSpeed, const mcp2515_clkin_e canClock);

mcp2515_error_e MCP2515_SetClkOut(spi_mcp2515_t* pHandle, const mcp2515_clkout_e divisor);

mcp2515_error_e MCP2515_SetFilterMask(spi_mcp2515_t* pHandle, const MASK num, const bool ext, const uint32_t ulData);
mcp2515_error_e MCP2515_SetFilter(spi_mcp2515_t* pHandle, const RXF num, const bool ext, const uint32_t ulData);

mcp2515_error_e MCP2515_SendMessage(spi_mcp2515_t* pHandle, const can_frame_t* pFrame);
mcp2515_error_e MCP2515_ReadMessage(spi_mcp2515_t* pHandle, can_frame_t* pFrame);

bool MCP2515_CheckReceive(spi_mcp2515_t* pHandle);
bool MCP2515_CheckError(spi_mcp2515_t* pHandle);

uint8_t MCP2515_GetErrorFlags(spi_mcp2515_t* pHandle);
void    MCP2515_ClearRXnOVRFlags(spi_mcp2515_t* pHandle);

uint8_t MCP2515_GetInterrupts(spi_mcp2515_t* pHandle);
uint8_t MCP2515_GetInterruptMask(spi_mcp2515_t* pHandle);
void    MCP2515_ClearInterrupts(spi_mcp2515_t* pHandle);
void    MCP2515_ClearTXInterrupts(spi_mcp2515_t* pHandle);

uint8_t MCP2515_GetStatus(spi_mcp2515_t* pHandle);
void    MCP2515_ClearRXnOVR(spi_mcp2515_t* pHandle);
void    MCP2515_ClearMERR(spi_mcp2515_t* pHandle);
void    MCP2515_ClearERRIF(spi_mcp2515_t* pHandle);

uint8_t MCP2515_ErrorCountRX(spi_mcp2515_t* pHandle);
uint8_t MCP2515_ErrorCountTX(spi_mcp2515_t* pHandle);

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
