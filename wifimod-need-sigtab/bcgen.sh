#!/bin/bash
export SDK_PATH=/home/hub/Desktop/ESP8266_RTOS_SDK-2.0.0/
export BIN_PATH=/home/hub/Desktop/ESP8266_RTOS_SDK-2.0.0/bin/

if [ "$1" == "clean" ];then
    make clean
else
    make BOOT=new APP=1 SPI_SPEED=40 SPI_MODE=DOUT SPI_SIZE_MAP=3
fi
