#!/bin/bash

sudo ip link set wwan0 up 
sudo nmcli radio wwan0 on 
sudo nmcli dev connect cdc-wdm0

