#ifndef __SPI_NRF24L01_H__
#define __SPI_NRF24L01_H__

/**
 * @brief NRF24L01 2.4GHz Wireless Transceiver
 */

#include "spimst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define NRF24L01_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_CPOL_LOW | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW | SPI_FLAG_FAST_CLOCK_ENABLE)

/**
 * Power Amplifier level.
 */
typedef enum {
    RF24_PA_MIN = 0,
    RF24_PA_LOW,
    RF24_PA_HIGH,
    RF24_PA_MAX,
    RF24_PA_ERROR,
} rf24_pa_dbm_e;

/**
 * Data rate.  How fast data moves through the air.
 */
typedef enum {
    RF24_1MBPS = 0,
    RF24_2MBPS,
    RF24_250KBPS,
} rf24_datarate_e;

/**
 * CRC Length.  How big (if any) of a CRC is included.
 */
typedef enum {
    RF24_CRC_DISABLED = 0,
    RF24_CRC_8,
    RF24_CRC_16,
} rf24_crc_len_e;

typedef struct {
    __IN spi_mst_t*  hSPI;
    __IN const pin_t CE;  // The pin attached to Chip Enable on the RF module
} spi_nrf24l01_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void NRF24L01_Init(spi_nrf24l01_t* pHandle);

/**
 * Start listening on the pipes opened for reading.
 *
 * Be sure to call NRF24L01_OpenReadingPipe() first.  Do not call NRF24L01_Write() while
 * in this mode, without first calling NRF24L01_StopListening().  Call
 * NRF24L01_IsAvailable() to check for incoming traffic, and NRF24L01_Read() to get it.
 */
void NRF24L01_StartListening(spi_nrf24l01_t* pHandle);

/**
 * Stop listening for incoming messages
 *
 * Do this before calling NRF24L01_Write().
 */
void NRF24L01_StopListening(spi_nrf24l01_t* pHandle);

/**
 * Write to the open writing pipe
 *
 * Be sure to call NRF24L01_OpenWritingPipe() first to set the destination
 * of where to write to.
 *
 * This blocks until the message is successfully acknowledged by
 * the receiver or the timeout/retransmit maxima are reached.  In
 * the current configuration, the max delay here is 60ms.
 *
 * The maximum size of data written is the fixed payload size, see
 * NRF24L01_GetPayloadSize().  However, you can write less, and the remainder
 * will just be filled with zeroes.
 *
 * @param buf Pointer to the data to be sent
 * @param len Number of bytes to be sent
 * @return True if the payload was delivered successfully false if not
 */
bool NRF24L01_Write(spi_nrf24l01_t* pHandle, const void* buf, uint8_t len);

/**
 * Read the payload
 *
 * Return the last payload received
 *
 * The size of data read is the fixed payload size, see NRF24L01_GetPayloadSize()
 *
 * @note I specifically chose 'void*' as a data type to make it easier
 * for beginners to use.  No casting needed.
 *
 * @param buf Pointer to a buffer where the data should be written
 * @param len Maximum number of bytes to read into the buffer
 * @return True if the payload was delivered successfully false if not
 */
bool NRF24L01_Read(spi_nrf24l01_t* pHandle, void* buf, uint8_t len);

/**
 * Open a pipe for writing
 *
 * Only one pipe can be open at once, but you can change the pipe
 * you'll listen to.  Do not call this while actively listening.
 * Remember to NRF24L01_StopListening() first.
 *
 * Addresses are 40-bit hex values, e.g.:
 *
 * @code
 *   NRF24L01_OpenWritingPipe(spi_nrf24l01_t* pHandle,0xF0F0F0F0F0);
 * @endcode
 *
 * @param address The 40-bit address of the pipe to open.  This can be
 * any value whatsoever, as long as you are the only one writing to it
 * and only one other radio is listening to it.  Coordinate these pipe
 * addresses amongst nodes on the network.
 */
void NRF24L01_OpenWritingPipe(spi_nrf24l01_t* pHandle, uint64_t address);

/**
 * Open a pipe for reading
 *
 * Up to 6 pipes can be open for reading at once.  Open all the
 * reading pipes, and then call NRF24L01_StartListening().
 *
 * @see openWritingPipe
 *
 * @warning Pipes 1-5 should share the first 32 bits.
 * Only the least significant byte should be unique, e.g.
 * @code
 *   NRF24L01_OpenReadingPipe(spi_nrf24l01_t* pHandle,1,0xF0F0F0F0AA);
 *   NRF24L01_OpenReadingPipe(spi_nrf24l01_t* pHandle,2,0xF0F0F0F066);
 * @endcode
 *
 * @warning Pipe 0 is also used by the writing pipe.  So if you open
 * pipe 0 for reading, and then NRF24L01_StartListening(), it will overwrite the
 * writing pipe.  Ergo, do an NRF24L01_OpenWritingPipe() again before NRF24L01_Write().
 *
 * @todo Enforce the restriction that pipes 1-5 must share the top 32 bits
 *
 * @param number Which pipe# to open, 0-5.
 * @param address The 40-bit address of the pipe to open.
 */
void NRF24L01_OpenReadingPipe(spi_nrf24l01_t* pHandle, uint8_t number, uint64_t address);

/**
 * @name Optional Configurators
 *
 *  Methods you can use to get or set the configuration of the chip.
 *  None are required.  Calling NRF24L01_Begin() sets up a reasonable set of
 *  defaults.
 */

/**
 * Set the number and delay of retries upon failed submit
 *
 * @param delay How long to wait between each retry, in multiples of 250us,
 * max is 15.  0 means 250us, 15 means 4000us.
 * @param count How many retries before giving up, max 15
 */
void NRF24L01_SetRetries(spi_nrf24l01_t* pHandle, uint8_t delay, uint8_t count);

/**
 * Set RF communication channel
 *
 * @param channel Which RF channel to communicate on, 0-127
 */
void NRF24L01_SetChannel(spi_nrf24l01_t* pHandle, uint8_t channel);

/**
 * Set Static Payload Size
 *
 * This implementation uses a pre-stablished fixed payload size for all
 * transmissions.  If this method is never called, the driver will always
 * transmit the maximum payload size (32 bytes), no matter how much
 * was sent to NRF24L01_Write().
 *
 * @todo Implement variable-sized payloads feature
 *
 * @param size The number of bytes in the payload
 */
void NRF24L01_SetPayloadSize(spi_nrf24l01_t* pHandle, uint8_t size);

/**
 * Get Static Payload Size
 *
 * @see NRF24L01_SetPayloadSize()
 *
 * @return The number of bytes in the payload
 */
uint8_t NRF24L01_GetPayloadSize(spi_nrf24l01_t* pHandle);

/**
 * Get Dynamic Payload Size
 *
 * For dynamic payloads, this pulls the size of the payload off
 * the chip
 *
 * @return Payload length of last-received dynamic payload
 */
uint8_t NRF24L01_GetDynamicPayloadSize(spi_nrf24l01_t* pHandle);

/**
 * Enable custom payloads on the acknowledge packets
 *
 * Ack payloads are a handy way to return data back to senders without
 * manually changing the radio modes on both units.
 *
 * @see examples/pingpair_pl/pingpair_pl.pde
 */
void NRF24L01_EnableAckPayload(spi_nrf24l01_t* pHandle);

/**
 * Enable dynamically-sized payloads
 *
 * This way you don't always have to send large packets just to send them
 * once in a while.  This enables dynamic payloads on ALL pipes.
 *
 * @see examples/pingpair_pl/pingpair_dyn.pde
 */
void NRF24L01_EnableDynamicPayloads(spi_nrf24l01_t* pHandle);

/**
 * Determine whether the hardware is an nRF24L01+ or not.
 *
 * @return true if the hardware is nRF24L01+ (or compatible) and false
 * if its not.
 */
bool NRF24L01_IsPVariant(spi_nrf24l01_t* pHandle);

/**
 * Enable or disable auto-acknowlede packets
 *
 * This is enabled by default, so it's only needed if you want to turn
 * it off for some reason.
 *
 * @param enable Whether to enable (true) or disable (false) auto-acks
 */
void NRF24L01_SetAutoAck(spi_nrf24l01_t* pHandle, bool enable);

/**
 * Enable or disable auto-acknowlede packets on a per pipeline basis.
 *
 * AA is enabled by default, so it's only needed if you want to turn
 * it off/on for some reason on a per pipeline basis.
 *
 * @param pipe Which pipeline to modify
 * @param enable Whether to enable (true) or disable (false) auto-acks
 */
void NRF24L01_SetAutoAckEx(spi_nrf24l01_t* pHandle, uint8_t pipe, bool enable);

/**
 * Set Power Amplifier (PA) level to one of four levels.
 * Relative mnemonics have been used to allow for future PA level
 * changes. According to 6.5 of the nRF24L01+ specification sheet,
 * they translate to: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm,
 * RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
 *
 * @param level Desired PA level.
 */
void NRF24L01_SetPALevel(spi_nrf24l01_t* pHandle, rf24_pa_dbm_e level);

/**
 * Fetches the current PA level.
 *
 * @return Returns a value from the rf24_pa_dbm_e enum describing
 * the current PA setting. Please remember, all values represented
 * by the enum mnemonics are negative dBm. See setPALevel for
 * return value descriptions.
 */
rf24_pa_dbm_e NRF24L01_GetPALevel(spi_nrf24l01_t* pHandle);

/**
 * Set the transmission data rate
 *
 * @warning setting RF24_250KBPS will fail for non-plus units
 *
 * @param speed RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
 * @return true if the change was successful
 */
bool NRF24L01_SetDataRate(spi_nrf24l01_t* pHandle, rf24_datarate_e speed);

/**
 * Fetches the transmission data rate
 *
 * @return Returns the hardware's currently configured datarate. The value
 * is one of 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS, as defined in the
 * rf24_datarate_e enum.
 */
rf24_datarate_e NRF24L01_GetDataRate(spi_nrf24l01_t* pHandle);

/**
 * Set the CRC length
 *
 * @param length RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
 */
void NRF24L01_SetCRCLength(spi_nrf24l01_t* pHandle, rf24_crc_len_e length);

/**
 * Get the CRC length
 *
 * @return RF24_DISABLED if disabled or RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
 */
rf24_crc_len_e NRF24L01_GetCRCLength(spi_nrf24l01_t* pHandle);

/**
 * Disable CRC validation
 *
 */
void NRF24L01_DisableCRC(spi_nrf24l01_t* pHandle);

/**@}*/
/**
 * @name Advanced Operation
 *
 *  Methods you can use to drive the chip in more advanced ways
 */
/**@{*/

/**
 * Print a giant block of debugging information to stdout
 *
 * @warning Does nothing if stdout is not defined.  See fdevopen in stdio.h
 */
void NRF24L01_PrintDetails(spi_nrf24l01_t* pHandle);

/**
 * Enter low-power mode
 *
 * To return to normal power mode, either NRF24L01_Write() some data or
 * startListening, or NRF24L01_PowerUp().
 */
void NRF24L01_PowerDown(spi_nrf24l01_t* pHandle);

/**
 * Leave low-power mode - making radio more responsive
 *
 * To return to low power mode, call NRF24L01_PowerDown().
 */
void NRF24L01_PowerUp(spi_nrf24l01_t* pHandle);

/**
 * Test whether there are bytes available to be read
 *
 * Use this version to discover on which pipe the message
 * arrived.
 *
 * @param[out] pipe_num Which pipe has the payload available
 * @return True if there is a payload available, false if none is
 */
bool NRF24L01_Available(spi_nrf24l01_t* pHandle, uint8_t* pipe_num);

/**
 * Non-blocking write to the open writing pipe
 *
 * Just like NRF24L01_Write(), but it returns immediately. To find out what happened
 * to the send, catch the IRQ and then call NRF24L01_WhatHappened().
 *
 * @see NRF24L01_Write()
 * @see NRF24L01_WhatHappened()
 *
 * @param buf Pointer to the data to be sent
 * @param len Number of bytes to be sent
 * @return True if the payload was delivered successfully false if not
 */
void NRF24L01_StartWrite(spi_nrf24l01_t* pHandle, const uint8_t* cpu8Data, uint8_t u8Len);

/**
 * Write an ack payload for the specified pipe
 *
 * The next time a message is received on @p pipe, the data in @p buf will
 * be sent back in the acknowledgement.
 *
 * @warning According to the data sheet, only three of these can be pending
 * at any time.  I have not tested this.
 *
 * @param pipe Which pipe# (typically 1-5) will get this response.
 * @param buf Pointer to data that is sent
 * @param len Length of the data to send, up to 32 bytes max.  Not affected
 * by the static payload set by NRF24L01_SetPayloadSize().
 */
void NRF24L01_WriteAckPayload(spi_nrf24l01_t* pHandle, uint8_t pipe, const uint8_t* cpu8Data, uint8_t u8Len);

/**
 * Determine if an ack payload was received in the most recent call to
 * NRF24L01_Write().
 *
 * Call NRF24L01_Read() to retrieve the ack payload.
 *
 * @warning Calling this function clears the internal flag which indicates
 * a payload is available.  If it returns true, you must read the packet
 * out as the very next interaction with the radio, or the results are
 * undefined.
 *
 * @return True if an ack payload is available.
 */
bool NRF24L01_IsAckPayloadAvailable(spi_nrf24l01_t* pHandle);

/**
 * Call this when you get an interrupt to find out why
 *
 * Tells you what caused the interrupt, and clears the state of
 * interrupts.
 *
 * @param[out] tx_ok The send was successful (TX_DS)
 * @param[out] tx_fail The send failed, too many retries (MAX_RT)
 * @param[out] rx_ready There is a message waiting to be read (RX_DS)
 */
void NRF24L01_WhatHappened(spi_nrf24l01_t* pHandle, bool* tx_ok, bool* tx_fail, bool* rx_ready);

/**
 * Test whether there was a carrier on the line for the
 * previous listening period.
 *
 * Useful to check for interference on the current channel.
 *
 * @return true if was carrier, false if not
 */
bool NRF24L01_TestCarrier(spi_nrf24l01_t* pHandle);

/**
 * Test whether a signal (carrier or otherwise) greater than
 * or equal to -64dBm is present on the channel. Valid only
 * on nRF24L01P (+) hardware. On nRF24L01, use NRF24L01_TestCarrier().
 *
 * Useful to check for interference on the current channel and
 * channel hopping strategies.
 *
 * @return true if signal => -64dBm, false if not
 */
bool NRF24L01_TestRPD(spi_nrf24l01_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void NRF24L01_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
