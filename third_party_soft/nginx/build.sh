#!/bin/bash

OPEN_SRC_NGINX_DIR=`pwd`
NGINX_SRC_VERSION=nginx-1.13.7

OPEN_SRC_NGINX=nginx


function unzip_nginx()
{
    if [ -d ${OPEN_SRC_NGINX} ]; then
        rm -rf ${OPEN_SRC_NGINX}
    fi
    
    tar -zxvf ${NGINX_SRC_VERSION}.tar.gz > /dev/null
    mv ${NGINX_SRC_VERSION} ${OPEN_SRC_NGINX}
}


function make_nginx()
{
    cd ${OPEN_SRC_NGINX}

    ./configure --prefix=../lib --sbin-path=../lib --without-http_rewrite_module --without-http_gzip_module
    if [ $? -ne 0 ]; then
        echo "configure nginx failed"
        return 1
    fi
    
    make -j8
    if [ $? -ne 0 ]; then
        echo "make nginx failed"
        return 1
    fi

    make install
    if [ $? -ne 0 ]; then
        echo "make install nginx failed"
        return 1    
    fi
    
    return 0
}

unzip_nginx

make_nginx
if [ $? -ne 0 ]; then
    echo "make nginx failed"
    return 1    
fi
