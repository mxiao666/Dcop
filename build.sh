#/bin/sh
DIR_PATH=`pwd`


cd ${DIR_PATH}/3rd/EasyLogger/linux
make clean
make -j8


cd ${DIR_PATH}
make clean
make -j8


cd ${DIR_PATH}/demo
make clean
make -j8