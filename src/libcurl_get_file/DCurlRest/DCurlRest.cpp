#include <unistd.h>
#include "CThread.h"
#include "DCurlRest.h"

thread_mutex_t DCurlRest::m_mutex;

DCurlRest::DCurlRest()
{
    m_fileDir = "./";
    m_currentThreadNum = 0;
     CThread::InitLock(&m_mutex);
}

DCurlRest::~DCurlRest()
{
    CThread::DestoryLock(&m_mutex);
}

void DCurlRest::SetFileDir(const std::string& dir)
{
    m_fileDir = dir;
}

DCURL_RETURN_E DCurlRest::GetFileLength(const std::string& url, long* fileLength)
{
    double length = 0;
    
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    
    std::cout << "GetFileLength url:" << url << std::endl;
    if (CURLE_OK != curl_easy_perform(curl))
    {
        curl_easy_cleanup(curl);
        return DCURL_GET_FILELENGTH_FAILED;
    }
    
    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &length);
    *fileLength = length;

    return DCURL_OK; 
}


DCURL_RETURN_E DCurlRest::Download(const std::string& url, const std::string& localFile, uint32_t timeout)
{
    long fileLength = 0;
    DCURL_RETURN_E dRet = GetFileLength(url, &fileLength);
    if (DCURL_OK != dRet)
    {
        std::cout << "get the length of the download file failed, ret:" << dRet << std::endl;
        return dRet;
    }
    std::cout << "The length of the downlaod file is " << fileLength << std::endl;
    
    std::string filename = m_fileDir + "/" + localFile;
    FILE* fp = fopen(filename.c_str(), "wb");
    if (NULL == fp)
    {
        std::cout << "fopen " << filename << " failed." << std::endl;
        return DCURL_INTERNAL_FAILED;
    }
    
    uint8_t threadNum = 10;     //从配置文件中读取
    uint64_t blockSize = fileLength / threadNum;
    
    for (int i = 0; i <= threadNum; i++)
    {
        NodeParam *nodeParam = new NodeParam();
        if (NULL == nodeParam)
        {
            return DCURL_INTERNAL_FAILED;
        }
        
        if (i < threadNum)
        {
            nodeParam->startPos = i * blockSize;    //[startPos, endPos]
            nodeParam->endPos = (i + 1) * blockSize - 1;
        }
        else
        {
            if (fileLength % threadNum != 0)
            {
                nodeParam->startPos = i * blockSize;
                nodeParam->endPos = fileLength - 1;
            }
            else
            {
                break;
            }
        }
        
        nodeParam->fp = fp;
        nodeParam->timeout = timeout;
        nodeParam->url = url;
        
        dRet = DownloadFile(nodeParam);
        if (DCURL_OK != dRet)
        {
            std::cout << "download file failed." << std::endl;
        }
    }
    
    while (GetCurrentThreadNum() > 0)
    {
        sleep(2);
    }
    
    fclose(fp);
    
    std::cout << "download end ...." << std::endl;
    return DCURL_OK;
}

uint8_t DCurlRest::GetCurrentThreadNum()
{
    uint8_t currentThreadNum;

    {
        CAuthThreadLock threadLock(&m_mutex);
        currentThreadNum = m_currentThreadNum;
    }
    
    return currentThreadNum;
}

void DCurlRest::IncrThreadNum()
{
    CAuthThreadLock threadLock(&m_mutex);
    m_currentThreadNum++;
}

void DCurlRest::DecrThreadNum()
{
    CAuthThreadLock threadLock(&m_mutex);
    m_currentThreadNum--;
}

DCURL_RETURN_E DCurlRest::DownloadFile(NodeParam* param)
{
    return DCURL_OK;
}

size_t DCurlRest::writeData(void *buffer, size_t size, size_t nmemb, void *data)
{
    size_t written = 0;
    NodeParam* param = (NodeParam *)data;

    CAuthThreadLock threadLock(&m_mutex);
    if (param->startPos + size * nmemb <= param->endPos)
    {
        fseek(param->fp, param->startPos, SEEK_SET);
        written = fwrite(buffer, size, nmemb, param->fp);
        param->startPos += size * nmemb;
    }
    else
    {
        fseek(param->fp, param->startPos, SEEK_SET);
        written = fwrite(buffer, 1, param->endPos - param->startPos + 1, param->fp);
        written = size * nmemb;
    }
    
    return written;  
}