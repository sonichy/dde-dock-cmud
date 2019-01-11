#!/bin/bash
sudo cp `dirname $0`/libHTYCMDU.so /usr/lib/dde-dock/plugins/libHTYCMDU.so
killall dde-dock