#!/bin/bash
[[ ! -f "$1" ]] && { 
  echo "file not foud: $1"; 
	exit 1; 
};

file=$1; sed -i ':a;N;$!ba;s/\r\n//g' $file; sed -i ':a;N;$!ba;s/\n//g' $file; while [[ -n "$(grep ' <' $file)" ]]; do sed -i 's/ </</g' $file; done