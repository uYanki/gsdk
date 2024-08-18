/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_storage_if.h
  * @version        : v1.0_Cube
  * @brief          : Header for usbd_storage_if.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_STORAGE_IF_H__
#define __USBD_STORAGE_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief For Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE USBD_STORAGE
  * @brief Header file for the usb_storage_if.c file
  * @{
  */

/** @defgroup USBD_STORAGE_Exported_Defines USBD_STORAGE_Exported_Defines
  * @brief Defines.
  * @{
  */

/* USER CODE BEGIN EXPORTED_DEFINES */

/* USER CODE END EXPORTED_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Types USBD_STORAGE_Exported_Types
  * @brief Types.
  * @{
  */

/* USER CODE BEGIN EXPORTED_TYPES */

/* USER CODE END EXPORTED_TYPES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Macros USBD_STORAGE_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* USER CODE BEGIN EXPORTED_MACRO */

/* USER CODE END EXPORTED_MACRO */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Variables USBD_STORAGE_Exported_Variables
  * @brief Public variables.
  * @{
  */

/** STORAGE Interface callback. */
extern USBD_StorageTypeDef USBD_Storage_Interface_fops_FS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_FunctionsPrototype USBD_STORAGE_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
  */

/* USER CODE BEGIN EXPORTED_FUNCTIONS */
int8_t STORAGE_Init_FS(uint8_t lun);
int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t* block_num, uint16_t* block_size);
int8_t STORAGE_IsReady_FS(uint8_t lun);
int8_t STORAGE_IsWriteProtected_FS(uint8_t lun);
int8_t STORAGE_Read_FS(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_Write_FS(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_GetMaxLun_FS(void);
/* USER CODE END EXPORTED_FUNCTIONS */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_STORAGE_IF_H__ */

