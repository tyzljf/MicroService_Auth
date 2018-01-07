#!/bin/bash

FCGI_SRC_VERSION=fcgi-2.4.1-SNAP-0910052249
OPEN_SRC_FCGI=fcgi
FCGI_INSTALL_DIR="/home/tanyouzhang/codebase/MicroService_Auth/third_party_soft/fcgi/"
#$(cd `dirname $0`; pwd)

function unzip_fcgi()
{
    if [ -d ${OPEN_SRC_FCGI} ]; then
        rm -rf ${OPEN_SRC_FCGI}
    fi
    
    tar -zxvf ${FCGI_SRC_VERSION}*.tar.gz > /dev/null
    mv ${FCGI_SRC_VERSION} ${OPEN_SRC_FCGI}
}

function make_fcgi()
{
    cd ${OPEN_SRC_FCGI}
    
    echo "FCGI_INSTALL_DIR:" ${FCGI_INSTALL_DIR}    
    ./configure --prefix=${FCGI_INSTALL_DIR}/lib --disable-shared
    if [ $? -ne 0 ]; then
        echo "configure fcgi failed"
        return 1
    fi
    
    sed '200s#examples##g' Makefile.in > Makefile.in.bk
    rm -rf Makefile.in
    mv Makefile.in.bk Makefile.in
        
    sed '34a #include <cstdio>' ./include/fcgio.h > ./include/fcgio.h.bak
    rm -rf ./include/fcgio.h
    mv ./include/fcgio.h.bak ./include/fcgio.h
    
    make -j8
    if [ $? -ne 0 ]; then
        echo "make fcgi failed"
        return 1
    fi

    make install
    if [ $? -ne 0 ]; then
        echo "make install fcgi failed"
        return 1    
    fi
}

unzip_fcgi

make_fcgi
if [ $? -ne 0 ]; then
    echo "make fcgi failed"
    return 1    
fi