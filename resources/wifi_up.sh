#!/bin/bash

sudo ip link set wlan0 up 
sudo nmcli radio wifi on 
sudo nmcli dev connect wlan0

