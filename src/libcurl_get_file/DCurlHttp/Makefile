SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
CUR_SOURCE=${wildcard *.cpp}
CUR_OBJS=${patsubst %.cpp, %.o, $(CUR_SOURCE)}
all:$(SUBDIRS) $(CUR_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.cpp
	$(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@  -I$(INCLUDES) -I../DCurlRest/ -I../Common/ -lpthread -lcurl
ECHO:
	@echo $(SUBDIRS)
