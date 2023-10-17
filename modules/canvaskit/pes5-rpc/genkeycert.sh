#!/usr/bin/env bash

CWD=`pwd`

SCRIPT_PATH="$(realpath $( dirname ${BASH_SOURCE[0]} ))"
echo "SCRIPT_PATH: \"$SCRIPT_PATH\""


#dircert=cert
dircert=.
filekey=key.pem
filecsr=csr.pem
filecert=cert.pem

pathcert="$SCRIPT_PATH/$dircert"

[ ! -d "$pathcert" ] && {
  echo "mkdir: $dircert"
  mkdir -p "$pathcert"
}

echo "check: $filekey"
[ ! -f "$pathcert/$filekey" ] && {
  echo "gen: $filekey"
  openssl genrsa -out "$pathcert/$filekey"
}

echo "check: $filecsr"
[ ! -f "$pathcert/$filecsr" ] && {
  echo "gen: $filecsr"
  openssl req -new -key "$pathcert/$filekey" -out "$pathcert/$filecsr"
}

echo "check: $filecert"
[ ! -f "$pathcert/$filecert" ] && {
  echo "gen: $filecert"
  openssl x509 -req -days 365 -in "$pathcert/$filecsr" -signkey "$pathcert/$filekey" -out "$pathcert/$filecert"
}

cd $CWD