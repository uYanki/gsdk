/**
  **************************************************************************
  * @file     cdc_desc.c
  * @version  v2.0.5
  * @date     2022-02-11
  * @brief    usb cdc device descriptor
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
#include "stdio.h"
#include "usb_std.h"
#include "usbd_sdr.h"
#include "usbd_core.h"
#include "winusb_desc.h"

/** @addtogroup AT32F435_437_middlewares_usbd_class
  * @{
  */
  
/** @defgroup USB_cdc_desc
  * @brief usb device cdc descriptor
  * @{
  */  

/** @defgroup USB_cdc_desc_private_functions
  * @{
  */

static usbd_desc_t *get_device_descriptor(void);
static usbd_desc_t *get_device_qualifier(void);
static usbd_desc_t *get_device_configuration(void);
static usbd_desc_t *get_device_other_speed(void);
static usbd_desc_t *get_device_lang_id(void);
static usbd_desc_t *get_device_manufacturer_string(void);
static usbd_desc_t *get_device_product_string(void);
static usbd_desc_t *get_device_serial_string(void);
static usbd_desc_t *get_device_interface_string(void);
static usbd_desc_t *get_device_config_string(void);

static uint16_t usbd_unicode_convert(uint8_t *string, uint8_t *unicode_buf);
static void usbd_int_to_unicode (uint32_t value , uint8_t *pbuf , uint8_t len);
static void get_serial_num(void);
static uint8_t g_usbd_desc_buffer[256];


#if (USBD_SUPPORT_WINUSB==1)
static usbd_desc_t *usbd_winusbosstrdescriptor(void);
static usbd_desc_t *get_device_winusbosfeature_descriptor(void);
static usbd_desc_t *get_device_winusbosproperty_descriptor(void);
#endif // (USBD_SUPPORT_WINUSB==1)

/**
  * @brief device descriptor handler structure
  */
usbd_desc_handler cdc_desc_handler =
{
  get_device_descriptor,
  get_device_qualifier,
  get_device_configuration,
  get_device_other_speed,
  get_device_lang_id,
  get_device_manufacturer_string,
  get_device_product_string,
  get_device_serial_string,
  get_device_interface_string,
  get_device_config_string,
	
	#if (USBD_SUPPORT_WINUSB == 1)
	usbd_winusbosstrdescriptor,
	get_device_winusbosfeature_descriptor,
	get_device_winusbosproperty_descriptor,
	#endif
};

/**
  * @brief usb device standard descriptor
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_descriptor[USB_DEVICE_DESC_LEN] ALIGNED_TAIL =
{
  USB_DEVICE_DESC_LEN,                   /* bLength */
  USB_DESCIPTOR_TYPE_DEVICE,             /* bDescriptorType */
  0x00,                                  /* bcdUSB */
  0x02,
  0x00,                                  /* bDeviceClass */
  0x00,                                  /* bDeviceSubClass */
  0x00,                                  /* bDeviceProtocol */
  USB_MAX_EP0_SIZE,                      /* bMaxPacketSize */
  LBYTE(USBD_CDC_VENDOR_ID),             /* idVendor */
  HBYTE(USBD_CDC_VENDOR_ID),             /* idVendor */
  LBYTE(USBD_CDC_PRODUCT_ID),            /* idProduct */
  HBYTE(USBD_CDC_PRODUCT_ID),            /* idProduct */
  0x00,                                  /* bcdDevice rel. 2.00 */
  0x02,
  USB_MFC_STRING,                        /* Index of manufacturer string */
  USB_PRODUCT_STRING,                    /* Index of product string */
  USB_SERIAL_STRING,                     /* Index of serial number string */
  1                                      /* bNumConfigurations */
};

/**
  * @brief usb configuration standard descriptor
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_configuration[USBD_CDC_CONFIG_DESC_SIZE] ALIGNED_TAIL =
{
  USB_DEVICE_CFG_DESC_LEN,               /* bLength: configuration descriptor size */
  USB_DESCIPTOR_TYPE_CONFIGURATION,      /* bDescriptorType: configuration */
  LBYTE(USBD_CDC_CONFIG_DESC_SIZE),          /* wTotalLength: bytes returned */
  HBYTE(USBD_CDC_CONFIG_DESC_SIZE),          /* wTotalLength: bytes returned */
  0x01,                                  /* bNumInterfaces: 2 interface */
  0x01,                                  /* bConfigurationValue: configuration value */
  0x00,                                  /* iConfiguration: index of string descriptor describing
                                            the configuration */
  0xC0,                                  /* bmAttributes: self powered */
  0x32,                                  /* MaxPower 100 mA: this current is used for detecting vbus */
  
  USB_DEVICE_IF_DESC_LEN,                /* bLength: interface descriptor size */
  USB_DESCIPTOR_TYPE_INTERFACE,          /* bDescriptorType: interface descriptor type */
  0x00,                                  /* bInterfaceNumber: number of interface */
  0x00,                                  /* bAlternateSetting: alternate set */
  0x02,                                  /* bNumEndpoints: number of endpoints */
  0xff,                                  /* bInterfaceClass: CDC class code */
  0x00,                                  /* bInterfaceSubClass: subclass code, Abstract Control Model*/
  0x00,                                  /* bInterfaceProtocol: protocol code, AT Command */
  0x00,                                  /* iInterface: index of string descriptor */

  USB_DEVICE_EPT_LEN,                    /* bLength: size of endpoint descriptor in bytes */
  USB_DESCIPTOR_TYPE_ENDPOINT,           /* bDescriptorType: endpoint descriptor type */
  USBD_CDC_BULK_OUT_EPT,                 /* bEndpointAddress: the address of endpoint on usb device described by this descriptor */
  USB_EPT_DESC_BULK,                     /* bmAttributes: endpoint attributes */
  LBYTE(USBD_CDC_OUT_MAXPACKET_SIZE),        
  HBYTE(USBD_CDC_OUT_MAXPACKET_SIZE),        /* wMaxPacketSize: maximum packe size this endpoint */
  0x00,                                  /* bInterval: interval for polling endpoint for data transfers */    
  
  USB_DEVICE_EPT_LEN,                    /* bLength: size of endpoint descriptor in bytes */
  USB_DESCIPTOR_TYPE_ENDPOINT,           /* bDescriptorType: endpoint descriptor type */
  USBD_CDC_BULK_IN_EPT,                  /* bEndpointAddress: the address of endpoint on usb device described by this descriptor */
  USB_EPT_DESC_BULK,                     /* bmAttributes: endpoint attributes */
  LBYTE(USBD_CDC_IN_MAXPACKET_SIZE),
  HBYTE(USBD_CDC_IN_MAXPACKET_SIZE),         /* wMaxPacketSize: maximum packe size this endpoint */
  0x00,                                  /* bInterval: interval for polling endpoint for data transfers */   
  
             
};

#define USB_LEN_OS_FEATURE_DESC 0x28

#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif

ALIGNED_HEAD static uint8_t g_usbd_winusbosfeature[USB_LEN_OS_FEATURE_DESC] ALIGNED_TAIL =
{
	 0x28, 0, 0, 0, // length
   0, 1,          // bcd version 1.0
   4, 0,          // windex: extended compat ID descritor
   1,             // no of function
   0, 0, 0, 0, 0, 0, 0, // reserve 7 bytes
   // function
   0,             // interface no
   0,             // reserved
   'W', 'I', 'N', 'U', 'S', 'B', 0, 0, //  first ID
   0,   0,   0,   0,   0,   0, 0, 0,  // second ID
   0,   0,   0,   0,   0,   0 // reserved 6 bytes    
};

#define USB_LEN_OS_PROPERTY_DESC 0x8E
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_winusbosproperty[USB_LEN_OS_PROPERTY_DESC] ALIGNED_TAIL =
{
	0x8E, 0, 0, 0,  // length 246 byte
  0x00, 0x01,   // BCD version 1.0
  0x05, 0x00,   // Extended Property Descriptor Index(5)
  0x01, 0x00,   // number of section (1)
  //; property section        
  0x84, 0x00, 0x00, 0x00,// size of property section
  0x1, 0, 0, 0,   //; property data type (1)
  0x28, 0,        //; property name length (42)
  'D', 0,
  'e', 0,
  'v', 0,
  'i', 0,
  'c', 0,
  'e', 0,
  'I', 0,
  'n', 0,
  't', 0,
  'e', 0,
  'r', 0,
  'f', 0,
  'a', 0,
  'c', 0,
  'e', 0,
  'G', 0,
  'U', 0,
  'I', 0,
  'D', 0,
  0, 0,
  /* {13eb360b-bc1e-46cb-ac8b-ef3da47b4062} */
  0x4E, 0, 0, 0,  // ; property data length
  '{', 0,
  '1', 0,
  '3', 0,
  'E', 0,
  'B', 0,
  '3', 0,
  '6', 0,
  '0', 0,
  'B', 0,
  '-', 0,
  'B', 0,
  'C', 0,
  '1', 0,
  'E', 0,
  '-', 0,
  '4', 0,
  '6', 0,
  'C', 0,
  'B', 0,
  '-', 0,
  'A', 0,
  'C', 0,
  '8', 0,
  'B', 0,
  '-', 0,
  'E', 0,
  'F', 0,
  '3', 0,
  'D', 0,
  'A', 0,
  '4', 0,
  '7', 0,
  'B', 0,
  '4', 0,
  '0', 0,
  '6', 0,
  '2', 0,
  '}', 0,
  0,   0,
};


/**
  * @brief usb string lang id
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_string_lang_id[USBD_CDC_SIZ_STRING_LANGID] ALIGNED_TAIL =
{
  USBD_CDC_SIZ_STRING_LANGID,
  USB_DESCIPTOR_TYPE_STRING,
  0x09,
  0x04,
};

/**
  * @brief usb string serial
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_string_serial[USBD_CDC_SIZ_STRING_SERIAL] ALIGNED_TAIL =
{
  USBD_CDC_SIZ_STRING_SERIAL,
  USB_DESCIPTOR_TYPE_STRING,
};


/* device descriptor */
static usbd_desc_t device_descriptor =
{
  USB_DEVICE_DESC_LEN,
  g_usbd_descriptor
};

/* config descriptor */
static usbd_desc_t config_descriptor =
{
  USBD_CDC_CONFIG_DESC_SIZE,
  g_usbd_configuration
};

/* langid descriptor */
static usbd_desc_t langid_descriptor =
{
  USBD_CDC_SIZ_STRING_LANGID,
  g_string_lang_id
};

/* serial descriptor */
static usbd_desc_t serial_descriptor =
{
  USBD_CDC_SIZ_STRING_SERIAL,
  g_string_serial
};


#if (USBD_SUPPORT_WINUSB==1)

//static usbd_desc_t winusb_osstring_descriptor =
//{
//  0x12,
//  USBD_OS_STRING
//};

static usbd_desc_t winusb_osfeature_descriptor =
{
  USB_LEN_OS_FEATURE_DESC,
  g_usbd_winusbosfeature
};

static usbd_desc_t winusb_osproperty_descriptor =
{
  USB_LEN_OS_PROPERTY_DESC,
  g_usbd_winusbosproperty
};

#endif

static usbd_desc_t vp_desc;

/**
  * @brief  standard usb unicode convert
  * @param  string: source string
  * @param  unicode_buf: unicode buffer
  * @retval length                        
  */
static uint16_t usbd_unicode_convert(uint8_t *string, uint8_t *unicode_buf)
{
  uint16_t str_len = 0, id_pos = 2;
  uint8_t *tmp_str = string;
  
  while(*tmp_str != '\0')
  {
    str_len ++;
    unicode_buf[id_pos ++] = *tmp_str ++;
    unicode_buf[id_pos ++] = 0x00;
  }
  
  str_len = str_len * 2 + 2;
  unicode_buf[0] = (uint8_t)str_len;
  unicode_buf[1] = USB_DESCIPTOR_TYPE_STRING;
  
  return str_len;
}

/**
  * @brief  usb int convert to unicode
  * @param  value: int value
  * @param  pbus: unicode buffer
  * @param  len: length
  * @retval none                        
  */
static void usbd_int_to_unicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2 * idx] = (value >> 28) + '0';
  }
  else
  {
      pbuf[2 * idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[2 * idx + 1] = 0;
  }
}

/**
  * @brief  usb get serial number
  * @param  none
  * @retval none                        
  */
static void get_serial_num(void)
{
  uint32_t serial0, serial1, serial2;
  
  serial0 = *(uint32_t*)MCU_ID1;
  serial1 = *(uint32_t*)MCU_ID2;
  serial2 = *(uint32_t*)MCU_ID3;
  
  serial0 += serial2;
  
  if (serial0 != 0)
  {
    usbd_int_to_unicode (serial0, &g_string_serial[2] ,8);
    usbd_int_to_unicode (serial1, &g_string_serial[18] ,4);
  }
}

/**
  * @brief  get device descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_descriptor(void)
{
  return &device_descriptor;
}

/**
  * @brief  get device qualifier
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t * get_device_qualifier(void)
{
  return NULL;
}

/**
  * @brief  get config descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_configuration(void)
{
  return &config_descriptor;
}

/**
  * @brief  get other speed descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_other_speed(void)
{
  return NULL;
}

/**
  * @brief  get lang id descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_lang_id(void)
{
  return &langid_descriptor;
}


/**
  * @brief  get manufacturer descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_manufacturer_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_MANUFACTURER_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get product descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_product_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_PRODUCT_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get serial descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_serial_string(void)
{
  get_serial_num();
  return &serial_descriptor;
}

/**
  * @brief  get interface descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_interface_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_INTERFACE_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get device config descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_device_config_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_CONFIGURATION_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

#if (USBD_SUPPORT_WINUSB==1)
const uint8_t USBD_OS_STRING[8] = { 
   'M',
   'S',
   'F',
   'T',
   '1',
   '0',
   '0',
   0XA0, 
}; 

usbd_desc_t *usbd_winusbosstrdescriptor(void)
{
//  return &winusb_osstring_descriptor;
	
	vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_OS_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}


static usbd_desc_t *get_device_winusbosfeature_descriptor(void)
{
	
	return &winusb_osfeature_descriptor;

}

static usbd_desc_t *get_device_winusbosproperty_descriptor(void)
{
	return &winusb_osproperty_descriptor;
}


#endif // (USBD_SUPPORT_WINUSB==1)




/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */
