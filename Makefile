##############################
# file   Makefile
# author  mlu
# date  2020-06-07
###############################
 
 
#编译指定子目录
SUBDIRS=elog os demo

define make_subdir
 @for subdir in $(SUBDIRS) ; do \
 ( cd $$subdir && make $1) \
 done;
endef

all:
	$(call make_subdir , all)
 
install :
	$(call make_subdir , install)
 
debug:
	$(call make_subdir , debug)
clean:
	$(call make_subdir , clean) 