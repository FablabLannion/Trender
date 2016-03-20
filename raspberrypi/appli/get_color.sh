#!/bin/bash

# title         : get_color.sh
# description   : This script is part of the Trender project. 
#                 http://fablab-lannion.org:8080/wiki/index.php?title=Trender
# author        : jerome.labidurie@orange.com, julien.jacques@nokia.com
# date          : 20160228
# version       : 0.1  
# usage         : bash get_color.sh
# notes         : lirc must be available on the platform and running correctly
# bash_version  : 4.3.30(1)-release
#
# Copyright (C) 2016 Galilabs' team

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA


DBG_IR=0
SERVER="http://galicloud.roozeec.fr/requests/get_last/murlockinthesky/jenkins"

# assuming TheCube, KEY_POWER, KEY_RED, KEY_GREEN are listed in:
# /etc/lirc/lircd.conf
#    Default lircd configuration file. It should contain all the remotes,
#    their infra-red codes and the corresponding timing and waveform details.


# send_by_IR(command, dbg_flag)
function send_by_IR() {
    command=$1
    dbg_flag=$2
    echo "command=$command"
    echo "dbg_flag=$dbg_flag"
    if [ "$dbg_flag" -ne 0 ]; then
        echo "irsend send_once TheCube $command"
    else
        irsend send_once TheCube $command
    fi
}

# trap ctrl-c and call theCubePowerDown()
trap theCube_switch_off INT

function theCube_switch_off() {
        echo "** Switch off theCube"
        send_by_IR KEY_POWER $DBG_IR
        echo -e "\nbye!\n    make sweet dreams...\n"
        exit 0
}



###
### Main section
###

send_by_IR KEY_POWER $DBG_IR

while [ 1 ]
do
    # res=( $(curl -s -w "\n%{http_code}\n" http://galicloud.roozeec.fr/requests/get_last/murlockinthesky/jenkins) )
	echo "start curl"
    res=( $(curl -s -w "\n%{http_code}\n" ${SERVER}) )
    echo "curl ok "
    last=$((${#res}-1))
    http_code=${res[$last]}

    if [ "$http_code" = 200 ]
    then
        val=${res[0]}
    else
        echo "Erreur GET: $http_code"
        exit 1
    fi

    echo "val=$val"
    if [ "$val" -le 80 ]
    then
        send_by_IR KEY_RED $DBG_IR
    elif [ "$val" -ge 90 ]
    then
        send_by_IR KEY_GREEN $DBG_IR
    else
        echo "val unknown (${val})"
    fi

    if [ "$DBG_IR" -ne 0 ]; then
        echo "take a nap..."
        sleep 5
    else
        sleep 30
    fi

done

# send_by_IR KEY_POWER $DBG_IR # <- trapped by theCube_switch_off

