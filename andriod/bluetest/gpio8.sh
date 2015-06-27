#!/bin/sh

echo 49 > /sys/class/gpio/export
echo 214 > /sys/class/gpio/export
echo 256 > /sys/class/gpio/export
echo low > /sys/class/gpio/gpio214/direction
echo high > /sys/class/gpio/gpio256/direction
echo high > /sys/class/gpio/gpio214/direction
echo high > /sys/class/gpio/gpio214/direction
echo out > /sys/class/gpio/gpio49/direction

#blow script use to blink GPIO8 LED
#while :
#do
# echo 1 > /sys/class/gpio/gpio49/value
# sleep 1
# echo 0 > /sys/class/gpio/gpio49/value
# sleep 1
#done
