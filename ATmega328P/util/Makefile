#子目录的Makefile直接读取其子目录就行
SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
#以下同根目录下的makefile的相同代码的解释
CUR_CSOURCE=${wildcard *.c}
CUR_CPPSOURCE=${wildcard *.cpp}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_CSOURCE)}
CUR_OBJS +=${patsubst %.cpp, %.o, $(CUR_CPPSOURCE)}
all:$(SUBDIRS) $(CUR_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	$(CC) -c  $(ALL_CFLAGS)   $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@

%.o:%.cpp
	$(CC) -c  $(ALL_CXXFLAGS)   $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@
ECHO:
	@echo $(SUBDIRS)



