#!/bin/bash
SRCPATH="../src/"
HEADPATH="../include/"
rm -f test_log test_pthread_log ultimate_test
g++ -o test_pthread_log  "$SRCPATH"*.c  thread_test.c  \
-lpthread -I "$HEADPATH"  -rdynamic -Wno-write-strings  -g  -D_SW_VER_ #-D_DEBUG_

if ls *.access >/dev/null 2>/dev/null  
then
	rm *.access
fi
if ls *.error >/dev/null 2>/dev/null  
then 
	rm *.error
fi
if ls *fuse_hidden* >/dev/null 2>/dev/null  
then 
	rm *fuse_hidden*
fi

