#!/bin/bash

#sudo putty /dev/ttyUSB0 -log test -serial -sercfg 9600,8,e,1,N 

#tio --baudrate 9600 --databits 8 --flow none --stopbits 1 --parity even /dev/ttyUSB0

sudo chmod 666 /dev/ttyUSB0
sudo rm test

LOGFILE="test"
while [[ ! -f "$LOGFILE" ]]; do
    sleep 1
done
echo "Done init"
game=""
aimove=1
while true; do
	sleep 1
	newgame=$(sudo cat test | tail -n +2)
	if [[ ${#newgame}-${#game} -ge 4  ]]; then
		if [[ $aimove -eq 1 ]]; then
		echo -e "position startpos moves $newgame\ngo\nquit\n"
		echo -e "position startpos moves $newgame\ngo\nquit\n" > input
		output=$(./MisterQueen-master/main < input | tail -n 1 | grep -oP "^bestmove \K.*" | head -c 4)
		sudo echo -e -n "$output" > /dev/ttyUSB0
			aimove=0;
		else
			aimove=1;
		fi
		game=$newgame
	fi

done