#!/usr/bin/python3

import subprocess
import datetime
import sys
import os.path

usage = '''
command usage
      time : time auto #sync time from ntp server
             time manual unixtimestamp

 bluetooth : bluetooth up / bluetooth down / bluetooth status (enabled|disabled)

      WiFi : wifi up / wifi down / wifi status (enabled|disabled) 

        4G : 4g up / 4g down / 4g status (enabled|disabled) 
'''

def main(argv,argc):
    
    try:
        
        mode = argv[0]
        
        if(argc > 1):
            command = argv[1]
            
        if(argc > 2):
            param = argv[2]

        if(mode == "usage"):
            print(usage)

        elif(mode == "time"):
            if(command=="auto"):
                syncNTP()

            if(command == "manual"):
                setTime(param)

        elif(mode == "bluetooth"):
            if(command == "up" or command == "down"):
                setBluetoothRadio(command)
            elif(command == "status"):
                getBluetoothRadioStatus()

        elif(mode == "wifi"):
            if(command == "up" or command == "down"):
                setWiFiRadio(command)
            elif(command == "status"):
                getWiFiRadioStatus()

        elif(mode == "4g"):
            if(command == "up" or command == "down"):
                set4GRadio(command)
            elif(command == "status"):
                get4GRadioStatus()
        
        
    except IndexError:
        raise SystemExit("No argruments type usage for usable commands")
    
    return 0


#subprocess.call(['sudo', 'date', '-s', '17 NOV 2021 12:59:00'], shell=False)
#sys.exit(0) 

def syncNTP():
    try:
        ret = subprocess.Popen(['sudo','ntpdate','-u','time.navy.mi.th'])
    except subprocess.CalledProcessError:
        raise sys.exit(-1) 

def setTime(timeStamp):
    try:
        ret = subprocess.call(['sudo', 'date', '+%s', '-s', '@'+timeStamp], shell=False)
    except subprocess.CalledProcessError:
        raise sys.exit(-1)

def getTime():
    try:
        ret = subprocess.check_output(['date'])
    except subprocess.CalledProcessError:
        raise sys.exit(-1)

def setBluetoothRadio(command):
    try:
        ret = subprocess.call(['sudo', 'hciconfig' , 'hci0' , command], shell=False)       
    except subprocess.CalledProcessError:
        raise sys.exit(-1)

def getBluetoothRadioStatus():
    try:
        ret = subprocess.check_output([dirname+'/bluetooth_status.sh'])
        status=ret.strip().decode('UTF-8')
        if(status == "UP"):
            print("enabled")
        elif(status == "DOWN"):
            print("disabled")
    except subprocess.CalledProcessError:
        raise sys.exit(-1)

def setWiFiRadio(command):
    try:
        if(command=="up"):
            ret = subprocess.call(['sudo', dirname+'/wifi_up.sh'], shell=False)
        elif(command=="down"):
            ret = subprocess.call(['sudo', dirname+'/wifi_down.sh'], shell=False)
    except subprocess.CalledProcessError:
        raise sys.exit(-1)

def getWiFiRadioStatus():
    try:
        ret = subprocess.check_output(['nmcli', 'radio' , 'wifi'])
        print(ret.strip().decode('UTF-8'))
    except subprocess.CalledProcessError:
        raise sys.exit(-1)

def set4GRadio(command):
    try:
        if(command=="up"):
            ret = subprocess.call(['sudo', dirname+'/4g_up.sh'], shell=False)
        elif(command=="down"):
            ret = subprocess.call(['sudo', dirname+'/4g_down.sh'], shell=False)
    except subprocess.CalledProcessError:
        raise sys.exit(-1)

def get4GRadioStatus():
    try:
        ret = subprocess.check_output(['nmcli', 'radio' , 'wwan'])
        print(ret.strip().decode('UTF-8'))
    except subprocess.CalledProcessError:
        raise sys.exit(-1)

if __name__ == '__main__':
    dirname, filename = os.path.split(os.path.abspath(__file__))
    ret=main(sys.argv[1:], len(sys.argv[1:]))
    sys.exit(ret)
