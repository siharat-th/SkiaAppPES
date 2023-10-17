#!/bin/bash
sudo nmcli dev disconnect wlan0
sudo nmcli radio wifi off
sudo ip link set wlan0 down

