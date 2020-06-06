#/bin/sh
DIR_PATH=`pwd`


cd ${DIR_PATH}/3rd/EasyLogger/demo/os/linux

make -j8


cd ${DIR_PATH}

make -j8


cd ${DIR_PATH}/demo

make -j8