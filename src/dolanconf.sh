#!/usr/bin/env bash

if [ -n $1 ]
then
	prod-db --get $1 --log 2
	mac1=$(prod-db --get $1 --ni 0 --log 3)
	mac2=$(prod-db --get $1 --ni 1 --log 3)

	echo "lanconf --i210 --mac $mac1"
	echo "lanconf --i217 --mac $mac2"
else
	echo "Please enter serial number" 
fi
