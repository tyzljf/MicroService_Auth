#ifndef __DCURL_HTTP_H
#define __DCURL_HTTP_H

#include "DCurlRest.h"

class DCurlHttp: public DCurlRest
{
public:
    DCurlHttp();
    virtual ~DCurlHttp();
    
public:
    /*
    @函数功能   重载基类的下载文件方法
    @返回值
        DCURL_OK    成功
        其他        失败
    **/
    virtual DCURL_RETURN_E DownloadFile(NodeParam* param);
    /*
    @函数功能：  线程处理函数    
    @返回值
        
    **/
    static void* WorkThread(void* arg);
    
private:
    /*
    @函数功能：  初始化HTTP协议需要的curl信息
    @返回值
        NULL     初始化失败
        非空     curl句柄
    */
    CURL* initHttpCurl(NodeParam* param);
};

#endif