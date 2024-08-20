编译环境：esp-idf v4.4

由于和上位机通信采用的是 USBCDC，因此不便于移植到 esp32 上。

目前已将主要功能移植到 `stm32hal_f407ve_serial2can` 上了。

