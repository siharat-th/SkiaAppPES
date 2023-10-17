import requests
import sys

if( len(sys.argv) != 2 ):
    print( 'no file' )
    exit()


filename = sys.argv[1]
print( 'send ' + filename )
files = {'myFile': open(filename,'rb')}
values = {}

url = "https://log2.pandahut.co/upload"

r = requests.post(url, files=files, data=values)