/**
  **************************************************************************
  * @file     i2c_application.c
  * @brief    the driver library of the i2c peripheral
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "i2c_application.h"

/** @addtogroup AT32F415_middlewares_i2c_application_library
  * @{
  */

/**
  * @brief  wait for the flag to be set or reset, only BUSYF flag
  *         is waiting to be reset, and other flags are waiting to be set
  * @param  hi2c: the handle points to the operation information.
  * @param  flag: specifies the flag to check.
  *         this parameter can be one of the following values:
  *         - I2C_STARTF_FLAG: start condition generation complete flag.
  *         - I2C_ADDR7F_FLAG: 0~7 bit address match flag.
  *         - I2C_TDC_FLAG: transmit data complete flag.
  *         - I2C_ADDRHF_FLAG: master 9~8 bit address header match flag.
  *         - I2C_STOPF_FLAG: stop condition generation complete flag.
  *         - I2C_RDBF_FLAG: receive data buffer full flag.
  *         - I2C_TDBE_FLAG: transmit data buffer empty flag.
  *         - I2C_BUSERR_FLAG: bus error flag.
  *         - I2C_ARLOST_FLAG: arbitration lost flag.
  *         - I2C_ACKFAIL_FLAG: acknowledge failure flag.
  *         - I2C_OUF_FLAG: overflow or underflow flag.
  *         - I2C_PECERR_FLAG: pec receive error flag.
  *         - I2C_TMOUT_FLAG: smbus timeout flag.
  *         - I2C_ALERTF_FLAG: smbus alert flag.
  *         - I2C_TRMODE_FLAG: transmission mode.
  *         - I2C_BUSYF_FLAG: bus busy flag transmission mode.
  *         - I2C_DIRF_FLAG: transmission direction flag.
  *         - I2C_GCADDRF_FLAG: general call address received flag.
  *         - I2C_DEVADDRF_FLAG: smbus device address received flag.
  *         - I2C_HOSTADDRF_FLAG: smbus host address received flag.
  *         - I2C_ADDR2_FLAG: own address 2 received flag.
  * @param  event_check: check other error flags while waiting for the flag.
  *         parameter as following values:
  *         - I2C_EVENT_CHECK_NONE
  *         - I2C_EVENT_CHECK_ACKFAIL
  *         - I2C_EVENT_CHECK_STOP
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type i2c_wait_flag(i2c_handle_type* hi2c, uint32_t flag, uint32_t event_check, uint32_t timeout)
{
  if(flag == I2C_BUSYF_FLAG)
  {
    while(i2c_flag_get(hi2c->i2cx, flag) != RESET)
    {
      /* check timeout */
      if((timeout--) == 0)
      {
        hi2c->error_code = I2C_ERR_TIMEOUT;

        return I2C_ERR_TIMEOUT;
      }
    }
  }
  else
  {
    while(i2c_flag_get(hi2c->i2cx, flag) == RESET)
    {
      /* check the ack fail flag */
      if(event_check & I2C_EVENT_CHECK_ACKFAIL)
      {
        if(i2c_flag_get(hi2c->i2cx, I2C_ACKFAIL_FLAG) != RESET)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          /* clear ack fail flag */
          i2c_flag_clear(hi2c->i2cx, I2C_ACKFAIL_FLAG);

          hi2c->error_code = I2C_ERR_ACKFAIL;

          return I2C_ERR_ACKFAIL;
        }
      }

      /* check the stop flag */
      if(event_check & I2C_EVENT_CHECK_STOP)
      {
        if(i2c_flag_get(hi2c->i2cx, I2C_STOPF_FLAG) != RESET)
        {
          /* clear stop flag */
          i2c_flag_clear(hi2c->i2cx, I2C_STOPF_FLAG);

          hi2c->error_code = I2C_ERR_STOP;

          return I2C_ERR_STOP;
        }
      }

      /* check timeout */
      if((timeout--) == 0)
      {
        hi2c->error_code = I2C_ERR_TIMEOUT;

        return I2C_ERR_TIMEOUT;
      }
    }
  }

  return I2C_OK;
}

/**
  * @brief  send address in master transmits mode.
  * @param  hi2c: the handle points to the operation information.
  * @param  address: slave address.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type i2c_master_write_addr(i2c_handle_type *hi2c, uint16_t address, uint32_t timeout)
{
  /* generate start condtion */
  i2c_start_generate(hi2c->i2cx);

  /* wait for the start flag to be set */
  if(i2c_wait_flag(hi2c, I2C_STARTF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    hi2c->error_code = I2C_ERR_START;

    return I2C_ERR_START;
  }

  if(hi2c->i2cx->oaddr1_bit.addr1mode == I2C_ADDRESS_MODE_7BIT)
  {
    /* send slave address */
    i2c_7bit_address_send(hi2c->i2cx, address, I2C_DIRECTION_TRANSMIT);
  }
  else
  {
    /* send slave 10-bit address header */
    i2c_data_send(hi2c->i2cx, (uint8_t)((address & 0x0300) >> 7) | 0xF0);

    /* wait for the addrh flag to be set */
    if(i2c_wait_flag(hi2c, I2C_ADDRHF_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
    {
      hi2c->error_code = I2C_ERR_ADDR10;

      return I2C_ERR_ADDR10;
    }

    /* send slave address */
    i2c_data_send(hi2c->i2cx, (uint8_t)(address & 0x00FF));
  }

  /* wait for the addr7 flag to be set */
  if(i2c_wait_flag(hi2c, I2C_ADDR7F_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
  {
    hi2c->error_code = I2C_ERR_ADDR;

    return I2C_ERR_ADDR;
  }

  return I2C_OK;
}

/**
  * @brief  send address in master receive mode.
  * @param  hi2c: the handle points to the operation information.
  * @param  address: slave address.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type i2c_master_read_addr(i2c_handle_type *hi2c, uint16_t address, uint32_t timeout)
{
  /* enable ack */
  i2c_ack_enable(hi2c->i2cx, TRUE);

  /* generate start condtion */
  i2c_start_generate(hi2c->i2cx);

  /* wait for the start flag to be set */
  if(i2c_wait_flag(hi2c, I2C_STARTF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    hi2c->error_code = I2C_ERR_START;

    return I2C_ERR_START;
  }

  if(hi2c->i2cx->oaddr1_bit.addr1mode == I2C_ADDRESS_MODE_7BIT)
  {
    /* send slave address */
    i2c_7bit_address_send(hi2c->i2cx, address, I2C_DIRECTION_RECEIVE);
  }
  else
  {
    /* send slave 10-bit address header */
    i2c_data_send(hi2c->i2cx, (uint8_t)((address & 0x0300) >> 7) | 0xF0);

    /* wait for the addrh flag to be set */
    if(i2c_wait_flag(hi2c, I2C_ADDRHF_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
    {
      hi2c->error_code = I2C_ERR_ADDR10;

      return I2C_ERR_ADDR10;
    }

    /* send slave address */
    i2c_data_send(hi2c->i2cx, (uint8_t)(address & 0x00FF));

    /* wait for the addr7 flag to be set */
    if(i2c_wait_flag(hi2c, I2C_ADDR7F_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
    {
      hi2c->error_code = I2C_ERR_ADDR;

      return I2C_ERR_ADDR;
    }

    /* clear addr flag */
    i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);

    /* generate restart condtion */
    i2c_start_generate(hi2c->i2cx);

    /* wait for the start flag to be set */
    if(i2c_wait_flag(hi2c, I2C_STARTF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
    {
      hi2c->error_code = I2C_ERR_START;

      return I2C_ERR_START;
    }

    /* send slave 10-bit address header */
    i2c_data_send(hi2c->i2cx, (uint8_t)((address & 0x0300) >> 7) | 0xF1);
  }

  /* wait for the addr7 flag to be set */
  if(i2c_wait_flag(hi2c, I2C_ADDR7F_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
  {
    hi2c->error_code = I2C_ERR_ADDR;

    return I2C_ERR_ADDR;
  }

  return I2C_OK;
}

/**
  * @brief  the master transmits data through polling mode.
  * @param  hi2c: the handle points to the operation information.
  * @param  address: slave address.
  * @param  pdata: data buffer.
  * @param  size: data size.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type i2c_master_transmit(i2c_handle_type* hi2c, uint16_t address, uint8_t* pdata, uint16_t size, uint32_t timeout)
{
  /* initialization parameters */
  hi2c->pbuff = pdata;
  hi2c->pcount = size;

  hi2c->error_code = I2C_OK;

  /* wait for the busy flag to be reset */
  if(i2c_wait_flag(hi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_1;
  }

  /* ack acts on the current byte */
  i2c_master_receive_ack_set(hi2c->i2cx, I2C_MASTER_ACK_CURRENT);

  /* send slave address */
  if(i2c_master_write_addr(hi2c, address, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_2;
  }

  /* clear addr flag */
  i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);

  while(size > 0)
  {
    /* wait for the tdbe flag to be set */
    if(i2c_wait_flag(hi2c, I2C_TDBE_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
    {
      /* generate stop condtion */
      i2c_stop_generate(hi2c->i2cx);

      return I2C_ERR_STEP_3;
    }

    /* write data */
    i2c_data_send(hi2c->i2cx, (*pdata++));
    size--;
  }

  /* wait for the tdc flag to be set */
  if(i2c_wait_flag(hi2c, I2C_TDC_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_4;
  }

  /* generate stop condtion */
  i2c_stop_generate(hi2c->i2cx);

  return I2C_OK;
}

/**
  * @brief  the master receive data through polling mode.
  * @param  hi2c: the handle points to the operation information.
  * @param  address: slave address.
  * @param  pdata: data buffer.
  * @param  size: data size.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type i2c_master_receive(i2c_handle_type* hi2c, uint16_t address, uint8_t* pdata, uint16_t size, uint32_t timeout)
{
  /* initialization parameters */
  hi2c->pbuff = pdata;
  hi2c->pcount = size;

  hi2c->error_code = I2C_OK;

  /* wait for the busy flag to be reset */
  if(i2c_wait_flag(hi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_1;
  }

  /* ack acts on the current byte */
  i2c_master_receive_ack_set(hi2c->i2cx, I2C_MASTER_ACK_CURRENT);

  /* enable ack */
  i2c_ack_enable(hi2c->i2cx, TRUE);

  /* send slave address */
  if(i2c_master_read_addr(hi2c, address, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_2;
  }

  if(size == 1)
  {
    /* disable ack */
    i2c_ack_enable(hi2c->i2cx, FALSE);

    /* clear addr flag */
    i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);

    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);
  }
  else if(size == 2)
  {
    /* ack acts on the next byte */
    i2c_master_receive_ack_set(hi2c->i2cx, I2C_MASTER_ACK_NEXT);

    /* disable ack */
    i2c_ack_enable(hi2c->i2cx, FALSE);

    /* clear addr flag */
    i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);
  }
  else
  {
    /* enable ack */
    i2c_ack_enable(hi2c->i2cx, TRUE);

    /* clear addr flag */
    i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);
  }

  while(size > 0)
  {
    if(size <= 3)
    {
      /* 1 byte */
      if(size == 1)
      {
        /* wait for the rdbf flag to be set */
        if(i2c_wait_flag(hi2c, I2C_RDBF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          return I2C_ERR_STEP_3;
        }

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;
      }
      /* 2 bytes */
      else if(size == 2)
      {
        /* wait for the tdc flag to be set */
        if(i2c_wait_flag(hi2c, I2C_TDC_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          return I2C_ERR_STEP_4;
        }

        /* generate stop condtion */
        i2c_stop_generate(hi2c->i2cx);

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;
      }
      /* 3 last bytes */
      else
      {
        /* wait for the tdc flag to be set */
        if(i2c_wait_flag(hi2c, I2C_TDC_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          return I2C_ERR_STEP_5;
        }

        /* disable ack */
        i2c_ack_enable(hi2c->i2cx, FALSE);

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;

        /* wait for the tdc flag to be set */
        if(i2c_wait_flag(hi2c, I2C_TDC_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          return I2C_ERR_STEP_6;
        }

        /* generate stop condtion */
        i2c_stop_generate(hi2c->i2cx);

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;
      }
    }
    else
    {
      /* wait for the rdbf flag to be set */
      if(i2c_wait_flag(hi2c, I2C_RDBF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
      {
        /* generate stop condtion */
        i2c_stop_generate(hi2c->i2cx);

        return I2C_ERR_STEP_7;
      }

      /* read data */
      (*pdata++) = i2c_data_receive(hi2c->i2cx);
      size--;
    }
  }

  return I2C_OK;
}

/**
  * @brief  send memory address.
  * @param  hi2c: the handle points to the operation information.
  * @param  mem_address_width: memory address width.
  *         this parameter can be one of the following values:
  *         - I2C_MEM_ADDR_WIDIH_8:  memory address is 8 bit
  *         - I2C_MEM_ADDR_WIDIH_16:  memory address is 16 bit
  * @param  address: memory device address.
  * @param  mem_address: memory address.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type i2c_memory_address_send(i2c_handle_type* hi2c, i2c_mem_address_width_type mem_address_width, uint16_t mem_address, int32_t timeout)
{
  i2c_status_type err_code;

  if(mem_address_width == I2C_MEM_ADDR_WIDIH_8)
  {
    /* send memory address */
    i2c_data_send(hi2c->i2cx, mem_address & 0xFF);
  }
  else
  {
    /* send memory address */
    i2c_data_send(hi2c->i2cx, (mem_address >> 8) & 0xFF);

    /* wait for the tdbe flag to be set */
    err_code = i2c_wait_flag(hi2c, I2C_TDBE_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout);

    if(err_code != I2C_OK)
    {
      /* generate stop condtion */
      i2c_stop_generate(hi2c->i2cx);

      return err_code;
    }

    /* send memory address */
    i2c_data_send(hi2c->i2cx, mem_address & 0xFF);
  }

  return I2C_OK;
}

/**
  * @brief  write data to the memory device through polling mode.
  * @param  hi2c: the handle points to the operation information.
  * @param  mem_address_width: memory address width.
  *         this parameter can be one of the following values:
  *         - I2C_MEM_ADDR_WIDIH_8:  memory address is 8 bit
  *         - I2C_MEM_ADDR_WIDIH_16:  memory address is 16 bit
  * @param  address: memory device address.
  * @param  mem_address: memory address.
  * @param  pdata: data buffer.
  * @param  size: data size.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type i2c_memory_write(i2c_handle_type* hi2c, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout)
{
  /* initialization parameters */
  hi2c->pbuff = pdata;
  hi2c->pcount = size;

  hi2c->error_code = I2C_OK;

  /* wait for the busy flag to be reset */
  if(i2c_wait_flag(hi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_1;
  }

  /* ack acts on the current byte */
  i2c_master_receive_ack_set(hi2c->i2cx, I2C_MASTER_ACK_CURRENT);

  /* send slave address */
  if(i2c_master_write_addr(hi2c, address, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_2;
  }

  /* clear addr flag */
  i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);

  /* wait for the tdbe flag to be set */
  if(i2c_wait_flag(hi2c, I2C_TDBE_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_3;
  }

  /* send memory address */
  if(i2c_memory_address_send(hi2c, mem_address_width, mem_address, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_4;
  }

  while(size > 0)
  {
    /* wait for the tdbe flag to be set */
    if(i2c_wait_flag(hi2c, I2C_TDBE_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
    {
      /* generate stop condtion */
      i2c_stop_generate(hi2c->i2cx);

      return I2C_ERR_STEP_5;
    }

    /* write data */
    i2c_data_send(hi2c->i2cx, (*pdata++));
    size--;
  }

  /* wait for the tdc flag to be set */
  if(i2c_wait_flag(hi2c, I2C_TDC_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_6;
  }

  /* generate stop condtion */
  i2c_stop_generate(hi2c->i2cx);

  return I2C_OK;
}

/**
  * @brief  read data from memory device through polling mode.
  * @param  hi2c: the handle points to the operation information.
  * @param  mem_address_width: memory address width.
  *         this parameter can be one of the following values:
  *         - I2C_MEM_ADDR_WIDIH_8:  memory address is 8 bit
  *         - I2C_MEM_ADDR_WIDIH_16:  memory address is 16 bit
  * @param  address: memory device address.
  * @param  mem_address: memory address.
  * @param  pdata: data buffer.
  * @param  size: data size.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type i2c_memory_read(i2c_handle_type* hi2c, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout)
{
  /* initialization parameters */
  hi2c->pbuff = pdata;
  hi2c->pcount = size;

  hi2c->error_code = I2C_OK;

  /* wait for the busy flag to be reset */
  if(i2c_wait_flag(hi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_1;
  }

  /* ack acts on the current byte */
  i2c_master_receive_ack_set(hi2c->i2cx, I2C_MASTER_ACK_CURRENT);

  /* enable ack */
  i2c_ack_enable(hi2c->i2cx, TRUE);

  /* send slave address */
  if(i2c_master_write_addr(hi2c, address, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_2;
  }

  /* clear addr flag */
  i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);

  /* wait for the tdbe flag to be set */
  if(i2c_wait_flag(hi2c, I2C_TDBE_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_3;
  }

  /* send memory address */
  if(i2c_memory_address_send(hi2c, mem_address_width, mem_address, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_4;
  }

  /* wait for the tdbe flag to be set */
  if(i2c_wait_flag(hi2c, I2C_TDBE_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_5;
  }

  /* send slave address */
  if(i2c_master_read_addr(hi2c, address, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);

    return I2C_ERR_STEP_6;
  }

  if(size == 1)
  {
    /* disable ack */
    i2c_ack_enable(hi2c->i2cx, FALSE);

    /* clear addr flag */
    i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);

    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);
  }
  else if(size == 2)
  {
    /* ack acts on the next byte */
    i2c_master_receive_ack_set(hi2c->i2cx, I2C_MASTER_ACK_NEXT);

    /* clear addr flag */
    i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);

    /* disable ack */
    i2c_ack_enable(hi2c->i2cx, FALSE);
  }
  else
  {
    /* enable ack */
    i2c_ack_enable(hi2c->i2cx, TRUE);

    /* clear addr flag */
    i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);
  }

  while(size > 0)
  {
    if(size <= 3)
    {
      /* 1 byte */
      if(size == 1)
      {
        /* wait for the rdbf flag to be set */
        if(i2c_wait_flag(hi2c, I2C_RDBF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          return I2C_ERR_STEP_7;
        }

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;
      }
      /* 2 bytes */
      else if(size == 2)
      {
        /* wait for the tdc flag to be set */
        if(i2c_wait_flag(hi2c, I2C_TDC_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          return I2C_ERR_STEP_8;
        }

        /* generate stop condtion */
        i2c_stop_generate(hi2c->i2cx);

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;
      }
      /* 3 last bytes */
      else
      {
        /* wait for the tdc flag to be set */
        if(i2c_wait_flag(hi2c, I2C_TDC_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          return I2C_ERR_STEP_9;
        }

        /* disable ack */
        i2c_ack_enable(hi2c->i2cx, FALSE);

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;

        /* wait for the tdc flag to be set */
        if(i2c_wait_flag(hi2c, I2C_TDC_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
        {
          /* generate stop condtion */
          i2c_stop_generate(hi2c->i2cx);

          return I2C_ERR_STEP_10;
        }

        /* generate stop condtion */
        i2c_stop_generate(hi2c->i2cx);

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;

        /* read data */
        (*pdata++) = i2c_data_receive(hi2c->i2cx);
        size--;
      }
    }
    else
    {
      /* wait for the rdbf flag to be set */
      if(i2c_wait_flag(hi2c, I2C_RDBF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
      {
        /* generate stop condtion */
        i2c_stop_generate(hi2c->i2cx);

        return I2C_ERR_STEP_11;
      }

      /* read data */
      (*pdata++) = i2c_data_receive(hi2c->i2cx);
      size--;
    }
  }

  return I2C_OK;
}

/**
  * @}
  */
