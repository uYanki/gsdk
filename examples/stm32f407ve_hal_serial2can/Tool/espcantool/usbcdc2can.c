/* USB Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// DESCRIPTION:
// This example contains minimal code to make ESP32-S2 based device
// recognizable by USB-host devices as a USB Serial Device.

#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "sdkconfig.h"
#include "driver/twai.h"
#include "nvs_flash.h"
#include "nvs.h"

//����TWAI�ӿ��շ�����
#define TX_GPIO_NUM             33
#define RX_GPIO_NUM             34

static const char *TAG = "example";
static uint8_t buf[CONFIG_USB_CDC_RX_BUFSIZE + 1];

nvs_handle_t my_handle;
esp_err_t err;

//�����д���״̬��ö��
typedef enum
{
	CMD_GET_START,
	CMD_GET_HRAD,
	CMD_GET_DATA,
	CMD_GET_CRC,
	CMD_GET_END
}CMD_SM_t;

typedef struct
{
	uint32_t RX_OK;
	uint32_t TX_COUNT;
	uint32_t TX_SUCCESS;
}TWAI_COUNT_t;

static CMD_SM_t cmd_rx_sm;

uint8_t CMD_BUFF[30];
uint8_t CMD_BUFF_index=0;

static SemaphoreHandle_t CDC_CMD_RX_OK_SEM;

uint32_t STM32_TIR_REG=0;
twai_message_t TX_MSG;

uint8_t Handshark_connected=0;

TWAI_COUNT_t twai_count;

static twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
static twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);


uint32_t BRP=8;
uint8_t TSEG1=15;
uint8_t TSEG2=4;
uint8_t MODE;

uint8_t TWAI_BUS_OFF=0;
uint8_t ERROR_CODE=0;

//void DUMP_CMD(void)
//{
//	int i=0;
//	for(i=0;i<30;i++)
//	{
//		printf("%02X ",CMD_BUFF[i]);
//	}
//	printf("\n");
//}
//
//void DUMP_TWAI_MSG(twai_message_t *msg)
//{
//	int i=0;
//	if(msg==NULL) return;
//	ESP_LOGI(TAG, "ID:%08X",msg->identifier);
//	ESP_LOGI(TAG, "LEN:%d",msg->data_length_code);
//	ESP_LOGI(TAG, "RTR:%d",msg->rtr);
//	ESP_LOGI(TAG, "EXT:%d",msg->extd);
//	for(i=0;i<8;i++)
//	{
//		printf("%02X ",msg->data[i]);
//	}
//	printf("\n");
//}


//�ۼӺ�У��
uint8_t caluate_CRC(uint8_t *data)
{
	uint8_t result=0;
	int i=0;
	for(i=0;i<28;i++)
	{
		result+=data[i];
	}
	return result;
}

//�����ʼ���
uint32_t caluate_baudrate(void)
{
	return 80000000/(BRP)/(TSEG1+TSEG2+1);
}

//���ݽ���״̬������
uint8_t CDC_CMD_RX_HANDLE(uint8_t data)
{
	switch(cmd_rx_sm)
	{
	case CMD_GET_START:
		if(data==0x55)
		{
			cmd_rx_sm=CMD_GET_HRAD;
			CMD_BUFF[0]=data;
			CMD_BUFF_index++;
		}
		break;
	case CMD_GET_HRAD:
		if(data==0xAA)
		{
			cmd_rx_sm=CMD_GET_DATA;
			CMD_BUFF[1]=data;
			CMD_BUFF_index++;
		}
		else
		{
			CMD_BUFF_index=0;
			cmd_rx_sm=CMD_GET_START;
		}
		break;
	case CMD_GET_DATA:
		CMD_BUFF[CMD_BUFF_index]=data;
		CMD_BUFF_index++;
		if(CMD_BUFF_index>27)
		{
			cmd_rx_sm=CMD_GET_CRC;
		}
		break;
	case CMD_GET_CRC:
		//�ж�У���
		if(data==caluate_CRC(CMD_BUFF))
		{
			CMD_BUFF[CMD_BUFF_index]=data;
			CMD_BUFF_index++;
			cmd_rx_sm=CMD_GET_END;
		}
		else
		{
			CMD_BUFF_index=0;
			cmd_rx_sm=CMD_GET_START;
		}
		break;
	case CMD_GET_END:
		CMD_BUFF[CMD_BUFF_index]=data;
		CMD_BUFF_index=0;
		cmd_rx_sm=CMD_GET_START;
		if(data==0x88)
		{
			//�����ɹ�
			return 1;
		}
		break;
	}
	return 0;
}

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;
    int rx_index=0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_USB_CDC_RX_BUFSIZE, &rx_size);
    if (ret == ESP_OK)
    {
    	for(rx_index=0;rx_index<rx_size;rx_index++)
    	{
    		if(CDC_CMD_RX_HANDLE(buf[rx_index])==1)
    		{
    			xSemaphoreGive(CDC_CMD_RX_OK_SEM);
    			ESP_LOGI(TAG, "CMD RX OK %02X",CMD_BUFF[2]);
//    			DUMP_CMD();
    		}
    	}
    }
    else
    {
        ESP_LOGE(TAG, "Read error");
    }
}

//��������֡���
static void CDC_CMD_RX_task(void *pvParameters)
{
	while(1)
	{
		xSemaphoreTake(CDC_CMD_RX_OK_SEM, portMAX_DELAY);
		if(Handshark_connected==0)
		{
			//һ�����յ�������һ֡���ݣ��ͱ�ʾUSB�ӿ����Ž�ͨ
			Handshark_connected=1;
		}
		//USB CDC���յ����ݴ���
		switch(CMD_BUFF[2])
		{
		case 0xFF:
			if(CMD_BUFF[3])
			{
				Handshark_connected=1;
			}
			else
			{
				Handshark_connected=0;
			}
			//����
			break;
		case 0x01:
			//���ò�����Ȼ������
		    err = nvs_open("cfg", NVS_READWRITE, &my_handle);
			MODE=CMD_BUFF[3];
			TSEG1=CMD_BUFF[4];
			TSEG2=CMD_BUFF[5];
			BRP=(CMD_BUFF[6]<<8)|(CMD_BUFF[7]<<0);
			err = nvs_set_u8(my_handle, "MODE", MODE);
			err = nvs_set_u8(my_handle, "TSEG1", TSEG1);
			err = nvs_set_u8(my_handle, "TSEG2", TSEG2);
			err = nvs_set_u32(my_handle, "BRP", BRP);
	    	err = nvs_commit(my_handle);
	        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
	        nvs_close(my_handle);
	        vTaskDelay(100);
	        esp_restart();
			break;
		case 0x02:
			//�˲�����
			if(CMD_BUFF[4])
			{
				//���ʹ�ܣ��͹ر�TWAU��Ȼ�������´�TWAI
				f_config.acceptance_code=(CMD_BUFF[5]<<24)+(CMD_BUFF[6]<<16)+(CMD_BUFF[7]<<8)+(CMD_BUFF[8]<<0);
				f_config.acceptance_mask=(CMD_BUFF[9]<<24)+(CMD_BUFF[10]<<16)+(CMD_BUFF[11]<<8)+(CMD_BUFF[12]<<0);
			}
			else
			{
				f_config.acceptance_code=0;
				f_config.acceptance_mask=0xFFFFFFFF;
			}
			ESP_ERROR_CHECK(twai_driver_uninstall());
			vTaskDelay(100);
		    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
		    ESP_ERROR_CHECK(twai_start());
			break;
		case 0x03:
			//��������
			STM32_TIR_REG = (CMD_BUFF[3]<<24)+(CMD_BUFF[4]<<16)+(CMD_BUFF[5]<<8)+(CMD_BUFF[6]<<0);
			TX_MSG.rtr = (STM32_TIR_REG >> 1) & 0x01;
			TX_MSG.extd= (STM32_TIR_REG >> 2) & 0x01;
			if(TX_MSG.extd)
			{
				TX_MSG.identifier= (STM32_TIR_REG >> 3) & 0x1FFFFFFF;
			}
			else
			{
				TX_MSG.identifier= (STM32_TIR_REG >> 21) & 0x7FF;
			}
			TX_MSG.data_length_code=CMD_BUFF[10] & 0x0F;
			TX_MSG.data[7] =CMD_BUFF[11];
			TX_MSG.data[6] =CMD_BUFF[12];
			TX_MSG.data[5] =CMD_BUFF[13];
			TX_MSG.data[4] =CMD_BUFF[14];
			TX_MSG.data[3] =CMD_BUFF[15];
			TX_MSG.data[2] =CMD_BUFF[16];
			TX_MSG.data[1] =CMD_BUFF[17];
			TX_MSG.data[0] =CMD_BUFF[18];
//			DUMP_TWAI_MSG(&TX_MSG);
			//TODO:TWAI�ӿڷ���
			twai_count.TX_COUNT++;
			//���÷��ͳ�ʱ5ms
			err=twai_transmit(&TX_MSG,5);
			if(err==ESP_OK)
			{
				//���ͳɹ�
				twai_count.TX_SUCCESS++;
			}
			break;
		case 0x04:
			//�����λ
			esp_restart();
			break;
		case 0x05:
			//�������
			memset(&twai_count,'\0',sizeof(TWAI_COUNT_t));
			break;
		}
	}
}

static void TWAI_RECOVERY_task(void *arg)
{
	uint32_t alerts;
	//����TWAI��Ҫ����ľ�������
    twai_reconfigure_alerts(0x00000080 | 0x00000800 | 0x00001000, NULL);
	while(1)
	{
		twai_read_alerts(&alerts, portMAX_DELAY);
        if (alerts & 0x00000080)
        {

            //ESP_LOGI(TAG, "Surpassed Error Warning Limit");
        }
        if (alerts & 0x00000800)
        {
            //ESP_LOGI(TAG, "Entered Error Passive state");
        }
        if (alerts & 0x00001000)
        {
        	TWAI_BUS_OFF=1;
        	//TWAI��������
            ESP_LOGI(TAG, "Bus Off state");
            //Prepare to initiate bus recovery, reconfigure alerts to detect bus recovery completion
            twai_reconfigure_alerts(0x00000020, NULL);
            twai_initiate_recovery();    //Needs 128 occurrences of bus free signal
            //ESP_LOGI(TAG, "Initiate bus recovery");
        }
        if (alerts & 0x00000020)
        {
            //Bus recovery was successful, exit control task to uninstall driver
            //ESP_LOGI(TAG, "Bus Recovered");
            //�������ñ���������ֵ
            twai_reconfigure_alerts(0x00000080 | 0x00000800 | 0x00001000, NULL);
            //�ָ�����stop״̬����Ҫ���¿���twai
            ESP_ERROR_CHECK(twai_start());
            //��������־λ
            TWAI_BUS_OFF=0;
        }
	}
}







//����CAN��Ϣ֡���
uint8_t CDC_CMD_TX_CAN_INFO_pack(uint8_t *data,twai_message_t *msg)
{
	return 1;
}

uint8_t CDC_TX_MSG(uint8_t *data,int len)
{
	int to_write = len;
	//���ͻ����Ѿ����ˣ���ֱ�ӷ���
	if(tud_cdc_n_write_available(TINYUSB_CDC_ACM_0)==0)
	{
		if(tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0,1)==ESP_ERR_TIMEOUT)
		{
			//ͬ����ʱ
			return 0;
		}
	}
	while(to_write>0)
	{
        int written = tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, data+(len - to_write), to_write);
        if(tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 5)==ESP_ERR_TIMEOUT)
        {
        	return 0;
        }
        to_write -= written;
	}
	return 1;
}

//����CAN����֡���
uint8_t CDC_CMD_TX_CAN_MSG_pack(twai_message_t *msg)
{
	uint8_t data[30];
	uint32_t STM32_CAN_RIR=0;
	if(data==NULL) return 0;
	if(msg==NULL) return 0;
	data[0]=0xAA;
	data[1]=0x55;
	data[2]=0x03;
	STM32_CAN_RIR |= (msg->rtr << 1);
	STM32_CAN_RIR |= (msg->extd << 2);
	if(msg->extd)
	{
		STM32_CAN_RIR |= (msg->identifier << 3);
	}
	else
	{
		STM32_CAN_RIR |= (msg->identifier << 21);
	}
	data[3]=(STM32_CAN_RIR>>24) & 0xFF;
	data[4]=(STM32_CAN_RIR>>16) & 0xFF;
	data[5]=(STM32_CAN_RIR>> 8) & 0xFF;
	data[6]=(STM32_CAN_RIR>> 0) & 0xFF;

	data[7]=0x00;
	data[8]=0x00;
	data[9]=0x00;
	data[10]=msg->data_length_code & 0x0F;

	data[11]=msg->data[7];
	data[12]=msg->data[6];
	data[13]=msg->data[5];
	data[14]=msg->data[4];
	data[15]=msg->data[3];
	data[16]=msg->data[2];
	data[17]=msg->data[1];
	data[18]=msg->data[0];

	data[28]=caluate_CRC(data);
	data[29]=0x88;
	if(Handshark_connected)
	{
		if(CDC_TX_MSG(data,sizeof(data))==0)
		{
			return 0;
		}
	}
	return 1;
}

uint8_t CDC_CMD_TX_INFO_pack(void)
{
	uint32_t baudrate=0;
	twai_status_info_t info;
	uint8_t data[30];
	memset(data,0x00,sizeof(data));
	if(data==NULL) return 0;
	data[0]=0xAA;
	data[1]=0x55;
	data[2]=0x01;
	//�˲���ʹ�ܸ���
	data[3]=0x00;
	data[6]=TWAI_BUS_OFF;
	data[7]=ERROR_CODE;
	if(twai_get_status_info(&info)==ESP_OK)
	{
		//���ʹ������
		data[8]=info.tx_error_counter;
		//���մ������
		data[9]=info.rx_error_counter;
	}
	//���㲨����
	baudrate=caluate_baudrate();
	data[10]=(baudrate>>16) & 0xFF ;
	data[11]=(baudrate>> 8) & 0xFF ;
	data[12]=(baudrate>> 0) & 0xFF ;
	//���ͼ���
	data[13]=(twai_count.TX_COUNT>> 24) & 0xFF ;
	data[14]=(twai_count.TX_COUNT>> 16) & 0xFF ;
	data[15]=(twai_count.TX_COUNT>>  8) & 0xFF ;
	data[16]=(twai_count.TX_COUNT>>  0) & 0xFF ;

	//���ͳɹ�����
	data[17]=(twai_count.TX_SUCCESS>> 24) & 0xFF ;
	data[18]=(twai_count.TX_SUCCESS>> 16) & 0xFF ;
	data[19]=(twai_count.TX_SUCCESS>>  8) & 0xFF ;
	data[20]=(twai_count.TX_SUCCESS>>  0) & 0xFF ;

	//����ʧ�ܼ���
	data[21]=((twai_count.TX_COUNT-twai_count.TX_SUCCESS)>> 24) & 0xFF ;
	data[22]=((twai_count.TX_COUNT-twai_count.TX_SUCCESS)>> 16) & 0xFF ;
	data[23]=((twai_count.TX_COUNT-twai_count.TX_SUCCESS)>>  8) & 0xFF ;
	data[24]=((twai_count.TX_COUNT-twai_count.TX_SUCCESS)>>  0) & 0xFF ;

	data[28]=caluate_CRC(data);
	data[29]=0x88;
	if(Handshark_connected)
	{
		if(CDC_TX_MSG(data,sizeof(data))==0)
		{
			return 0;
		}
	}
	return 1;
}

uint8_t CDC_CMD_RX_INFO_pack(void)
{
	uint8_t data[30];
	memset(data,0x00,sizeof(data));
	if(data==NULL) return 0;
	data[0]=0xAA;
	data[1]=0x55;
	data[2]=0x02;

	data[3]=((twai_count.RX_OK)>> 24) & 0xFF ;
	data[4]=((twai_count.RX_OK)>> 16) & 0xFF ;
	data[5]=((twai_count.RX_OK)>>  8) & 0xFF ;
	data[6]=((twai_count.RX_OK)>>  0) & 0xFF ;

	data[28]=caluate_CRC(data);
	data[29]=0x88;
	if(Handshark_connected)
	{
		if(CDC_TX_MSG(data,sizeof(data))==0)
		{
			return 0;
		}
	}
	return 1;
}

static void CDC_UPDATE_task(void *arg)
{
	while(1)
	{
		CDC_CMD_TX_INFO_pack();
		vTaskDelay(100);
		CDC_CMD_RX_INFO_pack();
		vTaskDelay(100);
	}
}




static void TWAI_RX_task(void *arg)
{
    twai_message_t rx_message;
    while(1)
    {
    	ESP_ERROR_CHECK(twai_receive(&rx_message, portMAX_DELAY));
    	twai_count.RX_OK++;
    	CDC_CMD_TX_CAN_MSG_pack(&rx_message);
    }
}





void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rst = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed! dtr:%d, rst:%d", dtr, rst);
}


void app_main(void)
{

	//NVS��ʼ��
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    err = nvs_open("cfg", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
    	err = nvs_get_u32(my_handle, "BRP", &BRP);
    	if(err==ESP_ERR_NVS_NOT_FOUND)
    	{
    		BRP=t_config.brp;
    		err = nvs_set_u32(my_handle, "BRP", BRP);
    		ESP_LOGE(TAG, "SET TWAI BRP TO NVS��");
    	}

    	err = nvs_get_u8(my_handle, "TSEG1", &TSEG1);
    	if(err==ESP_ERR_NVS_NOT_FOUND)
    	{
    		TSEG1=t_config.tseg_1;
    		err = nvs_set_u8(my_handle, "TSEG1", TSEG1);
    		ESP_LOGE(TAG, "SET TWAI TSEG1 TO NVS��");
    	}

    	err = nvs_get_u8(my_handle, "TSEG2", &TSEG2);
    	if(err==ESP_ERR_NVS_NOT_FOUND)
    	{
    		TSEG2=t_config.tseg_2;
    		err = nvs_set_u8(my_handle, "TSEG2", TSEG2);
    		ESP_LOGE(TAG, "SET TWAI TSEG2 TO NVS");
    	}

    	err = nvs_get_u8(my_handle, "MODE", &MODE);
    	if(err==ESP_ERR_NVS_NOT_FOUND)
    	{
    		MODE=g_config.mode;
    		err = nvs_set_u8(my_handle, "MODE", MODE);
    		ESP_LOGE(TAG, "SET TWAI MODE TO NVS��");
    	}
    	err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
		t_config.brp=BRP;
		t_config.tseg_1=TSEG1;
		t_config.tseg_2=TSEG2;
		switch(MODE)
		{
		case 1:
			g_config.mode=(TWAI_MODE_NORMAL);
			break;
		case 2:
			g_config.mode=(TWAI_MODE_NO_ACK);
			break;
		case 3:
		case 4:
			g_config.mode=(TWAI_MODE_LISTEN_ONLY);
			break;
		}
    }
    //���NVS��ȡʧ�ܣ��Ͱ���Ĭ��500K��������ʼ��������Ͱ�NVS�洢��ֵ���ò�����
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());
    ESP_LOGE(TAG, "TWAI CFG %d-%d-%d-%d",BRP,TSEG1,TSEG2,MODE);
    //���ñ�����ֵ
    twai_reconfigure_alerts(0x00000080 | 0x00000800 | 0x00001000, NULL);
	//��ʼ���������ź���
	CDC_CMD_RX_OK_SEM=xSemaphoreCreateBinary();
    //ESP_LOGI(TAG, "USB initialization");
    tinyusb_config_t tusb_cfg = {}; // the configuration using default values
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t amc_cfg =
    {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 64,
        .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&amc_cfg));
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));

    //ESP_LOGI(TAG, "USB initialization DONE");

    xTaskCreate(CDC_CMD_RX_task, "CMD_RX", 4096, NULL, 6, NULL);
    xTaskCreate(TWAI_RECOVERY_task, "TWAI_RECOVERY", 4096, NULL, 7, NULL);
    xTaskCreate(TWAI_RX_task, "TWAI_RX", 4096, NULL, 5, NULL);
    xTaskCreate(CDC_UPDATE_task, "CDC_UPDATE", 4096, NULL, 8, NULL);
}
