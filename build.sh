#/bin/sh
DIR_PATH=`pwd`

cd ${DIR_PATH}/lib

make -j8

cd ${DIR_PATH}/demo

make -j8