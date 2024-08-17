/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file   fatfs.c
 * @brief  Code for fatfs applications
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
#include "fatfs.h"

uint8_t retUSER;     /* Return value for USER */
char    USERPath[4]; /* USER logical drive path */
FATFS   USERFatFS;   /* File system object for USER logical drive */
FIL     USERFile;    /* File object for USER */

/* USER CODE BEGIN Variables */
BYTE work[_MAX_SS] = {0};
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
    /*## FatFS: Link the USER driver ###########################*/
    retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

    /* USER CODE BEGIN Init */
    /* additional user code for init */
    /* USER CODE END Init */
}

/**
 * @brief  Gets Time from RTC
 * @param  None
 * @retval Time in DWORD
 */
DWORD get_fattime(void)
{
    /* USER CODE BEGIN get_fattime */
    return GetTickUs();
    /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

FRESULT CreateFile(void)
{
    FIL     file;
    FRESULT f_res = FR_OK;

    BYTE au8WriteBuffer[32] = "hello world !!\n";
    UINT bw;

    f_res = f_open(&file, "hello.txt", FA_OPEN_ALWAYS | FA_WRITE);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    f_res = f_write(&file, au8WriteBuffer, sizeof(au8WriteBuffer), &bw);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    f_res = f_close(&file);

 f_res = f_open(&file, "hello2.txt", FA_OPEN_ALWAYS | FA_WRITE);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    f_res = f_write(&file, au8WriteBuffer, sizeof(au8WriteBuffer), &bw);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    f_res = f_close(&file);
		
		
    printf("create file ok\r\n");
		
    return f_res;
}

FRESULT ReadFile(void)
{
    FIL     file;
    FRESULT f_res;

    BYTE au8ReadBuffer[32] = {0};
    UINT bw;

    f_res = f_open(&file, "hello.txt", FA_READ);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    f_res = f_read(&file, au8ReadBuffer, sizeof(au8ReadBuffer), &bw);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    printf("create file ok");
    f_res = f_close(&file);

    return f_res;
}

FRESULT InitFileSys(void)
{
    FRESULT res = FR_NO_FILESYSTEM;
	
#if 1
	
    res = f_mount(&USERFatFS, "", 1);

#endif
	
#if 0
    if (res == FR_NO_FILESYSTEM)
#else
    if (res != FR_OK)
#endif
	
    {
//			printf("Init FileSystem\r\n");
//        // No Disk file system,format disk !
//        res = f_mkfs("0:", FM_FAT, 4096, work, sizeof work);
//        if (res == FR_OK)
//        {
//            f_mount(NULL, "0:", 1);
//            res = f_mount(&USERFatFS, "0:", 1);
//        }
    }

    if (res == FR_OK)
    {
       CreateFile();
    }
		
		printf("exit\r\n");

    return res;
}

/* USER CODE END Application */
