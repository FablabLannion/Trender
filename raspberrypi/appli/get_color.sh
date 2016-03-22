#!/bin/bash

# title         : get_color.sh
# description   : This script is part of the Trender project.
#                 http://fablab-lannion.org:8080/wiki/index.php?title=Trender
# author        : jerome@labidurie.fr, julien.jacques@nokia.com
# date          : 20160228
# version       : 0.1
# usage         : bash get_color.sh <URL> [DBG_flag]
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


# assuming TheCube, KEY_POWER, KEY_RED, KEY_GREEN are listed in:
# /etc/lirc/lircd.conf
#    Default lircd configuration file. It should contain all the remotes,
#    their infra-red codes and the corresponding timing and waveform details.


# send_by_IR(command, dbg_flag)
function send_by_IR() {
    command=$1
    dbg_flag=$2
    log "command=${command}"
    log "dbg_flag=${dbg_flag}"
    if [ "$dbg_flag" -ne 0 ]; then
        log "irsend send_once TheCube ${command}"
    else
        irsend send_once TheCube $command
    fi
}

# trap ctrl-c and call theCubePowerDown()
trap theCube_switch_off INT

function theCube_switch_off() {
        log "** Switch off theCube"
        send_by_IR KEY_POWER $DBG_flag
        echo -e "\nbye!\n    make sweet dreams...\n"
        exit 0
}

# exit_message(exit_code)
function exit_message() {
    exit_code=$1
    echo "  Usage :: $0 <URL> [DBG_flag]"
    if [ "$exit_code" -ne 0 ]; then exit ${exit_code}; fi
}

# log(msg)
function log() {
    msg=$1
    if [ "$DBG_flag" -ne 0 ]; then
        echo -n "[DEBUG] "
    fi
    echo "`date +"%Y-%m-%d_%H:%M:%S"`  $msg"
}

###
### Main section
###

if [ "$#" -lt 1 ]; then
    exit_message 0
fi
SERVER=$1

DBG_flag=0
if [ "$#" -eq 2 ] && [ "$2" -ne 0 ]; then
    DBG_flag=1
elif [ "$#" -ne 1 ]; then
    exit_message 1
fi

log "SERVER=$SERVER"
log "DBG_flag=$DBG_flag"

send_by_IR KEY_POWER $DBG_flag

while [ 1 ]
do
    log "start curl"
    log "curl -s -w \"\n%{http_code}\n\" ${SERVER}"
    res=( $(curl -s -w "\n%{http_code}\n" ${SERVER}) )
    log "curl ok (ret=$?)"
    last=$((${#res}-1))
    http_code=${res[$last]}

    if [ "$http_code" = 200 ]
    then
        val=${res[0]}

        log "val=$val"
        if [ "$val" -le 80 ]
        then
            send_by_IR KEY_RED $DBG_flag
        elif [ "$val" -ge 90 ]
        then
            send_by_IR KEY_GREEN $DBG_flag
        else
            log "val unknown (${val})"
        fi

    else
        echo "GET Error: http_code=${http_code}"
        # exit 1
    fi



    if [ "$DBG_flag" -ne 0 ]; then
        log "take a nap..."
        sleep 5
    else
        sleep 60
    fi

done

# send_by_IR KEY_POWER $DBG_flag # <- trapped by theCube_switch_off

