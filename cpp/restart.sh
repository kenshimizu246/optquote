#!/usr/bin/bash

TM=`date "+%Y%m%d-%H%M%S"`

echo $TM

mv test_201* .bkup
mv core.* .bkup
cp a.txt .bkup/a.txt.$TM

ps -ef | grep 'OptQuoteServer' | egrep -v 'grep'

if [ -e /tmp/optquoteserver.pid ]; then
	echo "optquoteserver.pid:" 
	cat /tmp/optquoteserver.pid 
	cat /tmp/optquoteserver.pid | xargs kill -HUP
else
	if [ -e /tmp/optquoteserver.pid ]; then
		rm /tmp/optquoteserver.pid;
	fi
fi

./OptQuoteServer resources/config.json

