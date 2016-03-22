#!/bin/bash

# title         : cube_power_on_off.sh
# description   : This script is part of the Trender project.
#                 http://fablab-lannion.org:8080/wiki/index.php?title=Trender
# author        : jerome@labidurie.fr, julien.jacques@nokia.com
# date          : 20160228
# version       : 0.1
# usage         : bash cube_power_on_off.sh
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

# exit_message(exit_code)
function exit_message() {
    exit_code=$1
    echo "  Usage :: $0"
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

if [ "$#" -ne 0 ]; then
    exit_message 1
fi

DBG_flag=0

send_by_IR KEY_POWER $DBG_flag

