#!/bin/sh

export BOOST_BUILD_PATH=/boost/boost_1_44_0/tools/build/v2
export BOOST=/boost/boost_1_44_0

cd /mnt/omnibot_svn_root/GameInterfaces/ET/2.60/src/

bjam -a -q debug
