#子目录的Makefile直接读取其子目录就行
SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')

CUR_CSOURCE=${wildcard *.c}
CUR_CPPSOURCE=${wildcard *.cpp}

CUR_COBJS := $(addsuffix .o,$(addprefix $(ROOT_DIR)/$(OBJS_DIR)/,$(basename $(notdir $(CUR_CSOURCE)))))
DEPENDS := $(addsuffix .d,$(CUR_COBJS))

all:$(SUBDIRS) $(CUR_COBJS)
$(SUBDIRS):ECHO
	make -C $@

define make-cmd-cc
$2 : $1
	$$(info CC $$<)
	$$(hide) $$(CC) $$(ALL_CFLAGS)  -Wa,-adhlns=$$(ROOT_DIR)/$$(OBJS_DIR)/$$(<:.c=.lst) -MMD -MT $$@ -MF $$@.d -c -o $$@ $$<   
endef
 
$(foreach afile,$(CUR_CSOURCE),\
    $(eval $(call make-cmd-cc,$(afile),\
        $(addsuffix .o,$(addprefix $(ROOT_DIR)/$(OBJS_DIR)/,$(basename $(notdir $(afile))))))))


ECHO:
	@echo $(SUBDIRS)


-include $(DEPENDS)

