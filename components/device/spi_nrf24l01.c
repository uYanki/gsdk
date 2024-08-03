#include "spi_nrf24l01.h"
#include "hexdump.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG         "nrf24l01"
#define LOG_LOCAL_LEVEL       LOG_LEVEL_VERBOSE

#define RF24_MAX_PAYLOAD_SIZE 32
#define RF24_MAX_CHANNEL      127

/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD       0x1C
#define FEATURE     0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5      5
#define DPL_P4      4
#define DPL_P3      3
#define DPL_P2      2
#define DPL_P1      1
#define DPL_P0      0
#define EN_DPL      2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

/* Non-P omissions */
#define LNA_HCURR 0

/* P model memory Map */
#define RPD 0x09

/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static inline uint8_t NRF24L01_ReadByte(spi_nrf24l01_t* pHandle, uint8_t u8MemAddr);
static inline uint8_t NRF24L01_ReadBlock(spi_nrf24l01_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data, uint8_t u8Size);
static inline uint8_t NRF24L01_WriteByte(spi_nrf24l01_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data);
static inline uint8_t NRF24L01_WriteBlock(spi_nrf24l01_t* pHandle, uint8_t u8MemAddr, const uint8_t* cpu8Data, uint8_t u8Size);

/**
 * @brief Write the transmit payload
 * @note The size of data written is the fixed payload size, see NRF24L01_GetPayloadSize()
 * @param cpu8Data Where to get the data
 * @param u8Len Number of bytes to be sent
 * @return Current value of status register
 */
static inline uint8_t NRF24L01_WritePayload(spi_nrf24l01_t* pHandle, const uint8_t* cpu8Data, uint8_t u8Len);

/**
 * @brief Read the receive payload
 * @note The size of data read is the fixed payload size, see NRF24L01_GetPayloadSize()
 * @param pu8Data Where to put the data
 * @param u8Len Maximum number of bytes to read
 * @return Current value of status register
 */
static inline uint8_t NRF24L01_ReadPayload(spi_nrf24l01_t* pHandle, uint8_t* pu8Data, uint8_t u8Len);

/**
 * @brief Empty the receive buffer
 * @return Current value of status register
 */
static inline uint8_t NRF24L01_FlushRx(spi_nrf24l01_t* pHandle);

/**
 * @brief Empty the transmit buffer
 * @return Current value of status register
 */
static inline uint8_t NRF24L01_FlushTx(spi_nrf24l01_t* pHandle);

/**
 * @brief Retrieve the current status of the chip
 * @return Current value of status register
 */
static inline uint8_t NRF24L01_GetStatus(spi_nrf24l01_t* pHandle);

/**
 * Turn on or off the special features of the chip
 *
 * The chip has certain 'features' which are only available when the 'features'
 * are enabled.  See the datasheet for details.
 */
static inline void NRF24L01_ToggleFeatures(spi_nrf24l01_t* pHandle);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static const uint8_t m_cau8ChildPipe[]        = {RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5};
static const uint8_t m_cau8ChildPayloadSize[] = {RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5};
static const uint8_t m_cau8ChildPipeEnable[]  = {ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline uint8_t NRF24L01_ReadBlock(spi_nrf24l01_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data, uint8_t u8Size)
{
    uint8_t u8Status;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitReceiveByte(pHandle->hSPI, R_REGISTER | (REGISTER_MASK & u8MemAddr), &u8Status);
    SPI_Master_ReceiveBlock(pHandle->hSPI, pu8Data, u8Size);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Status;
}

static inline uint8_t NRF24L01_ReadByte(spi_nrf24l01_t* pHandle, uint8_t u8MemAddr)
{
    uint8_t u8Data;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, R_REGISTER | (REGISTER_MASK & u8MemAddr));
    SPI_Master_ReceiveByte(pHandle->hSPI, &u8Data);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Data;
}

static inline uint8_t NRF24L01_WriteBlock(spi_nrf24l01_t* pHandle, uint8_t u8MemAddr, const uint8_t* cpu8Data, uint8_t u8Size)
{
    uint8_t u8Status;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitReceiveByte(pHandle->hSPI, W_REGISTER | (REGISTER_MASK & u8MemAddr), &u8Status);
    SPI_Master_TransmitBlock(pHandle->hSPI, cpu8Data, u8Size);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Status;
}

static inline uint8_t NRF24L01_WriteByte(spi_nrf24l01_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data)
{
    uint8_t u8Status;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitReceiveByte(pHandle->hSPI, W_REGISTER | (REGISTER_MASK & u8MemAddr), &u8Status);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Data);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Status;
}

static inline uint8_t NRF24L01_WritePayload(spi_nrf24l01_t* pHandle, const uint8_t* cpu8Data, uint8_t u8Len)
{
    uint8_t u8Status;

    uint8_t u8DataLen  = MIN(u8Len, pHandle->u8PayloadSize);
    uint8_t u8BlankLen = pHandle->bDynamicPayloadsEnabled ? 0 : pHandle->u8PayloadSize - u8DataLen;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitReceiveByte(pHandle->hSPI, W_TX_PAYLOAD, &u8Status);
    SPI_Master_TransmitBlock(pHandle->hSPI, cpu8Data, u8DataLen);

    while (u8BlankLen--)
    {
        SPI_Master_TransmitByte(pHandle->hSPI, 0x00);
    }

    SPI_Master_Deselect(pHandle->hSPI);

    return u8Status;
}

static inline uint8_t NRF24L01_ReadPayload(spi_nrf24l01_t* pHandle, uint8_t* pu8Data, uint8_t u8Len)
{
    uint8_t u8Status;

    uint8_t u8DataLen  = MIN(u8Len, pHandle->u8PayloadSize);
    uint8_t u8BlankLen = pHandle->bDynamicPayloadsEnabled ? 0 : pHandle->u8PayloadSize - u8DataLen;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitReceiveByte(pHandle->hSPI, R_RX_PAYLOAD, &u8Status);
    SPI_Master_ReceiveBlock(pHandle->hSPI, pu8Data, u8DataLen);

    while (u8BlankLen--)
    {
        SPI_Master_TransmitByte(pHandle->hSPI, 0xFF);
    }

    SPI_Master_Deselect(pHandle->hSPI);

    return u8Status;
}

static inline uint8_t NRF24L01_FlushRx(spi_nrf24l01_t* pHandle)
{
    uint8_t u8Status;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitReceiveByte(pHandle->hSPI, FLUSH_RX, &u8Status);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Status;
}

static inline uint8_t NRF24L01_FlushTx(spi_nrf24l01_t* pHandle)
{
    uint8_t u8Status;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitReceiveByte(pHandle->hSPI, FLUSH_TX, &u8Status);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Status;
}

static inline uint8_t NRF24L01_GetStatus(spi_nrf24l01_t* pHandle)
{
    uint8_t u8Status;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitReceiveByte(pHandle->hSPI, NOP, &u8Status);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Status;
}

static inline void NRF24L01_ToggleFeatures(spi_nrf24l01_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, ACTIVATE);
    SPI_Master_TransmitByte(pHandle->hSPI, 0x73);
    SPI_Master_Deselect(pHandle->hSPI);
}

void NRF24L01_PrintDetails(spi_nrf24l01_t* pHandle)
{
    uint8_t u8Status = NRF24L01_GetStatus(pHandle);

    PRINTLN("STATUS        : 0x%02X", u8Status);

    PRINTLN("STATUS_BIT    : RX_DR=%X TX_DS=%X MAX_RT=%X RX_P_NO=%X TX_FULL=%X",
            (u8Status & BV(RX_DR)) ? 1 : 0,
            (u8Status & BV(TX_DS)) ? 1 : 0,
            (u8Status & BV(MAX_RT)) ? 1 : 0,
            ((u8Status >> RX_P_NO) & 0b111),
            (u8Status & BV(TX_FULL)) ? 1 : 0);

    //

    uint8_t au8Data[32];

    struct {
        uint8_t     u8Type;  // 0: byte register, 1: address register
        const char* szName;
        uint8_t     u8MemAddr;
        uint8_t     u8MemSize;
    } au8RegMap[] = {
        {1, "TX_ADDR       : ", TX_ADDR,    1},
        {1, "RX_ADDR_P0-1  : ", RX_ADDR_P0, 2},
        {0, "RX_ADDR_P2-5  : ", RX_ADDR_P2, 4},
        {0, "RX_PW_P0-6    : ", RX_PW_P0,   6},
        {0, "EN_AA         : ", EN_AA,      1},
        {0, "EN_RXADDR     : ", EN_RXADDR,  1},
        {0, "RF_CH         : ", RF_CH,      1},
        {0, "RF_SETUP      : ", RF_SETUP,   1},
        {0, "CONFIG        : ", CONFIG,     1},
        {0, "DYNPD/FEATURE : ", DYNPD,      2},
    };

    for (uint8_t i = 0; i < ARRAY_SIZE(au8RegMap); i++)
    {
        if (au8RegMap[i].u8Type == 0)
        {
            for (uint8_t j = 0; j < au8RegMap[i].u8MemSize; j++)
            {
                au8Data[j] = NRF24L01_ReadByte(pHandle, au8RegMap[i].u8MemAddr + j);
            }

            hexdump(au8Data, au8RegMap[i].u8MemSize, 16, 1, false, au8RegMap[i].szName, 0);
        }
        else
        {
#define MEM_SIZE 5

            for (uint8_t j = 0; j < au8RegMap[i].u8MemSize; j++)
            {
                NRF24L01_ReadBlock(pHandle, au8RegMap[i].u8MemAddr + j, au8Data + MEM_SIZE * j, MEM_SIZE);
            }

            hexdump(au8Data, au8RegMap[i].u8MemSize * MEM_SIZE, 16, 1, false, au8RegMap[i].szName, 0);

#undef MEM_SIZE
        }
    }

    //

    static const char* const rf24_datarate_e_str[] = {
        "1MBPS",
        "2MBPS",
        "250KBPS",
    };
    static const char* const rf24_model_e_str[] = {
        "nRF24L01",
        "nRF24L01+",
    };

    static const char* const rf24_crclength_e_str[] = {
        "Disabled",
        "8 bits",
        "16 bits",
    };

    static const char* const rf24_pa_dbm_e_str[] = {
        "PA_MIN",
        "PA_LOW",
        "LA_MED",
        "PA_HIGH",
    };

    PRINTLN("Data Rate     : %s", rf24_datarate_e_str[NRF24L01_GetDataRate(pHandle)]);
    PRINTLN("Model         : %s", rf24_model_e_str[NRF24L01_IsPVariant(pHandle)]);
    PRINTLN("CRC Length    : %s", rf24_crclength_e_str[NRF24L01_GetCRCLength(pHandle)]);
    PRINTLN("PA Power      : %s", rf24_pa_dbm_e_str[NRF24L01_GetPALevel(pHandle)]);
}

void NRF24L01_SetChannel(spi_nrf24l01_t* pHandle, uint8_t u8Channel)
{
    // TODO: This method could take advantage of the 'bWideBand' calculation
    // done in NRF24L01_SetChannel() to require certain channel spacing.
    NRF24L01_WriteByte(pHandle, RF_CH, MIN(u8Channel, RF24_MAX_CHANNEL));
}

void NRF24L01_SetPayloadSize(spi_nrf24l01_t* pHandle, uint8_t u8Size)
{
    pHandle->u8PayloadSize = MIN(u8Size, RF24_MAX_PAYLOAD_SIZE);
}

uint8_t NRF24L01_GetPayloadSize(spi_nrf24l01_t* pHandle)
{
    return pHandle->u8PayloadSize;
}

void NRF24L01_Init(spi_nrf24l01_t* pHandle)
{
    pHandle->bWideBand               = true;
    pHandle->bPlusVer                = false;
    pHandle->u8PayloadSize           = 32;
    pHandle->bAckPayloadAvailable    = false;
    pHandle->bDynamicPayloadsEnabled = false;
    pHandle->u8AckPayloadLength      = 0;
    pHandle->u64Pipe0ReadingAddress  = 0;

    PIN_SetMode(&pHandle->CE, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
    PIN_WriteLevel(&pHandle->CE, PIN_LEVEL_LOW);

    // Must allow the radio time to settle else configuration bits will not necessarily stick.
    // This is actually only required following power up but some settling time also appears to
    // be required after resets too. For full coverage, we'll always assume the worst.
    // Enabling 16b CRC is by far the most obvious case if the wrong timing is used - or skipped.
    // Technically we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good measure.
    // WARNING: Delay is based on P-variant whereby non-P *may* require different timing.
    DelayBlockMs(5);

    // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
    // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
    // sizes must never be used. See documentation for a more complete explanation.
    NRF24L01_WriteByte(pHandle, SETUP_RETR, (0b0100 << ARD) | (0b1111 << ARC));

    // Restore our default PA level
    NRF24L01_SetPALevel(pHandle, RF24_PA_MAX);

    // Determine if this is a p or non-p RF24 module and then
    // reset our data rate back to default value. This works
    // because a non-P variant won't allow the data rate to
    // be set to 250Kbps.
    if (NRF24L01_SetDataRate(pHandle, RF24_250KBPS))
    {
        pHandle->bPlusVer = true;
    }

    // Then set the data rate to the slowest (and most reliable) speed supported by all
    // hardware.
    NRF24L01_SetDataRate(pHandle, RF24_1MBPS);

    // Initialize CRC and request 2-byte (16bit) CRC
    NRF24L01_SetCRCLength(pHandle, RF24_CRC_16);

    // Disable dynamic payloads, to match bDynamicPayloadsEnabled setting
    NRF24L01_WriteByte(pHandle, DYNPD, 0);

    // Reset current status
    // Notice reset and flush is the last thing we do
    NRF24L01_WriteByte(pHandle, STATUS, BV(RX_DR) | BV(TX_DS) | BV(MAX_RT));

    // Set up default configuration.  Callers can always change it later.
    // This channel should be universally safe and not bleed over into adjacent
    // spectrum.
    NRF24L01_SetChannel(pHandle, 76);

    // Flush buffers
    NRF24L01_FlushRx(pHandle);
    NRF24L01_FlushTx(pHandle);
}

void NRF24L01_StartListening(spi_nrf24l01_t* pHandle)
{
    NRF24L01_WriteByte(pHandle, CONFIG, NRF24L01_ReadByte(pHandle, CONFIG) | BV(PWR_UP) | BV(PRIM_RX));
    NRF24L01_WriteByte(pHandle, STATUS, BV(RX_DR) | BV(TX_DS) | BV(MAX_RT));

    // Restore the pipe0 adddress, if exists
    if (pHandle->u64Pipe0ReadingAddress != 0)
    {
        NRF24L01_WriteBlock(pHandle, RX_ADDR_P0, (const uint8_t*)&pHandle->u64Pipe0ReadingAddress, 5);
    }

    // Flush buffers
    NRF24L01_FlushRx(pHandle);
    NRF24L01_FlushTx(pHandle);

    // Go!
    PIN_WriteLevel(&pHandle->CE, PIN_LEVEL_HIGH);

    // wait for the radio to come up (130us actually only needed)
    DelayBlockMs(130);
}

void NRF24L01_StopListening(spi_nrf24l01_t* pHandle)
{
    PIN_WriteLevel(&pHandle->CE, PIN_LEVEL_LOW);
    NRF24L01_FlushTx(pHandle);
    NRF24L01_FlushRx(pHandle);
}

void NRF24L01_PowerDown(spi_nrf24l01_t* pHandle)
{
    NRF24L01_WriteByte(pHandle, CONFIG, NRF24L01_ReadByte(pHandle, CONFIG) & ~BV(PWR_UP));
}

void NRF24L01_PowerUp(spi_nrf24l01_t* pHandle)
{
    NRF24L01_WriteByte(pHandle, CONFIG, NRF24L01_ReadByte(pHandle, CONFIG) | BV(PWR_UP));
}

bool NRF24L01_WriteData(spi_nrf24l01_t* pHandle, const uint8_t* cpu8Data, uint8_t u8Len)
{
    // Begin the write
    NRF24L01_StartWrite(pHandle, cpu8Data, u8Len);

    // ------------
    // At this point we could return from a non-blocking write, and then call
    // the rest after an interrupt

    // Instead, we are going to block here until we get TX_DS (transmission completed and ack'd)
    // or MAX_RT (maximum retries, transmission failed).  Also, we'll timeout in case the radio
    // is flaky and we get neither.

    // IN the end, the send should be blocking.  It comes back in 60ms worst case, or much faster
    // if I tighted up the retry logic.  (Default settings will be 1500us.

    // Monitor the send

    uint8_t u8ObserveTx;
    uint8_t u8Status;
    tick_t  TickStart = GetTickUs();

    do
    {
        u8Status = NRF24L01_ReadBlock(pHandle, OBSERVE_TX, &u8ObserveTx, 1);

        if (DelayNonBlockMs(TickStart, 500))
        {
            break;
        }

    } while (!(u8Status & (BV(TX_DS) | BV(MAX_RT))));

    // The part above is what you could recreate with your own interrupt handler,
    // and then call this when you got an interrupt
    // ------------

    // Call this when you get an interrupt
    // The status tells us three things
    // * The send was successful (TX_DS)
    // * The send failed, too many retries (MAX_RT)
    // * There is an ack packet waiting (RX_DR)
    bool bTxOk = false, bTxFail = true;
    NRF24L01_WhatHappened(pHandle, &bTxOk, &bTxFail, &pHandle->bAckPayloadAvailable);

    // Handle the ack packet
    if (pHandle->bAckPayloadAvailable)
    {
        pHandle->u8AckPayloadLength = NRF24L01_GetDynamicPayloadSize(pHandle);
        LOGV("[AckPacket] %d", pHandle->u8AckPayloadLength);
    }

    // Power down
    NRF24L01_PowerDown(pHandle);

    // Flush buffers (Is this a relic of past experimentation, and not needed anymore??)
    NRF24L01_FlushTx(pHandle);

    return bTxOk;
}

void NRF24L01_StartWrite(spi_nrf24l01_t* pHandle, const uint8_t* cpu8Data, uint8_t u8Len)
{
    // Transmitter power-up
    NRF24L01_WriteByte(pHandle, CONFIG, (NRF24L01_ReadByte(pHandle, CONFIG) | BV(PWR_UP)) & ~BV(PRIM_RX));
    DelayBlockMs(150);

    // Send the payload
    NRF24L01_WritePayload(pHandle, cpu8Data, u8Len);

    // Allons!
    PIN_WriteLevel(&pHandle->CE, PIN_LEVEL_HIGH);
    DelayBlockMs(15);
    PIN_WriteLevel(&pHandle->CE, PIN_LEVEL_LOW);
}

uint8_t NRF24L01_GetDynamicPayloadSize(spi_nrf24l01_t* pHandle)
{
    uint8_t u8Data;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, R_RX_PL_WID);
    SPI_Master_ReceiveByte(pHandle->hSPI, &u8Data);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Data;
}

bool NRF24L01_IsAvailable(spi_nrf24l01_t* pHandle)
{
    return NRF24L01_Available(pHandle, nullptr);
}

bool NRF24L01_Available(spi_nrf24l01_t* pHandle, uint8_t* pu8Pipe)
{
    uint8_t u8Status = NRF24L01_GetStatus(pHandle);

    bool bRxReady = (u8Status & BV(RX_DR)) ? true : false;

    if (bRxReady)
    {
        // If the caller wants the pipe number, include that
        if (pu8Pipe != nullptr)
        {
            *pu8Pipe = (u8Status >> RX_P_NO) & 0b111;
        }

        // Clear the status bit

        // ??? Should this REALLY be cleared now?  Or wait until we
        // actually READ the payload?

        NRF24L01_WriteByte(pHandle, STATUS, BV(RX_DR));

        // Handle ack payload receipt
        if (u8Status & BV(TX_DS))
        {
            NRF24L01_WriteByte(pHandle, STATUS, BV(TX_DS));
        }
    }

    return bRxReady;
}

bool NRF24L01_ReadData(spi_nrf24l01_t* pHandle, uint8_t* pu8Data, uint8_t u8Len)
{
    // Fetch the payload
    NRF24L01_ReadPayload(pHandle, pu8Data, u8Len);

    // was this the last of the data available?
    return NRF24L01_ReadByte(pHandle, FIFO_STATUS) & BV(RX_EMPTY);
}

void NRF24L01_WhatHappened(spi_nrf24l01_t* pHandle, bool* pbTxOk, bool* pbTxFali, bool* pbRxRdy)
{
    // Read the status & reset the status in one easy call
    // Or is that such a good idea?
    uint8_t u8Status = NRF24L01_WriteByte(pHandle, STATUS, BV(RX_DR) | BV(TX_DS) | BV(MAX_RT));

    // Report to the user what happened
    *pbTxOk   = (u8Status & BV(TX_DS)) ? true : false;
    *pbTxFali = (u8Status & BV(MAX_RT)) ? true : false;
    *pbRxRdy  = (u8Status & BV(RX_DR)) ? true : false;
}

void NRF24L01_OpenWritingPipe(spi_nrf24l01_t* pHandle, uint64_t u64Address)
{
    // Note that AVR 8-bit uC's store this LSB first, and the NRF24L01(+)
    // expects it LSB first too, so we're good.

    NRF24L01_WriteBlock(pHandle, RX_ADDR_P0, (uint8_t*)&u64Address, 5);  // 大小端?
    NRF24L01_WriteBlock(pHandle, TX_ADDR, (uint8_t*)&u64Address, 5);

    NRF24L01_WriteByte(pHandle, RX_PW_P0, MIN(pHandle->u8PayloadSize, RF24_MAX_PAYLOAD_SIZE));
}

void NRF24L01_OpenReadingPipe(spi_nrf24l01_t* pHandle, uint8_t u8Pipe, uint64_t u64Address)
{
    // If this is pipe 0, cache the address. This is needed because
    // NRF24L01_OpenWritingPipe() will overwrite the pipe 0 address, so
    // NRF24L01_StartListening() will have to restore it.

    if (u8Pipe == 0)
    {
        pHandle->u64Pipe0ReadingAddress = u64Address;
    }

    if (u8Pipe <= 6)
    {
        // For pipes 2-5, only write the LSB
        NRF24L01_WriteBlock(pHandle, m_cau8ChildPipe[u8Pipe], (const uint8_t*)&u64Address, (u8Pipe < 2) ? 5 : 1);

        NRF24L01_WriteByte(pHandle, m_cau8ChildPayloadSize[u8Pipe], pHandle->u8PayloadSize);

        // Note it would be more efficient to set all of the bits for all open
        // pipes at once.  However, I thought it would make the calling code
        // more simple to do it this way.
        NRF24L01_WriteByte(pHandle, EN_RXADDR, NRF24L01_ReadByte(pHandle, EN_RXADDR) | BV(m_cau8ChildPipeEnable[u8Pipe]));
    }
}

void NRF24L01_EnableDynamicPayloads(spi_nrf24l01_t* pHandle)
{
    // Enable dynamic payload throughout the system
    NRF24L01_WriteByte(pHandle, FEATURE, NRF24L01_ReadByte(pHandle, FEATURE) | BV(EN_DPL));

    // If it didn't work, the features are not enabled
    if (!NRF24L01_ReadByte(pHandle, FEATURE))
    {
        // So enable them and try again
        NRF24L01_ToggleFeatures(pHandle);
        NRF24L01_WriteByte(pHandle, FEATURE, NRF24L01_ReadByte(pHandle, FEATURE) | BV(EN_DPL));
    }

    // Enable dynamic payload on all pipes
    //
    // Not sure the use case of only having dynamic payload on certain
    // pipes, so the library does not support it.
    NRF24L01_WriteByte(pHandle, DYNPD, NRF24L01_ReadByte(pHandle, DYNPD) | BV(DPL_P5) | BV(DPL_P4) | BV(DPL_P3) | BV(DPL_P2) | BV(DPL_P1) | BV(DPL_P0));

    pHandle->bDynamicPayloadsEnabled = true;
}

void NRF24L01_EnableAckPayload(spi_nrf24l01_t* pHandle)
{
    // enable ack payload and dynamic payload features
    NRF24L01_WriteByte(pHandle, FEATURE, NRF24L01_ReadByte(pHandle, FEATURE) | BV(EN_ACK_PAY) | BV(EN_DPL));

    // If it didn't work, the features are not enabled
    if (!NRF24L01_ReadByte(pHandle, FEATURE))
    {
        // So enable them and try again
        NRF24L01_ToggleFeatures(pHandle);
        NRF24L01_WriteByte(pHandle, FEATURE, NRF24L01_ReadByte(pHandle, FEATURE) | BV(EN_ACK_PAY) | BV(EN_DPL));
    }

    printf("FEATURE=%i", NRF24L01_ReadByte(pHandle, FEATURE));

    // Enable dynamic payload on pipes 0 & 1
    NRF24L01_WriteByte(pHandle, DYNPD, NRF24L01_ReadByte(pHandle, DYNPD) | BV(DPL_P1) | BV(DPL_P0));
}

void NRF24L01_WriteAckPayload(spi_nrf24l01_t* pHandle, uint8_t u8Pipe, const uint8_t* cpu8Data, uint8_t u8Len)
{
    uint8_t u8DataLen = MIN(u8Len, RF24_MAX_PAYLOAD_SIZE);

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, W_ACK_PAYLOAD | (u8Pipe & 0b111));
    SPI_Master_TransmitBlock(pHandle->hSPI, cpu8Data, u8DataLen);
    SPI_Master_Deselect(pHandle->hSPI);
}

bool NRF24L01_IsAckPayloadAvailable(spi_nrf24l01_t* pHandle)
{
    bool result                   = pHandle->bAckPayloadAvailable;
    pHandle->bAckPayloadAvailable = false;
    return result;
}

bool NRF24L01_IsPVariant(spi_nrf24l01_t* pHandle)
{
    return pHandle->bPlusVer;
}

void NRF24L01_SetAutoAck(spi_nrf24l01_t* pHandle, bool bEnable)
{
    NRF24L01_WriteByte(pHandle, EN_AA, bEnable ? 0b111111 : 0);
}

void NRF24L01_SetAutoAckEx(spi_nrf24l01_t* pHandle, uint8_t u8Pipe, bool bEnable)
{
    if (u8Pipe <= 6)
    {
        uint8_t u8Data = NRF24L01_ReadByte(pHandle, EN_AA);

        if (bEnable)
        {
            u8Data |= BV(u8Pipe);
        }
        else
        {
            u8Data &= ~BV(u8Pipe);
        }

        NRF24L01_WriteByte(pHandle, EN_AA, u8Data);
    }
}

bool NRF24L01_TestCarrier(spi_nrf24l01_t* pHandle)
{
    return NRF24L01_ReadByte(pHandle, CD) & 1;
}

bool NRF24L01_TestRPD(spi_nrf24l01_t* pHandle)
{
    return NRF24L01_ReadByte(pHandle, RPD) & 1;
}

void NRF24L01_SetPALevel(spi_nrf24l01_t* pHandle, rf24_pa_dbm_e eLevel)
{
    uint8_t u8Data = NRF24L01_ReadByte(pHandle, RF_SETUP);

    u8Data &= ~(BV(RF_PWR_LOW) | BV(RF_PWR_HIGH));

    switch (eLevel)
    {
        case RF24_PA_MAX:
        {
            u8Data |= (BV(RF_PWR_LOW) | BV(RF_PWR_HIGH));
            break;
        }

        case RF24_PA_HIGH:
        {
            u8Data |= BV(RF_PWR_HIGH);
            break;
        }

        case RF24_PA_LOW:
        {
            u8Data |= BV(RF_PWR_LOW);
            break;
        }

        case RF24_PA_MIN:
        {
            // nothing
            break;
        }

        case RF24_PA_ERROR:
        {
            // On error, go to maximum PA
            u8Data |= (BV(RF_PWR_LOW) | BV(RF_PWR_HIGH));
            break;
        }

        default:
        {
            return;
        }
    }

    NRF24L01_WriteByte(pHandle, RF_SETUP, u8Data);
}

rf24_pa_dbm_e NRF24L01_GetPALevel(spi_nrf24l01_t* pHandle)
{
    rf24_pa_dbm_e ePaDbm = RF24_PA_ERROR;
    uint8_t       u8Data = NRF24L01_ReadByte(pHandle, RF_SETUP) & (BV(RF_PWR_LOW) | BV(RF_PWR_HIGH));

    if (u8Data == (BV(RF_PWR_LOW) | BV(RF_PWR_HIGH)))
    {
        ePaDbm = RF24_PA_MAX;
    }
    else if (u8Data == BV(RF_PWR_HIGH))
    {
        ePaDbm = RF24_PA_HIGH;
    }
    else if (u8Data == BV(RF_PWR_LOW))
    {
        ePaDbm = RF24_PA_LOW;
    }
    else
    {
        ePaDbm = RF24_PA_MIN;
    }

    return ePaDbm;
}

bool NRF24L01_SetDataRate(spi_nrf24l01_t* pHandle, rf24_datarate_e eSpeed)
{
    uint8_t u8Data = NRF24L01_ReadByte(pHandle, RF_SETUP);

    // HIGH and LOW '00' is 1Mbs - our default
    pHandle->bWideBand = false;
    u8Data &= ~(BV(RF_DR_LOW) | BV(RF_DR_HIGH));

    if (eSpeed == RF24_250KBPS)
    {
        // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
        // Making it '10'.
        pHandle->bWideBand = false;
        u8Data |= BV(RF_DR_LOW);
    }
    else
    {
        // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
        // Making it '01'
        if (eSpeed == RF24_2MBPS)
        {
            pHandle->bWideBand = true;
            u8Data |= BV(RF_DR_HIGH);
        }
        else
        {
            // 1Mbs
            pHandle->bWideBand = false;
        }
    }

    NRF24L01_WriteByte(pHandle, RF_SETUP, u8Data);

    // Verify our result
    if (NRF24L01_ReadByte(pHandle, RF_SETUP) == u8Data)
    {
        return true;
    }
    else
    {
        pHandle->bWideBand = false;
    }

    return false;
}

rf24_datarate_e NRF24L01_GetDataRate(spi_nrf24l01_t* pHandle)
{
    rf24_datarate_e eDataRate;
    uint8_t         u8Data = NRF24L01_ReadByte(pHandle, RF_SETUP) & (BV(RF_DR_LOW) | BV(RF_DR_HIGH));

    // Order matters in our case below
    if (u8Data == BV(RF_DR_LOW))
    {
        // '10' = 250KBPS
        eDataRate = RF24_250KBPS;
    }
    else if (u8Data == BV(RF_DR_HIGH))
    {
        // '01' = 2MBPS
        eDataRate = RF24_2MBPS;
    }
    else
    {
        // '00' = 1MBPS
        eDataRate = RF24_1MBPS;
    }

    return eDataRate;
}

void NRF24L01_SetCRCLength(spi_nrf24l01_t* pHandle, rf24_crc_len_e eLength)
{
    uint8_t u8Data = NRF24L01_ReadByte(pHandle, CONFIG) & ~(BV(CRCO) | BV(EN_CRC));

    if (eLength == RF24_CRC_DISABLED)
    {
        // Do nothing, we turned it off above.
    }
    else if (eLength == RF24_CRC_8)
    {
        u8Data |= BV(EN_CRC);
    }
    else
    {
        u8Data |= BV(EN_CRC);
        u8Data |= BV(CRCO);
    }

    NRF24L01_WriteByte(pHandle, CONFIG, u8Data);
}

rf24_crc_len_e NRF24L01_GetCRCLength(spi_nrf24l01_t* pHandle)
{
    rf24_crc_len_e eLength = RF24_CRC_DISABLED;
    uint8_t        u8Data  = NRF24L01_ReadByte(pHandle, CONFIG) & (BV(CRCO) | BV(EN_CRC));

    if (u8Data & BV(EN_CRC))
    {
        if (u8Data & BV(CRCO))
        {
            eLength = RF24_CRC_16;
        }
        else
        {
            eLength = RF24_CRC_8;
        }
    }

    return eLength;
}

void NRF24L01_DisableCRC(spi_nrf24l01_t* pHandle)
{
    uint8_t u8Data = NRF24L01_ReadByte(pHandle, CONFIG) & ~BV(EN_CRC);
    NRF24L01_WriteByte(pHandle, CONFIG, u8Data);
}

void NRF24L01_SetRetries(spi_nrf24l01_t* pHandle, uint8_t u8Delay, uint8_t u8Count)
{
    NRF24L01_WriteByte(pHandle, SETUP_RETR, (u8Delay & 0x0F) << ARD | (u8Count & 0x0F) << ARC);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

#define DEMO 1

void NRF24L01_Test(void)
{
#ifdef BOARD_STM32F407VET6_XWS

    spi_mst_t spi = {
        .MISO = {NRF24L01_MISO_PIN},
        .MOSI = {NRF24L01_MOSI_PIN},
        .SCLK = {NRF24L01_SCLK_PIN},
        .CS   = {NRF24L01_CS_PIN},
    };

    spi_nrf24l01_t nrf24l01 = {
        .hSPI = &spi,
        .CE   = {NRF24L01_CE_PIN},
    };

#endif

    SPI_Master_Init(&spi, 500000, SPI_DUTYCYCLE_50_50, NRF24L01_SPI_TIMING | SPI_FLAG_SOFT_CS);

#if DEMO == 0  // getting started

#define CONFIG_ROLE 0  // 0:tx 1:rx

    // Radio pipe addresses for the 2 nodes to communicate.
    const uint64_t au64Pipe[2] = {0xF0F0F0F0E1, 0xF0F0F0F0D2};

    NRF24L01_Init(&nrf24l01);
    NRF24L01_SetRetries(&nrf24l01, 15, 15);

#if CONFIG_ROLE == 0
    LOGD("TRANSMIT ROLE");
    NRF24L01_OpenWritingPipe(&nrf24l01, au64Pipe[0]);
    NRF24L01_OpenReadingPipe(&nrf24l01, 1, au64Pipe[1]);
#else
    LOGD("RECEIVE ROLE");
    NRF24L01_OpenWritingPipe(&nrf24l01, au64Pipe[1]);
    NRF24L01_OpenReadingPipe(&nrf24l01, 1, au64Pipe[0]);
#endif

    NRF24L01_StartListening(&nrf24l01);

    NRF24L01_PrintDetails(&nrf24l01);

    while (1)
    {
#if CONFIG_ROLE == 0

        //
        // Ping out role.  Repeatedly send the current time
        //

        // First, stop listening so we can talk.
        NRF24L01_StopListening(&nrf24l01);

        // Take the time, and send it.  This will block until complete
        tick_t TickTx_Us = GetTickMs();
        LOGI("Now sending %lu...", TickTx_Us);
        bool bOk = NRF24L01_WriteData(&nrf24l01, &TickTx_Us, sizeof(tick_t));

        if (bOk)
        {
            LOGI("Tx success.");
        }
        else
        {
            LOGI("Tx failed.");
        }

        // Now, continue listening
        NRF24L01_StartListening(&nrf24l01);

        // Wait here until we get a response, or timeout (250ms)
        bool   bTimeout     = false;
        tick_t TickStart_Us = GetTickUs();
        while (!NRF24L01_IsAvailable(&nrf24l01))
        {
            if (DelayNonBlockMs(TickStart_Us, 200))
            {
                bTimeout = true;
                break;
            }
        }

        // Describe the results
        if (bTimeout)
        {
            LOGW("Failed, response timed out.");
        }
        else
        {
            // Grab the response, compare, and send to debugging spew
            tick_t TickRx_Us;
            NRF24L01_ReadData(&nrf24l01, &TickRx_Us, sizeof(tick_t));

            // Spew it
            LOGI("Got response %lu", TickRx_Us);
        }

        // Try again 1s later
        DelayBlockMs(1000);

#else

        //
        // Pong back role.  Receive each packet, dump it out, and send it back
        //

        // if there is data ready
        if (NRF24L01_IsAvailable(&nrf24l01))
        {
            // Dump the payloads until we've gotten everything
            tick_t TickRxVal;
            bool   bDone = false;
            while (!bDone)
            {
                // Fetch the payload, and see if this was the last one.
                bDone = NRF24L01_ReadData(&nrf24l01, &TickRxVal, sizeof(tick_t));

                // Spew it
                LOGI("Got payload %lu...", TickRxVal);

                // Delay just a little bit to let the other unit
                // make the transition to receiver
                DelayBlockMs(20);
            }

            // First, stop listening so we can talk
            NRF24L01_StopListening(&nrf24l01);

            // Send the final one back.
            NRF24L01_WriteData(&nrf24l01, &TickRxVal, sizeof(unsigned long));
            LOGI("Sent response.");

            // Now, resume listening so we catch the next packets.
            NRF24L01_StartListening(&nrf24l01);
        }

#endif
    }

#elif DEMO == 1  // scanner

    const uint8_t  num_channels = 128;
    static uint8_t values[num_channels];

    NRF24L01_Init(&nrf24l01);
    NRF24L01_SetAutoAck(&nrf24l01, false);

    // Get into standby mode
    NRF24L01_StartListening(&nrf24l01);
    NRF24L01_StopListening(&nrf24l01);

    // Print out header, high then low digit
    int i = 100;
    while (i < num_channels)
    {
        printf("%x", i >> 4);
        ++i;
    }
    printf("");
    i = 0;
    while (i < num_channels)
    {
        printf("%x", i & 0x0F);
        ++i;
    }
    printf("");

    const int num_reps = 1;

    while (1)
    {
        LOGD("here");
        // Clear measurement values
        memset(values, 0, sizeof(values));

        // Scan all channels num_reps times
        int rep_counter = num_reps;
        while (rep_counter--)
        {
            int i = num_channels;
            LOGD("%d", rep_counter);
            while (i--)
            {
                // Select this channel
                NRF24L01_SetChannel(&nrf24l01, i);

                // Listen for a little
                NRF24L01_StartListening(&nrf24l01);
                // DelayBlockMs(128);
                DelayBlockMs(10);
                NRF24L01_StopListening(&nrf24l01);

                // Did we get a carrier?
                if (NRF24L01_TestCarrier(&nrf24l01))
                {
                    ++values[i];
                }
            }
        }

        // Print out channel measurements, clamped to a single hex digit
        int i = 0;
        while (i < num_channels)
        {
            printf("%x", MIN(0x0F, values[i] & 0x0F));
            ++i;
        }
        printf("");
    }

#endif
}

#endif /* CONFIG_DEMOS_SW */
