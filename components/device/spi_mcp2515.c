#include "spi_mcp2515.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "mcp2515"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MCP2515_Init(spi_mcp2515_t* pHandle)
{
    SPI_Master_Deselect(pHandle->hSPI);
}

const struct TXBn_REGS TXB[N_TXBUFFERS] = {
    {MCP_TXB0CTRL, MCP_TXB0SIDH, MCP_TXB0DATA},
    {MCP_TXB1CTRL, MCP_TXB1SIDH, MCP_TXB1DATA},
    {MCP_TXB2CTRL, MCP_TXB2SIDH, MCP_TXB2DATA}
};

const struct RXBn_REGS RXB[N_RXBUFFERS] = {
    {MCP_RXB0CTRL, MCP_RXB0SIDH, MCP_RXB0DATA, CANINTF_RX0IF},
    {MCP_RXB1CTRL, MCP_RXB1SIDH, MCP_RXB1DATA, CANINTF_RX1IF}
};

MCP_ERROR MCP2515_Reset(spi_mcp2515_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INSTRUCTION_RESET);
    SPI_Master_Deselect(pHandle->hSPI);

    DelayBlockMs(10);

    uint8_t zeros[14];
    memset(zeros, 0, sizeof(zeros));
    MCP2515_SetRegisters(pHandle, MCP_TXB0CTRL, zeros, 14);
    MCP2515_SetRegisters(pHandle, MCP_TXB1CTRL, zeros, 14);
    MCP2515_SetRegisters(pHandle, MCP_TXB2CTRL, zeros, 14);

    MCP2515_SetRegister(pHandle, MCP_RXB0CTRL, 0);
    MCP2515_SetRegister(pHandle, MCP_RXB1CTRL, 0);

    MCP2515_SetRegister(pHandle, MCP_CANINTE, CANINTF_RX0IF | CANINTF_RX1IF | CANINTF_ERRIF | CANINTF_MERRF);

    // receives all valid messages using either Standard or Extended Identifiers that
    // meet filter criteria. RXF0 is applied for RXB0, RXF1 is applied for RXB1
    MCP2515_ModifyRegister(pHandle, MCP_RXB0CTRL,
                           RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK,
                           RXBnCTRL_RXM_STDEXT | RXB0CTRL_BUKT | RXB0CTRL_FILHIT);
    MCP2515_ModifyRegister(pHandle, MCP_RXB1CTRL,
                           RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK,
                           RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT);

    // clear filters and masks
    // do not filter any standard frames for RXF0 used by RXB0
    // do not filter any extended frames for RXF1 used by RXB1
    RXF filters[] = {RXF0, RXF1, RXF2, RXF3, RXF4, RXF5};
    for (int i = 0; i < 6; i++)
    {
        bool      ext    = (i == 1);
        MCP_ERROR result = MCP2515_SetFilter(pHandle, filters[i], ext, 0);
        if (result != MCP_ERROR_OK)
        {
            return result;
        }
    }

    MASK masks[] = {MASK0, MASK1};
    for (int i = 0; i < 2; i++)
    {
        MCP_ERROR result = MCP2515_SetFilterMask(pHandle, masks[i], true, 0);
        if (result != MCP_ERROR_OK)
        {
            return result;
        }
    }

    return MCP_ERROR_OK;
}

uint8_t MCP2515_ReadRegister(spi_mcp2515_t* pHandle, const REGISTER reg)
{
    uint8_t ret;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INSTRUCTION_READ);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_ReceiveByte(pHandle->hSPI, &ret);
    SPI_Master_Deselect(pHandle->hSPI);

    return ret;
}

void MCP2515_ReadRegisters(spi_mcp2515_t* pHandle, const REGISTER reg, uint8_t values[], const uint8_t n)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INSTRUCTION_READ);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_ReceiveBlock(pHandle->hSPI, values, n);
    SPI_Master_Deselect(pHandle->hSPI);
}

void MCP2515_SetRegister(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t value)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INSTRUCTION_WRITE);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_TransmitByte(pHandle->hSPI, value);
    SPI_Master_Deselect(pHandle->hSPI);
}

void MCP2515_SetRegisters(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t values[], const uint8_t n)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INSTRUCTION_WRITE);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_TransmitBlock(pHandle->hSPI, values, n);
    SPI_Master_Deselect(pHandle->hSPI);
}

void MCP2515_ModifyRegister(spi_mcp2515_t* pHandle, const REGISTER reg, const uint8_t mask, const uint8_t data)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INSTRUCTION_BITMOD);
    SPI_Master_TransmitByte(pHandle->hSPI, reg);
    SPI_Master_TransmitByte(pHandle->hSPI, mask);
    SPI_Master_TransmitByte(pHandle->hSPI, data);
    SPI_Master_Deselect(pHandle->hSPI);
}

uint8_t MCP2515_GetStatus(spi_mcp2515_t* pHandle)
{
    uint8_t i;
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INSTRUCTION_READ_STATUS);
    SPI_Master_ReceiveByte(pHandle->hSPI, &i);
    SPI_Master_Deselect(pHandle->hSPI);
    return i;
}

MCP_ERROR MCP2515_SetConfigMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_CONFIG);
}

MCP_ERROR MCP2515_SetListenOnlyMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_LISTENONLY);
}

MCP_ERROR MCP2515_SetSleepMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_SLEEP);
}

MCP_ERROR MCP2515_SetLoopbackMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_LOOPBACK);
}

MCP_ERROR MCP2515_SetNormalMode(spi_mcp2515_t* pHandle)
{
    return MCP2515_SetMode(pHandle, CANCTRL_REQOP_NORMAL);
}

MCP_ERROR MCP2515_SetMode(spi_mcp2515_t* pHandle, const CANCTRL_REQOP_MODE mode)
{
    MCP2515_ModifyRegister(pHandle, MCP_CANCTRL, CANCTRL_REQOP, mode);

    unsigned long endTime   = GetTickMs() + 10;
    bool          modeMatch = false;
    while (GetTickMs() < endTime)
    {
        uint8_t newmode = MCP2515_ReadRegister(pHandle, MCP_CANSTAT);
        newmode &= CANSTAT_OPMOD;

        modeMatch = newmode == mode;

        if (modeMatch)
        {
            break;
        }
    }

    return modeMatch ? MCP_ERROR_OK : MCP_ERROR_FAIL;
}

MCP_ERROR MCP2515_SetBitrate(spi_mcp2515_t* pHandle, const CAN_SPEED canSpeed, CAN_CLOCK canClock)
{
    MCP_ERROR error = MCP2515_SetConfigMode(pHandle);

    if (error != MCP_ERROR_OK)
    {
        return error;
    }

    uint8_t set, cfg1, cfg2, cfg3;
    set = 1;
    switch (canClock)
    {
        case (MCP_8MHZ):
            switch (canSpeed)
            {
                case (CAN_5KBPS):  //   5KBPS
                    cfg1 = MCP_8MHz_5kBPS_CFG1;
                    cfg2 = MCP_8MHz_5kBPS_CFG2;
                    cfg3 = MCP_8MHz_5kBPS_CFG3;
                    break;

                case (CAN_10KBPS):  //  10KBPS
                    cfg1 = MCP_8MHz_10kBPS_CFG1;
                    cfg2 = MCP_8MHz_10kBPS_CFG2;
                    cfg3 = MCP_8MHz_10kBPS_CFG3;
                    break;

                case (CAN_20KBPS):  //  20KBPS
                    cfg1 = MCP_8MHz_20kBPS_CFG1;
                    cfg2 = MCP_8MHz_20kBPS_CFG2;
                    cfg3 = MCP_8MHz_20kBPS_CFG3;
                    break;

                case (CAN_31K25BPS):  //  31.25KBPS
                    cfg1 = MCP_8MHz_31k25BPS_CFG1;
                    cfg2 = MCP_8MHz_31k25BPS_CFG2;
                    cfg3 = MCP_8MHz_31k25BPS_CFG3;
                    break;

                case (CAN_33KBPS):  //  33.333KBPS
                    cfg1 = MCP_8MHz_33k3BPS_CFG1;
                    cfg2 = MCP_8MHz_33k3BPS_CFG2;
                    cfg3 = MCP_8MHz_33k3BPS_CFG3;
                    break;

                case (CAN_40KBPS):  //  40Kbps
                    cfg1 = MCP_8MHz_40kBPS_CFG1;
                    cfg2 = MCP_8MHz_40kBPS_CFG2;
                    cfg3 = MCP_8MHz_40kBPS_CFG3;
                    break;

                case (CAN_50KBPS):  //  50Kbps
                    cfg1 = MCP_8MHz_50kBPS_CFG1;
                    cfg2 = MCP_8MHz_50kBPS_CFG2;
                    cfg3 = MCP_8MHz_50kBPS_CFG3;
                    break;

                case (CAN_80KBPS):  //  80Kbps
                    cfg1 = MCP_8MHz_80kBPS_CFG1;
                    cfg2 = MCP_8MHz_80kBPS_CFG2;
                    cfg3 = MCP_8MHz_80kBPS_CFG3;
                    break;

                case (CAN_100KBPS):  // 100Kbps
                    cfg1 = MCP_8MHz_100kBPS_CFG1;
                    cfg2 = MCP_8MHz_100kBPS_CFG2;
                    cfg3 = MCP_8MHz_100kBPS_CFG3;
                    break;

                case (CAN_125KBPS):  // 125Kbps
                    cfg1 = MCP_8MHz_125kBPS_CFG1;
                    cfg2 = MCP_8MHz_125kBPS_CFG2;
                    cfg3 = MCP_8MHz_125kBPS_CFG3;
                    break;

                case (CAN_200KBPS):  // 200Kbps
                    cfg1 = MCP_8MHz_200kBPS_CFG1;
                    cfg2 = MCP_8MHz_200kBPS_CFG2;
                    cfg3 = MCP_8MHz_200kBPS_CFG3;
                    break;

                case (CAN_250KBPS):  // 250Kbps
                    cfg1 = MCP_8MHz_250kBPS_CFG1;
                    cfg2 = MCP_8MHz_250kBPS_CFG2;
                    cfg3 = MCP_8MHz_250kBPS_CFG3;
                    break;

                case (CAN_500KBPS):  // 500Kbps
                    cfg1 = MCP_8MHz_500kBPS_CFG1;
                    cfg2 = MCP_8MHz_500kBPS_CFG2;
                    cfg3 = MCP_8MHz_500kBPS_CFG3;
                    break;

                case (CAN_1000KBPS):  //   1Mbps
                    cfg1 = MCP_8MHz_1000kBPS_CFG1;
                    cfg2 = MCP_8MHz_1000kBPS_CFG2;
                    cfg3 = MCP_8MHz_1000kBPS_CFG3;
                    break;

                default:
                    set = 0;
                    break;
            }
            break;

        case (MCP_16MHZ):
            switch (canSpeed)
            {
                case (CAN_5KBPS):  //   5Kbps
                    cfg1 = MCP_16MHz_5kBPS_CFG1;
                    cfg2 = MCP_16MHz_5kBPS_CFG2;
                    cfg3 = MCP_16MHz_5kBPS_CFG3;
                    break;

                case (CAN_10KBPS):  //  10Kbps
                    cfg1 = MCP_16MHz_10kBPS_CFG1;
                    cfg2 = MCP_16MHz_10kBPS_CFG2;
                    cfg3 = MCP_16MHz_10kBPS_CFG3;
                    break;

                case (CAN_20KBPS):  //  20Kbps
                    cfg1 = MCP_16MHz_20kBPS_CFG1;
                    cfg2 = MCP_16MHz_20kBPS_CFG2;
                    cfg3 = MCP_16MHz_20kBPS_CFG3;
                    break;

                case (CAN_33KBPS):  //  33.333Kbps
                    cfg1 = MCP_16MHz_33k3BPS_CFG1;
                    cfg2 = MCP_16MHz_33k3BPS_CFG2;
                    cfg3 = MCP_16MHz_33k3BPS_CFG3;
                    break;

                case (CAN_40KBPS):  //  40Kbps
                    cfg1 = MCP_16MHz_40kBPS_CFG1;
                    cfg2 = MCP_16MHz_40kBPS_CFG2;
                    cfg3 = MCP_16MHz_40kBPS_CFG3;
                    break;

                case (CAN_50KBPS):  //  50Kbps
                    cfg1 = MCP_16MHz_50kBPS_CFG1;
                    cfg2 = MCP_16MHz_50kBPS_CFG2;
                    cfg3 = MCP_16MHz_50kBPS_CFG3;
                    break;

                case (CAN_80KBPS):  //  80Kbps
                    cfg1 = MCP_16MHz_80kBPS_CFG1;
                    cfg2 = MCP_16MHz_80kBPS_CFG2;
                    cfg3 = MCP_16MHz_80kBPS_CFG3;
                    break;

                case (CAN_83K3BPS):  //  83.333Kbps
                    cfg1 = MCP_16MHz_83k3BPS_CFG1;
                    cfg2 = MCP_16MHz_83k3BPS_CFG2;
                    cfg3 = MCP_16MHz_83k3BPS_CFG3;
                    break;

                case (CAN_95KBPS):  //  95Kbps
                    cfg1 = MCP_16MHz_95kBPS_CFG1;
                    cfg2 = MCP_16MHz_95kBPS_CFG2;
                    cfg3 = MCP_16MHz_95kBPS_CFG3;
                    break;

                case (CAN_100KBPS):  // 100Kbps
                    cfg1 = MCP_16MHz_100kBPS_CFG1;
                    cfg2 = MCP_16MHz_100kBPS_CFG2;
                    cfg3 = MCP_16MHz_100kBPS_CFG3;
                    break;

                case (CAN_125KBPS):  // 125Kbps
                    cfg1 = MCP_16MHz_125kBPS_CFG1;
                    cfg2 = MCP_16MHz_125kBPS_CFG2;
                    cfg3 = MCP_16MHz_125kBPS_CFG3;
                    break;

                case (CAN_200KBPS):  // 200Kbps
                    cfg1 = MCP_16MHz_200kBPS_CFG1;
                    cfg2 = MCP_16MHz_200kBPS_CFG2;
                    cfg3 = MCP_16MHz_200kBPS_CFG3;
                    break;

                case (CAN_250KBPS):  // 250Kbps
                    cfg1 = MCP_16MHz_250kBPS_CFG1;
                    cfg2 = MCP_16MHz_250kBPS_CFG2;
                    cfg3 = MCP_16MHz_250kBPS_CFG3;
                    break;

                case (CAN_500KBPS):  // 500Kbps
                    cfg1 = MCP_16MHz_500kBPS_CFG1;
                    cfg2 = MCP_16MHz_500kBPS_CFG2;
                    cfg3 = MCP_16MHz_500kBPS_CFG3;
                    break;

                case (CAN_1000KBPS):  //   1Mbps
                    cfg1 = MCP_16MHz_1000kBPS_CFG1;
                    cfg2 = MCP_16MHz_1000kBPS_CFG2;
                    cfg3 = MCP_16MHz_1000kBPS_CFG3;
                    break;

                default:
                    set = 0;
                    break;
            }
            break;

        case (MCP_20MHZ):
            switch (canSpeed)
            {
                case (CAN_33KBPS): cfg1 = MCP_20MHz_33k3BPS_CFG1, cfg2 = MCP_20MHz_33k3BPS_CFG2, cfg3 = MCP_20MHz_33k3BPS_CFG3; break;
                case (CAN_40KBPS): cfg1 = MCP_20MHz_40kBPS_CFG1, cfg2 = MCP_20MHz_40kBPS_CFG2, cfg3 = MCP_20MHz_40kBPS_CFG3; break;
                case (CAN_50KBPS): cfg1 = MCP_20MHz_50kBPS_CFG1, cfg2 = MCP_20MHz_50kBPS_CFG2, cfg3 = MCP_20MHz_50kBPS_CFG3; break;
                case (CAN_80KBPS): cfg1 = MCP_20MHz_80kBPS_CFG1, cfg2 = MCP_20MHz_80kBPS_CFG2, cfg3 = MCP_20MHz_80kBPS_CFG3; break;
                case (CAN_83K3BPS): cfg1 = MCP_20MHz_83k3BPS_CFG1, cfg2 = MCP_20MHz_83k3BPS_CFG2, cfg3 = MCP_20MHz_83k3BPS_CFG3; break;
                case (CAN_100KBPS): cfg1 = MCP_20MHz_100kBPS_CFG1, cfg2 = MCP_20MHz_100kBPS_CFG2, cfg3 = MCP_20MHz_100kBPS_CFG3; break;
                case (CAN_125KBPS): cfg1 = MCP_20MHz_125kBPS_CFG1, cfg2 = MCP_20MHz_125kBPS_CFG2, cfg3 = MCP_20MHz_125kBPS_CFG3; break;
                case (CAN_200KBPS): cfg1 = MCP_20MHz_200kBPS_CFG1, cfg2 = MCP_20MHz_200kBPS_CFG2, cfg3 = MCP_20MHz_200kBPS_CFG3; break;
                case (CAN_250KBPS): cfg1 = MCP_20MHz_250kBPS_CFG1, cfg2 = MCP_20MHz_250kBPS_CFG2, cfg3 = MCP_20MHz_250kBPS_CFG3; break;
                case (CAN_500KBPS): cfg1 = MCP_20MHz_500kBPS_CFG1, cfg2 = MCP_20MHz_500kBPS_CFG2, cfg3 = MCP_20MHz_500kBPS_CFG3; break;
                case (CAN_1000KBPS): cfg1 = MCP_20MHz_1000kBPS_CFG1, cfg2 = MCP_20MHz_1000kBPS_CFG2, cfg3 = MCP_20MHz_1000kBPS_CFG3; break;
                default:
                    set = 0;
                    break;
            }
            break;

        default:
            set = 0;
            break;
    }

    if (set)
    {
        MCP2515_SetRegister(pHandle, MCP_CNF1, cfg1);
        MCP2515_SetRegister(pHandle, MCP_CNF2, cfg2);
        MCP2515_SetRegister(pHandle, MCP_CNF3, cfg3);
        return MCP_ERROR_OK;
    }
    else
    {
        return MCP_ERROR_FAIL;
    }
}

MCP_ERROR MCP2515_SetClkOut(spi_mcp2515_t* pHandle, const CAN_CLKOUT divisor)
{
    if (divisor == CLKOUT_DISABLE)
    {
        /* Turn off CLKEN */
        MCP2515_ModifyRegister(pHandle, MCP_CANCTRL, CANCTRL_CLKEN, 0x00);

        /* Turn on CLKOUT for SOF */
        MCP2515_ModifyRegister(pHandle, MCP_CNF3, CNF3_SOF, CNF3_SOF);
        return MCP_ERROR_OK;
    }

    /* Set the MCP2515_Prescaler (pHandle,CLKPRE) */
    MCP2515_ModifyRegister(pHandle, MCP_CANCTRL, CANCTRL_CLKPRE, divisor);

    /* Turn on CLKEN */
    MCP2515_ModifyRegister(pHandle, MCP_CANCTRL, CANCTRL_CLKEN, CANCTRL_CLKEN);

    /* Turn off CLKOUT for SOF */
    MCP2515_ModifyRegister(pHandle, MCP_CNF3, CNF3_SOF, 0x00);
    return MCP_ERROR_OK;
}

void MCP2515_PrepareId(spi_mcp2515_t* pHandle, uint8_t* buffer, const bool ext, const uint32_t id)
{
    uint16_t canid = (uint16_t)(id & 0x0FFFF);

    if (ext)
    {
        buffer[MCP_EID0] = (uint8_t)(canid & 0xFF);
        buffer[MCP_EID8] = (uint8_t)(canid >> 8);
        canid            = (uint16_t)(id >> 16);
        buffer[MCP_SIDL] = (uint8_t)(canid & 0x03);
        buffer[MCP_SIDL] += (uint8_t)((canid & 0x1C) << 3);
        buffer[MCP_SIDL] |= TXB_EXIDE_MASK;
        buffer[MCP_SIDH] = (uint8_t)(canid >> 5);
    }
    else
    {
        buffer[MCP_SIDH] = (uint8_t)(canid >> 3);
        buffer[MCP_SIDL] = (uint8_t)((canid & 0x07) << 5);
        buffer[MCP_EID0] = 0;
        buffer[MCP_EID8] = 0;
    }
}

MCP_ERROR MCP2515_SetFilterMask(spi_mcp2515_t* pHandle, const MASK mask, const bool ext, const uint32_t ulData)
{
    MCP_ERROR res = MCP2515_SetConfigMode(pHandle);
    if (res != MCP_ERROR_OK)
    {
        return res;
    }

    uint8_t tbufdata[4];
    MCP2515_PrepareId(pHandle, tbufdata, ext, ulData);

    REGISTER reg;
    switch (mask)
    {
        case MASK0: reg = MCP_RXM0SIDH; break;
        case MASK1: reg = MCP_RXM1SIDH; break;
        default:
            return MCP_ERROR_FAIL;
    }

    MCP2515_SetRegisters(pHandle, reg, tbufdata, 4);

    return MCP_ERROR_OK;
}

MCP_ERROR MCP2515_SetFilter(spi_mcp2515_t* pHandle, const RXF num, const bool ext, const uint32_t ulData)
{
    MCP_ERROR res = MCP2515_SetConfigMode(pHandle);
    if (res != MCP_ERROR_OK)
    {
        return res;
    }

    REGISTER reg;

    switch (num)
    {
        case RXF0: reg = MCP_RXF0SIDH; break;
        case RXF1: reg = MCP_RXF1SIDH; break;
        case RXF2: reg = MCP_RXF2SIDH; break;
        case RXF3: reg = MCP_RXF3SIDH; break;
        case RXF4: reg = MCP_RXF4SIDH; break;
        case RXF5: reg = MCP_RXF5SIDH; break;
        default:
            return MCP_ERROR_FAIL;
    }

    uint8_t tbufdata[4];
    MCP2515_PrepareId(pHandle, tbufdata, ext, ulData);
    MCP2515_SetRegisters(pHandle, reg, tbufdata, 4);

    return MCP_ERROR_OK;
}

MCP_ERROR MCP2515_SendMessage(spi_mcp2515_t* pHandle, const TXBn txbn, const struct can_frame* frame)
{
    if (frame->can_dlc > CAN_MAX_DLEN)
    {
        return MCP_ERROR_FAILTX;
    }

    const struct TXBn_REGS* txbuf = &TXB[txbn];

    uint8_t data[13];

    bool     ext = (frame->can_id & CAN_EFF_FLAG);
    bool     rtr = (frame->can_id & CAN_RTR_FLAG);
    uint32_t id  = (frame->can_id & (ext ? CAN_EFF_MASK : CAN_SFF_MASK));

    MCP2515_PrepareId(pHandle, data, ext, id);

    data[MCP_DLC] = rtr ? (frame->can_dlc | RTR_MASK) : frame->can_dlc;

    memcpy(&data[MCP_DATA], frame->data, frame->can_dlc);

    MCP2515_SetRegisters(pHandle, txbuf->SIDH, data, 5 + frame->can_dlc);

    MCP2515_ModifyRegister(pHandle, txbuf->CTRL, TXB_TXREQ, TXB_TXREQ);

    uint8_t ctrl = MCP2515_ReadRegister(pHandle, txbuf->CTRL);
    if ((ctrl & (TXB_ABTF | TXB_MLOA | TXB_TXERR)) != 0)
    {
        return MCP_ERROR_FAILTX;
    }
    return MCP_ERROR_OK;
}

MCP_ERROR MCP2515_SendMessageEx(spi_mcp2515_t* pHandle, const struct can_frame* frame)
{
    if (frame->can_dlc > CAN_MAX_DLEN)
    {
        return MCP_ERROR_FAILTX;
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

    return MCP_ERROR_ALLTXBUSY;
}

MCP_ERROR MCP2515_ReadMessage(spi_mcp2515_t* pHandle, const RXBn rxbn, struct can_frame* frame)
{
    const struct RXBn_REGS* rxb = &RXB[rxbn];

    uint8_t tbufdata[5];

    MCP2515_ReadRegisters(pHandle, rxb->SIDH, tbufdata, 5);

    uint32_t id = (tbufdata[MCP_SIDH] << 3) + (tbufdata[MCP_SIDL] >> 5);

    if ((tbufdata[MCP_SIDL] & TXB_EXIDE_MASK) == TXB_EXIDE_MASK)
    {
        id = (id << 2) + (tbufdata[MCP_SIDL] & 0x03);
        id = (id << 8) + tbufdata[MCP_EID8];
        id = (id << 8) + tbufdata[MCP_EID0];
        id |= CAN_EFF_FLAG;
    }

    uint8_t dlc = (tbufdata[MCP_DLC] & DLC_MASK);
    if (dlc > CAN_MAX_DLEN)
    {
        return MCP_ERROR_FAIL;
    }

    uint8_t ctrl = MCP2515_ReadRegister(pHandle, rxb->CTRL);
    if (ctrl & RXBnCTRL_RTR)
    {
        id |= CAN_RTR_FLAG;
    }

    frame->can_id  = id;
    frame->can_dlc = dlc;

    MCP2515_ReadRegisters(pHandle, rxb->DATA, frame->data, dlc);

    MCP2515_ModifyRegister(pHandle, MCP_CANINTF, rxb->CANINTF_RXnIF, 0);

    return MCP_ERROR_OK;
}

MCP_ERROR MCP2515_ReadMessageEx(spi_mcp2515_t* pHandle, struct can_frame* frame)
{
    MCP_ERROR rc;
    uint8_t   stat = MCP2515_GetStatus(pHandle);

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
        rc = MCP_ERROR_NOMSG;
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

    if (eflg & EFLG_MCP_ERRORMASK)
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
    return MCP2515_ReadRegister(pHandle, MCP_EFLG);
}

void MCP2515_ClearRXnOVRFlags(spi_mcp2515_t* pHandle)
{
    MCP2515_ModifyRegister(pHandle, MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
}

uint8_t MCP2515_GetInterrupts(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP_CANINTF);
}

void MCP2515_ClearInterrupts(spi_mcp2515_t* pHandle)
{
    MCP2515_SetRegister(pHandle, MCP_CANINTF, 0);
}

uint8_t MCP2515_GetInterruptMask(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP_CANINTE);
}

void MCP2515_ClearTXInterrupts(spi_mcp2515_t* pHandle)
{
    MCP2515_ModifyRegister(pHandle, MCP_CANINTF, (CANINTF_TX0IF | CANINTF_TX1IF | CANINTF_TX2IF), 0);
}

void MCP2515_ClearRXnOVR(spi_mcp2515_t* pHandle)
{
    uint8_t eflg = MCP2515_GetErrorFlags(pHandle);
    if (eflg != 0)
    {
        MCP2515_ClearRXnOVRFlags(pHandle);
        MCP2515_ClearInterrupts(pHandle);
        // MCP2515_ModifyRegister(pHandle,MCP_CANINTF, CANINTF_ERRIF, 0);
    }
}

void MCP2515_ClearMERR(spi_mcp2515_t* pHandle)
{
    // MCP2515_ModifyRegister(pHandle,MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
    // MCP2515_ClearInterrupts(pHandle,);
    MCP2515_ModifyRegister(pHandle, MCP_CANINTF, CANINTF_MERRF, 0);
}

void MCP2515_ClearERRIF(spi_mcp2515_t* pHandle)
{
    // MCP2515_ModifyRegister(pHandle,MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
    // MCP2515_ClearInterrupts(pHandle,);
    MCP2515_ModifyRegister(pHandle, MCP_CANINTF, CANINTF_ERRIF, 0);
}

uint8_t errorCountRX(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP_REC);
}

uint8_t errorCountTX(spi_mcp2515_t* pHandle)
{
    return MCP2515_ReadRegister(pHandle, MCP_TEC);
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

#endif

    spi_mcp2515_t mcp2515 = {
        .hSPI = &spi,
    };

    SPI_Master_Init(&spi, 1000000, SPI_DUTYCYCLE_50_50, MCP2515_SPI_TIMING | SPI_FLAG_SOFT_CS);

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
    MCP2515_SetBitrate(&mcp2515, CAN_125KBPS, MCP_8MHZ);
    MCP2515_SetNormalMode(&mcp2515);

    while (1)
    {
        MCP2515_SendMessageEx(&mcp2515, &canMsg1);
      //  MCP2515_SendMessageEx(&mcp2515, &canMsg2);
        DelayBlockMs(10);
    }
}

#endif
