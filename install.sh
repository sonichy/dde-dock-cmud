#!/bin/bash

codename=$(lsb_release --codename | awk '{print$2}')
deepin="no"
if [ $codename=="stable" ]; then
    deepin="yes"
elif [ $codename=="unstable" ]; then
    deepin="yes"
else
    echo "未能确定deepin版本"
fi

if [ "$deepin" = "yes" ]; then
    sudo cp `dirname $0`/$codename/libHTYCMDU.so /usr/lib/dde-dock/plugins/libHTYCMDU.so
    killall dde-dock
fi
