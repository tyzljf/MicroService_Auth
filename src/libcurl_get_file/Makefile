CC=g++ -lpthread -lcurl
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
ROOT_DIR=$(shell pwd)
BIN=DCurlRest
OBJS_DIR=debug/obj
BIN_DIR=debug/bin
CUR_SOURCE=${wildcard *.cpp}
CUR_OBJS=${patsubst %.cpp, %.o, $(CUR_SOURCE)}
INCLUDES=$(ROOT_DIR)/Include/
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR INCLUDES

all:$(SUBDIRS) $(CUR_OBJS) DEBUG
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
	make -C debug
ECHO:
	@echo $(SUBDIRS)
$(CUR_OBJS):%.o:%.cpp 
	$(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ -I./DCurlHttp -I./DCurlRest -I./Include
clean:
	@rm $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)/*
