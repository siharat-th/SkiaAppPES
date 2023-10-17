#!/bin/bash
sudo hciconfig |awk '{print $1}' | grep -e 'UP' -e 'DOWN'
