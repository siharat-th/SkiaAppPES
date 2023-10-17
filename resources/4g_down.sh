#!/bin/bash
sudo nmcli dev disconnect cdc-wdm0
sudo nmcli radio wwan off
sudo ip link set wwan0 down

