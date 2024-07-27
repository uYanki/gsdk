#include "spi_mcp2515.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "mcp2515"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief mcp2515 bitrate configurations @8M crystal oscillator
 */
#define MCP2515_8MHz_1000kBPS_CFG1 (0x00)
#define MCP2515_8MHz_1000kBPS_CFG2 (0x80)
#define MCP2515_8MHz_1000kBPS_CFG3 (0x80)

#define MCP2515_8MHz_500kBPS_CFG1  (0x00)
#define MCP2515_8MHz_500kBPS_CFG2  (0x90)
#define MCP2515_8MHz_500kBPS_CFG3  (0x82)

#define MCP2515_8MHz_250kBPS_CFG1  (0x00)
#define MCP2515_8MHz_250kBPS_CFG2  (0xB1)
#define MCP2515_8MHz_250kBPS_CFG3  (0x85)

#define MCP2515_8MHz_200kBPS_CFG1  (0x00)
#define MCP2515_8MHz_200kBPS_CFG2  (0xB4)
#define MCP2515_8MHz_200kBPS_CFG3  (0x86)

#define MCP2515_8MHz_125kBPS_CFG1  (0x01)
#define MCP2515_8MHz_125kBPS_CFG2  (0xB1)
#define MCP2515_8MHz_125kBPS_CFG3  (0x85)

#define MCP2515_8MHz_100kBPS_CFG1  (0x01)
#define MCP2515_8MHz_100kBPS_CFG2  (0xB4)
#define MCP2515_8MHz_100kBPS_CFG3  (0x86)

#define MCP2515_8MHz_80kBPS_CFG1   (0x01)
#define MCP2515_8MHz_80kBPS_CFG2   (0xBF)
#define MCP2515_8MHz_80kBPS_CFG3   (0x87)

#define MCP2515_8MHz_50kBPS_CFG1   (0x03)
#define MCP2515_8MHz_50kBPS_CFG2   (0xB4)
#define MCP2515_8MHz_50kBPS_CFG3   (0x86)

#define MCP2515_8MHz_40kBPS_CFG1   (0x03)
#define MCP2515_8MHz_40kBPS_CFG2   (0xBF)
#define MCP2515_8MHz_40kBPS_CFG3   (0x87)

#define MCP2515_8MHz_33k3BPS_CFG1  (0x47)
#define MCP2515_8MHz_33k3BPS_CFG2  (0xE2)
#define MCP2515_8MHz_33k3BPS_CFG3  (0x85)

#define MCP2515_8MHz_31k25BPS_CFG1 (0x07)
#define MCP2515_8MHz_31k25BPS_CFG2 (0xA4)
#define MCP2515_8MHz_31k25BPS_CFG3 (0x84)

#define MCP2515_8MHz_20kBPS_CFG1   (0x07)
#define MCP2515_8MHz_20kBPS_CFG2   (0xBF)
#define MCP2515_8MHz_20kBPS_CFG3   (0x87)

#define MCP2515_8MHz_10kBPS_CFG1   (0x0F)
#define MCP2515_8MHz_10kBPS_CFG2   (0xBF)
#define MCP2515_8MHz_10kBPS_CFG3   (0x87)

#define MCP2515_8MHz_5kBPS_CFG1    (0x1F)
#define MCP2515_8MHz_5kBPS_CFG2    (0xBF)
#define MCP2515_8MHz_5kBPS_CFG3    (0x87)

/**
 * @brief mcp2515 bitrate configurations @16M crystal oscillator
 */
#define MCP2515_16MHz_1000kBPS_CFG1 (0x00)
#define MCP2515_16MHz_1000kBPS_CFG2 (0xD0)
#define MCP2515_16MHz_1000kBPS_CFG3 (0x82)

#define MCP2515_16MHz_500kBPS_CFG1  (0x00)
#define MCP2515_16MHz_500kBPS_CFG2  (0xF0)
#define MCP2515_16MHz_500kBPS_CFG3  (0x86)

#define MCP2515_16MHz_250kBPS_CFG1  (0x41)
#define MCP2515_16MHz_250kBPS_CFG2  (0xF1)
#define MCP2515_16MHz_250kBPS_CFG3  (0x85)

#define MCP2515_16MHz_200kBPS_CFG1  (0x01)
#define MCP2515_16MHz_200kBPS_CFG2  (0xFA)
#define MCP2515_16MHz_200kBPS_CFG3  (0x87)

#define MCP2515_16MHz_125kBPS_CFG1  (0x03)
#define MCP2515_16MHz_125kBPS_CFG2  (0xF0)
#define MCP2515_16MHz_125kBPS_CFG3  (0x86)

#define MCP2515_16MHz_100kBPS_CFG1  (0x03)
#define MCP2515_16MHz_100kBPS_CFG2  (0xFA)
#define MCP2515_16MHz_100kBPS_CFG3  (0x87)

#define MCP2515_16MHz_95kBPS_CFG1   (0x03)
#define MCP2515_16MHz_95kBPS_CFG2   (0xAD)
#define MCP2515_16MHz_95kBPS_CFG3   (0x07)

#define MCP2515_16MHz_83k3BPS_CFG1  (0x03)
#define MCP2515_16MHz_83k3BPS_CFG2  (0xBE)
#define MCP2515_16MHz_83k3BPS_CFG3  (0x07)

#define MCP2515_16MHz_80kBPS_CFG1   (0x03)
#define MCP2515_16MHz_80kBPS_CFG2   (0xFF)
#define MCP2515_16MHz_80kBPS_CFG3   (0x87)

#define MCP2515_16MHz_50kBPS_CFG1   (0x07)
#define MCP2515_16MHz_50kBPS_CFG2   (0xFA)
#define MCP2515_16MHz_50kBPS_CFG3   (0x87)

#define MCP2515_16MHz_40kBPS_CFG1   (0x07)
#define MCP2515_16MHz_40kBPS_CFG2   (0xFF)
#define MCP2515_16MHz_40kBPS_CFG3   (0x87)

#define MCP2515_16MHz_33k3BPS_CFG1  (0x4E)
#define MCP2515_16MHz_33k3BPS_CFG2  (0xF1)
#define MCP2515_16MHz_33k3BPS_CFG3  (0x85)

#define MCP2515_16MHz_20kBPS_CFG1   (0x0F)
#define MCP2515_16MHz_20kBPS_CFG2   (0xFF)
#define MCP2515_16MHz_20kBPS_CFG3   (0x87)

#define MCP2515_16MHz_10kBPS_CFG1   (0x1F)
#define MCP2515_16MHz_10kBPS_CFG2   (0xFF)
#define MCP2515_16MHz_10kBPS_CFG3   (0x87)

#define MCP2515_16MHz_5kBPS_CFG1    (0x3F)
#define MCP2515_16MHz_5kBPS_CFG2    (0xFF)
#define MCP2515_16MHz_5kBPS_CFG3    (0x87)

/**
 * @brief mcp2515 bitrate configurations @20M crystal oscillator
 */
#define MCP2515_20MHz_1000kBPS_CFG1 (0x00)
#define MCP2515_20MHz_1000kBPS_CFG2 (0xD9)
#define MCP2515_20MHz_1000kBPS_CFG3 (0x82)

#define MCP2515_20MHz_500kBPS_CFG1  (0x00)
#define MCP2515_20MHz_500kBPS_CFG2  (0xFA)
#define MCP2515_20MHz_500kBPS_CFG3  (0x87)

#define MCP2515_20MHz_250kBPS_CFG1  (0x41)
#define MCP2515_20MHz_250kBPS_CFG2  (0xFB)
#define MCP2515_20MHz_250kBPS_CFG3  (0x86)

#define MCP2515_20MHz_200kBPS_CFG1  (0x01)
#define MCP2515_20MHz_200kBPS_CFG2  (0xFF)
#define MCP2515_20MHz_200kBPS_CFG3  (0x87)

#define MCP2515_20MHz_125kBPS_CFG1  (0x03)
#define MCP2515_20MHz_125kBPS_CFG2  (0xFA)
#define MCP2515_20MHz_125kBPS_CFG3  (0x87)

#define MCP2515_20MHz_100kBPS_CFG1  (0x04)
#define MCP2515_20MHz_100kBPS_CFG2  (0xFA)
#define MCP2515_20MHz_100kBPS_CFG3  (0x87)

#define MCP2515_20MHz_83k3BPS_CFG1  (0x04)
#define MCP2515_20MHz_83k3BPS_CFG2  (0xFE)
#define MCP2515_20MHz_83k3BPS_CFG3  (0x87)

#define MCP2515_20MHz_80kBPS_CFG1   (0x04)
#define MCP2515_20MHz_80kBPS_CFG2   (0xFF)
#define MCP2515_20MHz_80kBPS_CFG3   (0x87)

#define MCP2515_20MHz_50kBPS_CFG1   (0x09)
#define MCP2515_20MHz_50kBPS_CFG2   (0xFA)
#define MCP2515_20MHz_50kBPS_CFG3   (0x87)

#define MCP2515_20MHz_40kBPS_CFG1   (0x09)
#define MCP2515_20MHz_40kBPS_CFG2   (0xFF)
#define MCP2515_20MHz_40kBPS_CFG3   (0x87)

#define MCP2515_20MHz_33k3BPS_CFG1  (0x0B)
#define MCP2515_20MHz_33k3BPS_CFG2  (0xFF)
#define MCP2515_20MHz_33k3BPS_CFG3  (0x87)

/**
 * @brief mcp2515 registers
 */
#define MCP2515_REG_RXF0SIDH 0x00
#define MCP2515_REG_RXF0SIDL 0x01
#define MCP2515_REG_RXF0EID8 0x02
#define MCP2515_REG_RXF0EID0 0x03
#define MCP2515_REG_RXF1SIDH 0x04
#define MCP2515_REG_RXF1SIDL 0x05
#define MCP2515_REG_RXF1EID8 0x06
#define MCP2515_REG_RXF1EID0 0x07
#define MCP2515_REG_RXF2SIDH 0x08
#define MCP2515_REG_RXF2SIDL 0x09
#define MCP2515_REG_RXF2EID8 0x0A
#define MCP2515_REG_RXF2EID0 0x0B
#define MCP2515_REG_CANSTAT  0x0E
#define MCP2515_REG_CANCTRL  0x0F
#define MCP2515_REG_RXF3SIDH 0x10
#define MCP2515_REG_RXF3SIDL 0x11
#define MCP2515_REG_RXF3EID8 0x12
#define MCP2515_REG_RXF3EID0 0x13
#define MCP2515_REG_RXF4SIDH 0x14
#define MCP2515_REG_RXF4SIDL 0x15
#define MCP2515_REG_RXF4EID8 0x16
#define MCP2515_REG_RXF4EID0 0x17
#define MCP2515_REG_RXF5SIDH 0x18
#define MCP2515_REG_RXF5SIDL 0x19
#define MCP2515_REG_RXF5EID8 0x1A
#define MCP2515_REG_RXF5EID0 0x1B
#define MCP2515_REG_TEC      0x1C
#define MCP2515_REG_REC      0x1D
#define MCP2515_REG_RXM0SIDH 0x20
#define MCP2515_REG_RXM0SIDL 0x21
#define MCP2515_REG_RXM0EID8 0x22
#define MCP2515_REG_RXM0EID0 0x23
#define MCP2515_REG_RXM1SIDH 0x24
#define MCP2515_REG_RXM1SIDL 0x25
#define MCP2515_REG_RXM1EID8 0x26
#define MCP2515_REG_RXM1EID0 0x27
#define MCP2515_REG_CNF3     0x28
#define MCP2515_REG_CNF2     0x29
#define MCP2515_REG_CNF1     0x2A
#define MCP2515_REG_CANINTE  0x2B
#define MCP2515_REG_CANINTF  0x2C
#define MCP2515_REG_EFLG     0x2D
#define MCP2515_REG_TXB0CTRL 0x30
#define MCP2515_REG_TXB0SIDH 0x31
#define MCP2515_REG_TXB0SIDL 0x32
#define MCP2515_REG_TXB0EID8 0x33
#define MCP2515_REG_TXB0EID0 0x34
#define MCP2515_REG_TXB0DLC  0x35
#define MCP2515_REG_TXB0DATA 0x36
#define MCP2515_REG_TXB1CTRL 0x40
#define MCP2515_REG_TXB1SIDH 0x41
#define MCP2515_REG_TXB1SIDL 0x42
#define MCP2515_REG_TXB1EID8 0x43
#define MCP2515_REG_TXB1EID0 0x44
#define MCP2515_REG_TXB1DLC  0x45
#define MCP2515_REG_TXB1DATA 0x46
#define MCP2515_REG_TXB2CTRL 0x50
#define MCP2515_REG_TXB2SIDH 0x51
#define MCP2515_REG_TXB2SIDL 0x52
#define MCP2515_REG_TXB2EID8 0x53
#define MCP2515_REG_TXB2EID0 0x54
#define MCP2515_REG_TXB2DLC  0x55
#define MCP2515_REG_TXB2DATA 0x56
#define MCP2515_REG_RXB0CTRL 0x60
#define MCP2515_REG_RXB0SIDH 0x61
#define MCP2515_REG_RXB0SIDL 0x62
#define MCP2515_REG_RXB0EID8 0x63
#define MCP2515_REG_RXB0EID0 0x64
#define MCP2515_REG_RXB0DLC  0x65
#define MCP2515_REG_RXB0DATA 0x66
#define MCP2515_REG_RXB1CTRL 0x70
#define MCP2515_REG_RXB1SIDH 0x71
#define MCP2515_REG_RXB1SIDL 0x72
#define MCP2515_REG_RXB1EID8 0x73
#define MCP2515_REG_RXB1EID0 0x74
#define MCP2515_REG_RXB1DLC  0x75
#define MCP2515_REG_RXB1DATA 0x76

/**
 * @brief mcp2515 instructions
 */
#define MCP2515_INST_WRITE       0x02
#define MCP2515_INST_READ        0x03
#define MCP2515_INST_BITMOD      0x05
#define MCP2515_INST_LOAD_TX0    0x40
#define MCP2515_INST_LOAD_TX1    0x42
#define MCP2515_INST_LOAD_TX2    0x44
#define MCP2515_INST_RTS_TX0     0x81
#define MCP2515_INST_RTS_TX1     0x82
#define MCP2515_INST_RTS_TX2     0x84
#define MCP2515_INST_RTS_ALL     0x87
#define MCP2515_INST_READ_RX0    0x90
#define MCP2515_INST_READ_RX1    0x94
#define MCP2515_INST_READ_STATUS 0xA0
#define MCP2515_INST_RX_STATUS   0xB0
#define MCP2515_INST_RESET       0xC0

#define CANCTRL_REQOP            0xE0
#define CANCTRL_ABAT             0x10
#define CANCTRL_OSM              0x08
#define CANCTRL_CLKEN            0x04
#define CANCTRL_CLKPRE           0x03

#define CANSTAT_OPMOD            0xE0
#define CANSTAT_ICOD             0x0E

#define CNF3_SOF                 0x80

#define TXB_EXIDE_MASK           0x08
#define DLC_MASK                 0x0F
#define RTR_MASK                 0x40

#define RXBnCTRL_RXM_STD         0x20
#define RXBnCTRL_RXM_EXT         0x40
#define RXBnCTRL_RXM_STDEXT      0x00
#define RXBnCTRL_RXM_MASK        0x60
#define RXBnCTRL_RTR             0x08
#define RXB0CTRL_BUKT            0x04
#define RXB0CTRL_FILHIT_MASK     0x03
#define RXB1CTRL_FILHIT_MASK     0x07
#define RXB0CTRL_FILHIT          0x00
#define RXB1CTRL_FILHIT          0x01

#define MCP2515_SIDH             0
#define MCP2515_SIDL             1
#define MCP2515_EID8             2
#define MCP2515_EID0             3
#define MCP2515_DLC              4
#define MCP2515_DATA             5

static const uint8_t STAT_RXIF_MASK = STAT_RX0IF | STAT_RX1IF;

static const uint8_t EFLG_MCP2515_ERRORMASK = EFLG_RX1OVR | EFLG_RX0OVR | EFLG_TXBO | EFLG_TXEP | EFLG_RXEP;

static const int N_TXBUFFERS = 3;
static const int N_RXBUFFERS = 2;

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const struct TXBn_REGS TXB[N_TXBUFFERS] = {
    {MCP2515_REG_TXB0CTRL, MCP2515_REG_TXB0SIDH, MCP2515_REG_TXB0DATA},
    {MCP2515_REG_TXB1CTRL, MCP2515_REG_TXB1SIDH, MCP2515_REG_TXB1DATA},
    {MCP2515_REG_TXB2CTRL, MCP2515_REG_TXB2SIDH, MCP2515_REG_TXB2DATA}
};

const struct RXBn_REGS RXB[N_RXBUFFERS] = {
    {MCP2515_REG_RXB0CTRL, MCP2515_REG_RXB0SIDH, MCP2515_REG_RXB0DATA, CANINTF_RX0IF},
    {MCP2515_REG_RXB1CTRL, MCP2515_REG_RXB1SIDH, MCP2515_REG_RXB1DATA, CANINTF_RX1IF}
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MCP2515_Init(spi_mcp2515_t* pHandle)
{
    SPI_Master_Deselect(pHandle->hSPI);
}

mcp2515_error_e MCP2515_Reset(spi_mcp2515_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, MCP2515_INST_RESET);
    SPI_Master_Deselect(pHandle->hSPI);

    DelayBlockMs(10);

    uint8_t zeros[14];
    memset(zeros, 0, sizeof(zeros));
    MCP2515_SetRegisters(pHandle, MCP2515_REG_TXB0CTRL, zeros, 14);
    MCP2515_SetRegisters(pHandle, MCP2515_REG_TXB1CTRL, zeros, 14);
    MCP2515_SetRegisters(pHandle, MCP2515_REG_TXB2CTRL, zeros, 14);

    MCP2515_SetRegister(pHandle, MCP2515_REG_RXB0CTRL, 0);
    MCP2515_SetRegister(pHandle, MCP2515_REG_RXB1CTRL, 0);

    MCP2515_SetRegister(pHandle, MCP2515_REG_CANINTE, CANINTF_RX0IF | CANINTF_RX1IF | CANINTF_ERRIF | CANINTF_MERRF);

    // receives all valid messages using either Standard or Extended Identifiers that
    // meet filter criteria. RXF0 is applied for RXB0, RXF1 is applied for RXB1
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_RXB0CTRL,
                           RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK,
                           RXBnCTRL_RXM_STDEXT | RXB0CTRL_BUKT | RXB0CTRL_FILHIT);
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_RXB1CTRL,
                           RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK,
                           RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT);

    // clear filters and masks
    // do not filter any standard frames for RXF0 used by RXB0
    // do not filter any extended frames for RXF1 used by RXB1
    RXF filters[] = {RXF0, RXF1, RXF2, RXF3, RXF4, RXF5};
    for (int i = 0; i < 6; i++)
    {
        bool            ext    = (i == 1);
        mcp2515_error_e result = MCP2515_SetFilter(pHandle, filters[i], ext, 0);
        if (result != MCP2515_ERROR_OK)
        {
            return result;
        }
    }

    MASK masks[] = {MASK0, MASK1};
    for (int i = 0; i < 2; i++)
    {
        mcp2515_error_e result = MCP2515_SetFilterMask(pHandle, masks[i], true, 0);
        if (result != MCP2515_ERROR_OK)
        {
            return result;
        }
    }

    return MCP2515_ERROR_OK;
}

uint8_t MCP2515_ReadRegister(spi_mcp2515_t* pHandle, const REGISTER reg)
{
    uint8_t ret;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, MCP2515_INST_READ);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_ReceiveByte(pHandle->hSPI, &ret);
    SPI_Master_Deselect(pHandle->hSPI);

    return ret;
}

void MCP2515_ReadRegisters(spi_mcp2515_t* pHandle, const REGISTER reg, uint8_t values[], const uint8_t n)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, MCP2515_INST_READ);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_ReceiveBlock(pHandle->hSPI, values, n);
    SPI_Master_Deselect(pHandle->hSPI);
}

void MCP2515_SetRegister(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t value)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, MCP2515_INST_WRITE);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_TransmitByte(pHandle->hSPI, value);
    SPI_Master_Deselect(pHandle->hSPI);
}

void MCP2515_SetRegisters(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t values[], const uint8_t n)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, MCP2515_INST_WRITE);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_TransmitBlock(pHandle->hSPI, values, n);
    SPI_Master_Deselect(pHandle->hSPI);
}

void MCP2515_ModifyRegister(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t mask, const uint8_t data)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, MCP2515_INST_BITMOD);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_TransmitByte(pHandle->hSPI, mask);
    SPI_Master_TransmitByte(pHandle->hSPI, data);
    SPI_Master_Deselect(pHandle->hSPI);
}

uint8_t MCP2515_GetStatus(spi_mcp2515_t* pHandle)
{
    uint8_t i;
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, MCP2515_INST_READ_STATUS);
    SPI_Master_ReceiveByte(pHandle->hSPI, &i);
    SPI_Master_Deselect(pHandle->hSPI);
    return i;
}

mcp2515_error_e MCP2515_SetConfigMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_CONFIG);
}

mcp2515_error_e MCP2515_SetListenOnlyMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_LISTENONLY);
}

mcp2515_error_e MCP2515_SetSleepMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_SLEEP);
}

mcp2515_error_e MCP2515_SetLoopbackMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_LOOPBACK);
}

mcp2515_error_e MCP2515_SetNormalMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_NORMAL);
}

mcp2515_error_e MCP2515_SetMode(spi_mcp2515_t* pHandle, const CANCTRL_REQOP_MODE mode)
{
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_CANCTRL, CANCTRL_REQOP, mode);

    unsigned long endTime   = GetTickMs() + 10;
    bool          modeMatch = false;
    while (GetTickMs() < endTime)
    {
        uint8_t newmode = MCP2515_ReadRegister(pHandle, MCP2515_REG_CANSTAT);
        newmode &= CANSTAT_OPMOD;

        modeMatch = newmode == mode;

        if (modeMatch)
        {
            break;
        }
    }

    return modeMatch ? MCP2515_ERROR_OK : MCP2515_ERROR_FAIL;
}

mcp2515_error_e MCP2515_SetBitrate(spi_mcp2515_t* pHandle, const can_bps_e canSpeed, mcp2515_clock_e canClock)
{
    mcp2515_error_e error = MCP2515_SetConfigMode(pHandle);

    if (error != MCP2515_ERROR_OK)
    {
        return error;
    }

    bool bSet = true;

    uint8_t CFG[3];

    // clang-format off
    switch (canClock)
    {
        case MCP2515_8MHZ:
        {
            switch (canSpeed)
            {
                case CAN_5KBPS:    CFG[0] = MCP2515_8MHz_5kBPS_CFG1,    CFG[1] = MCP2515_8MHz_5kBPS_CFG2,    CFG[2] = MCP2515_8MHz_5kBPS_CFG3;    break;
                case CAN_10KBPS:   CFG[0] = MCP2515_8MHz_10kBPS_CFG1,   CFG[1] = MCP2515_8MHz_10kBPS_CFG2,   CFG[2] = MCP2515_8MHz_10kBPS_CFG3;   break;
                case CAN_20KBPS:   CFG[0] = MCP2515_8MHz_20kBPS_CFG1,   CFG[1] = MCP2515_8MHz_20kBPS_CFG2,   CFG[2] = MCP2515_8MHz_20kBPS_CFG3;   break;
                case CAN_31K25BPS: CFG[0] = MCP2515_8MHz_31k25BPS_CFG1, CFG[1] = MCP2515_8MHz_31k25BPS_CFG2, CFG[2] = MCP2515_8MHz_31k25BPS_CFG3; break;
                case CAN_33KBPS:   CFG[0] = MCP2515_8MHz_33k3BPS_CFG1,  CFG[1] = MCP2515_8MHz_33k3BPS_CFG2,  CFG[2] = MCP2515_8MHz_33k3BPS_CFG3;  break;
                case CAN_40KBPS:   CFG[0] = MCP2515_8MHz_40kBPS_CFG1,   CFG[1] = MCP2515_8MHz_40kBPS_CFG2,   CFG[2] = MCP2515_8MHz_40kBPS_CFG3;   break;
                case CAN_50KBPS:   CFG[0] = MCP2515_8MHz_50kBPS_CFG1,   CFG[1] = MCP2515_8MHz_50kBPS_CFG2,   CFG[2] = MCP2515_8MHz_50kBPS_CFG3;   break;
                case CAN_80KBPS:   CFG[0] = MCP2515_8MHz_80kBPS_CFG1,   CFG[1] = MCP2515_8MHz_80kBPS_CFG2,   CFG[2] = MCP2515_8MHz_80kBPS_CFG3;   break;
                case CAN_100KBPS:  CFG[0] = MCP2515_8MHz_100kBPS_CFG1,  CFG[1] = MCP2515_8MHz_100kBPS_CFG2,  CFG[2] = MCP2515_8MHz_100kBPS_CFG3;  break;
                case CAN_125KBPS:  CFG[0] = MCP2515_8MHz_125kBPS_CFG1,  CFG[1] = MCP2515_8MHz_125kBPS_CFG2,  CFG[2] = MCP2515_8MHz_125kBPS_CFG3;  break;
                case CAN_200KBPS:  CFG[0] = MCP2515_8MHz_200kBPS_CFG1,  CFG[1] = MCP2515_8MHz_200kBPS_CFG2,  CFG[2] = MCP2515_8MHz_200kBPS_CFG3;  break;
                case CAN_250KBPS:  CFG[0] = MCP2515_8MHz_250kBPS_CFG1,  CFG[1] = MCP2515_8MHz_250kBPS_CFG2,  CFG[2] = MCP2515_8MHz_250kBPS_CFG3;  break;
                case CAN_500KBPS:  CFG[0] = MCP2515_8MHz_500kBPS_CFG1,  CFG[1] = MCP2515_8MHz_500kBPS_CFG2,  CFG[2] = MCP2515_8MHz_500kBPS_CFG3;  break;
                case CAN_1000KBPS: CFG[0] = MCP2515_8MHz_1000kBPS_CFG1, CFG[1] = MCP2515_8MHz_1000kBPS_CFG2, CFG[2] = MCP2515_8MHz_1000kBPS_CFG3; break;
                default: bSet = false; break;
            }
            break;
        }

        case MCP2515_16MHZ:
        {
            switch (canSpeed)
            {
                case CAN_5KBPS:    CFG[0] = MCP2515_16MHz_5kBPS_CFG1,    CFG[1] = MCP2515_16MHz_5kBPS_CFG2,    CFG[2] = MCP2515_16MHz_5kBPS_CFG3;    break;
                case CAN_10KBPS:   CFG[0] = MCP2515_16MHz_10kBPS_CFG1,   CFG[1] = MCP2515_16MHz_10kBPS_CFG2,   CFG[2] = MCP2515_16MHz_10kBPS_CFG3;   break;
                case CAN_20KBPS:   CFG[0] = MCP2515_16MHz_20kBPS_CFG1,   CFG[1] = MCP2515_16MHz_20kBPS_CFG2,   CFG[2] = MCP2515_16MHz_20kBPS_CFG3;   break;
                case CAN_33KBPS:   CFG[0] = MCP2515_16MHz_33k3BPS_CFG1,  CFG[1] = MCP2515_16MHz_33k3BPS_CFG2,  CFG[2] = MCP2515_16MHz_33k3BPS_CFG3;  break;
                case CAN_40KBPS:   CFG[0] = MCP2515_16MHz_40kBPS_CFG1,   CFG[1] = MCP2515_16MHz_40kBPS_CFG2,   CFG[2] = MCP2515_16MHz_40kBPS_CFG3;   break;
                case CAN_50KBPS:   CFG[0] = MCP2515_16MHz_50kBPS_CFG1,   CFG[1] = MCP2515_16MHz_50kBPS_CFG2,   CFG[2] = MCP2515_16MHz_50kBPS_CFG3;   break;
                case CAN_80KBPS:   CFG[0] = MCP2515_16MHz_80kBPS_CFG1,   CFG[1] = MCP2515_16MHz_80kBPS_CFG2,   CFG[2] = MCP2515_16MHz_80kBPS_CFG3;   break;
                case CAN_83K3BPS:  CFG[0] = MCP2515_16MHz_83k3BPS_CFG1,  CFG[1] = MCP2515_16MHz_83k3BPS_CFG2,  CFG[2] = MCP2515_16MHz_83k3BPS_CFG3;  break;
                case CAN_95KBPS:   CFG[0] = MCP2515_16MHz_95kBPS_CFG1,   CFG[1] = MCP2515_16MHz_95kBPS_CFG2,   CFG[2] = MCP2515_16MHz_95kBPS_CFG3;   break;
                case CAN_100KBPS:  CFG[0] = MCP2515_16MHz_100kBPS_CFG1,  CFG[1] = MCP2515_16MHz_100kBPS_CFG2,  CFG[2] = MCP2515_16MHz_100kBPS_CFG3;  break;
                case CAN_125KBPS:  CFG[0] = MCP2515_16MHz_125kBPS_CFG1,  CFG[1] = MCP2515_16MHz_125kBPS_CFG2,  CFG[2] = MCP2515_16MHz_125kBPS_CFG3;  break;
                case CAN_200KBPS:  CFG[0] = MCP2515_16MHz_200kBPS_CFG1,  CFG[1] = MCP2515_16MHz_200kBPS_CFG2,  CFG[2] = MCP2515_16MHz_200kBPS_CFG3;  break;
                case CAN_250KBPS:  CFG[0] = MCP2515_16MHz_250kBPS_CFG1,  CFG[1] = MCP2515_16MHz_250kBPS_CFG2,  CFG[2] = MCP2515_16MHz_250kBPS_CFG3;  break;
                case CAN_500KBPS:  CFG[0] = MCP2515_16MHz_500kBPS_CFG1,  CFG[1] = MCP2515_16MHz_500kBPS_CFG2,  CFG[2] = MCP2515_16MHz_500kBPS_CFG3;  break;
                case CAN_1000KBPS: CFG[0] = MCP2515_16MHz_1000kBPS_CFG1, CFG[1] = MCP2515_16MHz_1000kBPS_CFG2, CFG[2] = MCP2515_16MHz_1000kBPS_CFG3; break;
                default: bSet = false; break;
            }
            break;
        }

        case MCP2515_20MHZ:
        {
            switch (canSpeed)
            {
                case CAN_33KBPS:   CFG[0] = MCP2515_20MHz_33k3BPS_CFG1,   CFG[1] = MCP2515_20MHz_33k3BPS_CFG2,  CFG[2] = MCP2515_20MHz_33k3BPS_CFG3;  break;
                case CAN_40KBPS:   CFG[0] = MCP2515_20MHz_40kBPS_CFG1,    CFG[1] = MCP2515_20MHz_40kBPS_CFG2,   CFG[2] = MCP2515_20MHz_40kBPS_CFG3;   break;
                case CAN_50KBPS:   CFG[0] = MCP2515_20MHz_50kBPS_CFG1,    CFG[1] = MCP2515_20MHz_50kBPS_CFG2,   CFG[2] = MCP2515_20MHz_50kBPS_CFG3;   break;
                case CAN_80KBPS:   CFG[0] = MCP2515_20MHz_80kBPS_CFG1,    CFG[1] = MCP2515_20MHz_80kBPS_CFG2,   CFG[2] = MCP2515_20MHz_80kBPS_CFG3;   break;
                case CAN_83K3BPS:  CFG[0] = MCP2515_20MHz_83k3BPS_CFG1,   CFG[1] = MCP2515_20MHz_83k3BPS_CFG2,  CFG[2] = MCP2515_20MHz_83k3BPS_CFG3;  break;
                case CAN_100KBPS:  CFG[0] = MCP2515_20MHz_100kBPS_CFG1,   CFG[1] = MCP2515_20MHz_100kBPS_CFG2,  CFG[2] = MCP2515_20MHz_100kBPS_CFG3;  break;
                case CAN_125KBPS:  CFG[0] = MCP2515_20MHz_125kBPS_CFG1,   CFG[1] = MCP2515_20MHz_125kBPS_CFG2,  CFG[2] = MCP2515_20MHz_125kBPS_CFG3;  break;
                case CAN_200KBPS:  CFG[0] = MCP2515_20MHz_200kBPS_CFG1,   CFG[1] = MCP2515_20MHz_200kBPS_CFG2,  CFG[2] = MCP2515_20MHz_200kBPS_CFG3;  break;
                case CAN_250KBPS:  CFG[0] = MCP2515_20MHz_250kBPS_CFG1,   CFG[1] = MCP2515_20MHz_250kBPS_CFG2,  CFG[2] = MCP2515_20MHz_250kBPS_CFG3;  break;
                case CAN_500KBPS:  CFG[0] = MCP2515_20MHz_500kBPS_CFG1,   CFG[1] = MCP2515_20MHz_500kBPS_CFG2,  CFG[2] = MCP2515_20MHz_500kBPS_CFG3;  break;
                case CAN_1000KBPS: CFG[0] = MCP2515_20MHz_1000kBPS_CFG1,  CFG[1] = MCP2515_20MHz_1000kBPS_CFG2, CFG[2] = MCP2515_20MHz_1000kBPS_CFG3; break;
                default: bSet = false; break;
            }
            break;
        }

        default: bSet = false; break;
    }

    // clang-format on

    if (bSet)
    {
        MCP2515_SetRegister(pHandle, MCP2515_REG_CNF1, CFG[0]);
        MCP2515_SetRegister(pHandle, MCP2515_REG_CNF2, CFG[1]);
        MCP2515_SetRegister(pHandle, MCP2515_REG_CNF3, CFG[2]);
        return MCP2515_ERROR_OK;
    }
    else
    {
        return MCP2515_ERROR_FAIL;
    }
}

mcp2515_error_e MCP2515_SetClkOut(spi_mcp2515_t* pHandle, const CAN_CLKOUT divisor)
{
    if (divisor == CLKOUT_DISABLE)
    {
        /* Turn off CLKEN */
        MCP2515_ModifyRegister(pHandle, MCP2515_REG_CANCTRL, CANCTRL_CLKEN, 0x00);

        /* Turn on CLKOUT for SOF */
        MCP2515_ModifyRegister(pHandle, MCP2515_REG_CNF3, CNF3_SOF, CNF3_SOF);
        return MCP2515_ERROR_OK;
    }

    /* Set the MCP2515_Prescaler (pHandle,CLKPRE) */
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_CANCTRL, CANCTRL_CLKPRE, divisor);

    /* Turn on CLKEN */
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_CANCTRL, CANCTRL_CLKEN, CANCTRL_CLKEN);

    /* Turn off CLKOUT for SOF */
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_CNF3, CNF3_SOF, 0x00);
    return MCP2515_ERROR_OK;
}

void MCP2515_PrepareId(spi_mcp2515_t* pHandle, uint8_t* buffer, const bool ext, const uint32_t id)
{
    uint16_t canid = (uint16_t)(id & 0x0FFFF);

    if (ext)
    {
        buffer[MCP2515_EID0] = (uint8_t)(canid & 0xFF);
        buffer[MCP2515_EID8] = (uint8_t)(canid >> 8);
        canid                = (uint16_t)(id >> 16);
        buffer[MCP2515_SIDL] = (uint8_t)(canid & 0x03);
        buffer[MCP2515_SIDL] += (uint8_t)((canid & 0x1C) << 3);
        buffer[MCP2515_SIDL] |= TXB_EXIDE_MASK;
        buffer[MCP2515_SIDH] = (uint8_t)(canid >> 5);
    }
    else
    {
        buffer[MCP2515_SIDH] = (uint8_t)(canid >> 3);
        buffer[MCP2515_SIDL] = (uint8_t)((canid & 0x07) << 5);
        buffer[MCP2515_EID0] = 0;
        buffer[MCP2515_EID8] = 0;
    }
}

mcp2515_error_e MCP2515_SetFilterMask(spi_mcp2515_t* pHandle, const MASK mask, const bool ext, const uint32_t ulData)
{
    mcp2515_error_e res = MCP2515_SetConfigMode(pHandle);
    if (res != MCP2515_ERROR_OK)
    {
        return res;
    }

    uint8_t tbufdata[4];
    MCP2515_PrepareId(pHandle, tbufdata, ext, ulData);

    REGISTER reg;
    switch (mask)
    {
        case MASK0: reg = MCP2515_REG_RXM0SIDH; break;
        case MASK1: reg = MCP2515_REG_RXM1SIDH; break;
        default: return MCP2515_ERROR_FAIL;
    }

    MCP2515_SetRegisters(pHandle, reg, tbufdata, 4);

    return MCP2515_ERROR_OK;
}

mcp2515_error_e MCP2515_SetFilter(spi_mcp2515_t* pHandle, const RXF num, const bool ext, const uint32_t ulData)
{
    mcp2515_error_e res = MCP2515_SetConfigMode(pHandle);
    if (res != MCP2515_ERROR_OK)
    {
        return res;
    }

    REGISTER reg;

    switch (num)
    {
        case RXF0: reg = MCP2515_REG_RXF0SIDH; break;
        case RXF1: reg = MCP2515_REG_RXF1SIDH; break;
        case RXF2: reg = MCP2515_REG_RXF2SIDH; break;
        case RXF3: reg = MCP2515_REG_RXF3SIDH; break;
        case RXF4: reg = MCP2515_REG_RXF4SIDH; break;
        case RXF5: reg = MCP2515_REG_RXF5SIDH; break;
        default:
            return MCP2515_ERROR_FAIL;
    }

    uint8_t tbufdata[4];
    MCP2515_PrepareId(pHandle, tbufdata, ext, ulData);
    MCP2515_SetRegisters(pHandle, reg, tbufdata, 4);

    return MCP2515_ERROR_OK;
}

mcp2515_error_e MCP2515_SendMessage(spi_mcp2515_t* pHandle, const TXBn txbn, const struct can_frame* frame)
{
    if (frame->can_dlc > CAN_MAX_DLEN)
    {
        return MCP2515_ERROR_FAILTX;
    }

    const struct TXBn_REGS* txbuf = &TXB[txbn];

    uint8_t data[13];

    bool     ext = (frame->can_id & CAN_EFF_FLAG);
    bool     rtr = (frame->can_id & CAN_RTR_FLAG);
    uint32_t id  = (frame->can_id & (ext ? CAN_EFF_MASK : CAN_SFF_MASK));

    MCP2515_PrepareId(pHandle, data, ext, id);

    data[MCP2515_DLC] = rtr ? (frame->can_dlc | RTR_MASK) : frame->can_dlc;

    memcpy(&data[MCP2515_DATA], frame->data, frame->can_dlc);

    MCP2515_SetRegisters(pHandle, txbuf->SIDH, data, 5 + frame->can_dlc);

    MCP2515_ModifyRegister(pHandle, txbuf->CTRL, TXB_TXREQ, TXB_TXREQ);

    uint8_t ctrl = MCP2515_ReadRegister(pHandle, txbuf->CTRL);
    if ((ctrl & (TXB_ABTF | TXB_MLOA | TXB_TXERR)) != 0)
    {
        return MCP2515_ERROR_FAILTX;
    }
    return MCP2515_ERROR_OK;
}

mcp2515_error_e MCP2515_SendMessageEx(spi_mcp2515_t* pHandle, const struct can_frame* frame)
{
    if (frame->can_dlc > CAN_MAX_DLEN)
    {
        return MCP2515_ERROR_FAILTX;
    }

    TXBn txBuffers[N_TXBUFFERS] = {TXB0, TXB1, TXB2};

    for (int i = 0; i < N_TXBUFFERS; i++)
    {
        const struct TXBn_REGS* txbuf   = &TXB[txBuffers[i]];
        uint8_t                 ctrlval = MCP2515_ReadRegister(pHandle, txbuf->CTRL);
        if ((ctrlval & TXB_TXREQ) == 0)
        {
            return MCP2515_SendMessage(pHandle, txBuffers[i], frame);
        }
    }

    return MCP2515_ERROR_ALLTXBUSY;
}

mcp2515_error_e MCP2515_ReadMessage(spi_mcp2515_t* pHandle, const RXBn rxbn, struct can_frame* frame)
{
    const struct RXBn_REGS* rxb = &RXB[rxbn];

    uint8_t tbufdata[5];

    MCP2515_ReadRegisters(pHandle, rxb->SIDH, tbufdata, 5);

    uint32_t id = (tbufdata[MCP2515_SIDH] << 3) + (tbufdata[MCP2515_SIDL] >> 5);

    if ((tbufdata[MCP2515_SIDL] & TXB_EXIDE_MASK) == TXB_EXIDE_MASK)
    {
        id = (id << 2) + (tbufdata[MCP2515_SIDL] & 0x03);
        id = (id << 8) + tbufdata[MCP2515_EID8];
        id = (id << 8) + tbufdata[MCP2515_EID0];
        id |= CAN_EFF_FLAG;
    }

    uint8_t dlc = (tbufdata[MCP2515_DLC] & DLC_MASK);
    if (dlc > CAN_MAX_DLEN)
    {
        return MCP2515_ERROR_FAIL;
    }

    uint8_t ctrl = MCP2515_ReadRegister(pHandle, rxb->CTRL);
    if (ctrl & RXBnCTRL_RTR)
    {
        id |= CAN_RTR_FLAG;
    }

    frame->can_id  = id;
    frame->can_dlc = dlc;

    MCP2515_ReadRegisters(pHandle, rxb->DATA, frame->data, dlc);

    MCP2515_ModifyRegister(pHandle, MCP2515_REG_CANINTF, rxb->CANINTF_RXnIF, 0);

    return MCP2515_ERROR_OK;
}

mcp2515_error_e MCP2515_ReadMessageEx(spi_mcp2515_t* pHandle, struct can_frame* frame)
{
    mcp2515_error_e rc;
    uint8_t         stat = MCP2515_GetStatus(pHandle);

    if (stat & STAT_RX0IF)
    {
        rc = MCP2515_ReadMessage(pHandle, RXB0, frame);
    }
    else if (stat & STAT_RX1IF)
    {
        rc = MCP2515_ReadMessage(pHandle, RXB1, frame);
    }
    else
    {
        rc = MCP2515_ERROR_NOMSG;
    }

    return rc;
}

bool MCP2515_CheckReceive(spi_mcp2515_t* pHandle)
{
    uint8_t res = MCP2515_GetStatus(pHandle);

    if (res & STAT_RXIF_MASK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MCP2515_CheckError(spi_mcp2515_t* pHandle)
{
    uint8_t eflg = MCP2515_GetErrorFlags(pHandle);

    if (eflg & EFLG_MCP2515_ERRORMASK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint8_t MCP2515_GetErrorFlags(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP2515_REG_EFLG);
}

void MCP2515_ClearRXnOVRFlags(spi_mcp2515_t* pHandle)
{
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
}

uint8_t MCP2515_GetInterrupts(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP2515_REG_CANINTF);
}

void MCP2515_ClearInterrupts(spi_mcp2515_t* pHandle)
{
    MCP2515_SetRegister(pHandle, MCP2515_REG_CANINTF, 0);
}

uint8_t MCP2515_GetInterruptMask(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP2515_REG_CANINTE);
}

void MCP2515_ClearTXInterrupts(spi_mcp2515_t* pHandle)
{
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_CANINTF, (CANINTF_TX0IF | CANINTF_TX1IF | CANINTF_TX2IF), 0);
}

void MCP2515_ClearRXnOVR(spi_mcp2515_t* pHandle)
{
    uint8_t eflg = MCP2515_GetErrorFlags(pHandle);
    if (eflg != 0)
    {
        MCP2515_ClearRXnOVRFlags(pHandle);
        MCP2515_ClearInterrupts(pHandle);
        // MCP2515_ModifyRegister(pHandle,MCP2515_REG_CANINTF, CANINTF_ERRIF, 0);
    }
}

void MCP2515_ClearMERR(spi_mcp2515_t* pHandle)
{
    // MCP2515_ModifyRegister(pHandle,MCP2515_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
    // MCP2515_ClearInterrupts(pHandle,);
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_CANINTF, CANINTF_MERRF, 0);
}

void MCP2515_ClearERRIF(spi_mcp2515_t* pHandle)
{
    // MCP2515_ModifyRegister(pHandle,MCP2515_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
    // MCP2515_ClearInterrupts(pHandle,);
    MCP2515_ModifyRegister(pHandle, MCP2515_REG_CANINTF, CANINTF_ERRIF, 0);
}

uint8_t errorCountRX(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP2515_REG_REC);
}

uint8_t errorCountTX(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP2515_REG_TEC);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void MCP2515_Test(void)
{
#if defined(BOARD_STM32F407VET6_XWS)

    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_5},
        .MOSI = {GPIOA, GPIO_PIN_4},
        .SCLK = {GPIOA, GPIO_PIN_6},
        .CS   = {GPIOA, GPIO_PIN_3},
    };

#elif defined(BOARD_AT32F415CB_DEV)
    /* SPI2: PB12/CS, PB13/SCLK, PB14/MISO, PB15/MOSI */

    spi_mst_t spi = {
        .MOSI = {GPIOB, GPIO_PINS_15}, /*MOSI*/
        .MISO = {GPIOB, GPIO_PINS_14}, /*MISO*/
        .SCLK = {GPIOB, GPIO_PINS_13}, /*SCLK*/
        .CS   = {GPIOB, GPIO_PINS_12}, /*CS*/
        .SPIx = SPI2,
    };

#elif defined(BOARD_CS32F103C8T6_QG)
    /* SPI2: PB12/CS, PB13/SCLK, PB14/MISO, PB15/MOSI */

    spi_mst_t spi = {
        .MOSI = {GPIOA, GPIO_PIN_7 }, /*MOSI*/
        .MISO = {GPIOA, GPIO_PIN_6 }, /*MISO*/
        .SCLK = {GPIOA, GPIO_PIN_5 }, /*SCLK*/
        .CS   = {GPIOB, GPIO_PIN_13}, /*CS*/
    };

#endif

    spi_mcp2515_t mcp2515 = {
        .hSPI = &spi,
    };

    SPI_Master_Init(&spi, 100000, SPI_DUTYCYCLE_33_67, MCP2515_SPI_TIMING | SPI_FLAG_SOFT_CS);

    MCP2515_Init(&mcp2515);

    struct can_frame canMsg1;
    struct can_frame canMsg2;

    canMsg1.can_id  = 0x0F6;
    canMsg1.can_dlc = 8;
    canMsg1.data[0] = 0x8E;
    canMsg1.data[1] = 0x87;
    canMsg1.data[2] = 0x32;
    canMsg1.data[3] = 0xFA;
    canMsg1.data[4] = 0x26;
    canMsg1.data[5] = 0x8E;
    canMsg1.data[6] = 0xBE;
    canMsg1.data[7] = 0x86;

    canMsg2.can_id  = 0x036;
    canMsg2.can_dlc = 8;
    canMsg2.data[0] = 0x0E;
    canMsg2.data[1] = 0x00;
    canMsg2.data[2] = 0x00;
    canMsg2.data[3] = 0x08;
    canMsg2.data[4] = 0x01;
    canMsg2.data[5] = 0x00;
    canMsg2.data[6] = 0x00;
    canMsg2.data[7] = 0xA0;

    MCP2515_Reset(&mcp2515);
    MCP2515_SetBitrate(&mcp2515, CAN_125KBPS, MCP2515_8MHZ);
    MCP2515_SetNormalMode(&mcp2515);

    while (1)
    {
        MCP2515_SendMessageEx(&mcp2515, &canMsg1);
        MCP2515_SendMessageEx(&mcp2515, &canMsg2);
        DelayBlockMs(1000);
    }
}

#endif
