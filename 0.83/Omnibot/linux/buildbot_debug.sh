#!/bin/sh

export BOOST=/boost/boost_1_44_0
export BOOST_BUILD_PATH=$BOOST/tools/build/v2
export BOOST_LIB=$BOOST/stage/lib
export BOOST_SUFFIX=-gcc41-mt-d-1_44

cd /media/sf_omnibot_svn_root/Omnibot
$BOOST/bjam -q debug
