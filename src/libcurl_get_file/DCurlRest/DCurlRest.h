#ifndef __DCURL_REST_H
#define __DCURL_REST_H

#include <iostream>
#include "Types.h"

class DCurlRest
{
public:
    DCurlRest();
    virtual ~DCurlRest();

public:
    /*
    @函数功能      下载文件的外部接口
    @return 
        DCURL_OK   成功
        其他       失败
    **/
    DCURL_RETURN_E Download(const std::string& url, const std::string& localFile, uint32_t timeout = 30);
    /*
    @函数功能       设置本地目录路径
    @返回值
        无
    **/
    void SetFileDir(const std::string& dir);
protected: 
    /*
    @函数功能       下载文件
    @返回值
        DCURL_OK    成功
        其他        失败
    **/
    virtual DCURL_RETURN_E DownloadFile(NodeParam* param);
    /*
    @函数功能：     获取下载长度
    @返回值
        DCURL_OK    成功
        其他        失败
    **/
    DCURL_RETURN_E GetFileLength(const std::string& url, long* fileLength);
    /*
    @函数功能：      保存文件
    @返回值
        实际保存的字节数
    **/
    static size_t writeData(void *buffer, size_t size, size_t nmemb, void *data);

    void IncrThreadNum();
    void DecrThreadNum();
    uint8_t GetCurrentThreadNum();

public:    
    static thread_mutex_t      m_mutex;
    uint8_t                    m_currentThreadNum;
    
private:
    std::string m_fileDir;
};

#endif