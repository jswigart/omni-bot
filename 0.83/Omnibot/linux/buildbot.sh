#!/bin/sh

export BOOST=/boost/boost_1_55_0
export BOOST_BUILD_PATH=$BOOST/tools/build/v2
export BOOST_LIB=/usr/local/lib
export BOOST_SUFFIX=

cd /media/sf_omnibot_svn_root/Omnibot
$BOOST/bjam -q release
