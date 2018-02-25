#!/bin/bash
sudo cp `dirname $0`/libCMDU.so /usr/lib/dde-dock/plugins/libCMDU.so
killall dde-dock