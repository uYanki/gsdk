# esp-idf-web-serial

This project is a Serial Monitor for esp-idf that can be accessed remotely via a web browser.   
Webpage are stored in program memory of ESP32.   

![Web-Serial](.assest/README/204442158-0e8e1b11-caa8-4937-b830-99d331ca3fa6.jpg)

I referred to [this](https://github.com/ayushsharma82/WebSerial).

另一端设备发送以换行符结尾的字符串！！！


# Software requirements
ESP-IDF V4.4/V5.0.   
ESP-IDF V5.0 is required when using ESP32-C2.   


# How to Install

- Write this sketch on Arduino Uno.   
You can use any AtMega328 microcontroller.   

```
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  while (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    Serial.println(command);
  }

  if(lastMillis + 1000 <= millis()){
    Serial.print("Hello World ");
    Serial.println(millis());
    lastMillis += 1000;
  }

  delay(1);
}
```

- Install websocket component   
I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
It's a great job.   

```
git clone https://github.com/nopnop2002/esp-idf-web-serial
cd esp-idf-web-serial/
git clone https://github.com/Molorius/esp32-websocket components/websocket
```


- Configuration of esp-idf
```
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3}
idf.py menuconfig
```
![config-top](.assest/README/164256546-da988299-c0ff-41e0-8c5a-45cdd11f9fe7.jpg)
![config-app](.assest/README/164256573-1e6fc379-699a-4464-a93d-70160fe2a0b0.jpg)


Set the information of your access point.

![config-wifi](.assest/README/164256660-c2def5c5-d524-483b-885a-fa8f32e9b471.jpg)


Set the information of your time zone.

![config-ntp](.assest/README/164256796-cf851736-2a8e-400f-b809-992aa2ff867e.jpg)


Set GPIO to use with UART.

![config-uart](.assest/README/164256738-0f59817b-0deb-41b5-a4e5-379cbe3c2574.jpg)


- Connect ESP32 and AtMega328 using wire cable   

|AtMega328||ESP32|ESP32-S2/S3|ESP32-C2/C3|
|:-:|:-:|:-:|:-:|:-:|
|TX|--|GPIO16|GPIO34|GPIO0|
|RX|--|GPIO17|GPIO35|GPIO1|
|GND|--|GND|GND|GND|

__You can change it to any pin using menuconfig.__   


- Flash firmware
```
idf.py flash monitor
```

- Launch a web browser   
Enter the following in the address bar of your web browser.   

```
http:://{IP of ESP32}/
or
http://esp32-server.local/
```

- You can communicate to Arduino-UNO

![Web-Serial](.assest/README/204442158-0e8e1b11-caa8-4937-b830-99d331ca3fa6-1724174654931.jpg)

# WEB Pages
WEB Pages are stored in the html folder.   
You cam change root.html as you like.   
