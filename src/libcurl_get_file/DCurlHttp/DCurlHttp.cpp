#include "DCurlHttp.h"
#include "CThread.h"

DCurlHttp::DCurlHttp()
{
}

DCurlHttp::~DCurlHttp()
{
}

DCURL_RETURN_E DCurlHttp::DownloadFile(NodeParam* param)
{
    CURL* curl = initHttpCurl(param);
    if (NULL != curl)
    {
        std::cout << "init http curl failed." << std::endl;
        return DCURL_INIT_CURL_FAILED;
    }
    param->curl = curl;
    param->data = this;
    
    int32_t ret = CThread::CreateThread(&param->id, WorkThread, param);
    if (0 != ret)
    {
        std::cout << "create thread failed, ret:" << std::endl;
        curl_easy_cleanup(curl);
        delete param;
        return DCURL_INTERNAL_FAILED;
    }
    else
    {
        IncrThreadNum();
    }
    
    return DCURL_OK;
}



CURL* DCurlHttp::initHttpCurl(NodeParam* param)
{
    CURL* curl = curl_easy_init();
    if (NULL == curl)
    {
        std::cout << "curl_easy_init failed." << std::endl;
        return NULL;
    }   
    
    char range[64] = {0};
    snprintf(range, sizeof(range), "%lld-%lld", param->startPos, param->endPos);
    
    curl_easy_setopt(curl, CURLOPT_URL, param->url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)param);
    curl_easy_setopt(curl, CURLOPT_RANGE, range);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1L);
    
    if (param->timeout > 0)
    {
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, param->timeout);  
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, param->timeout);
    }

    return curl;
}

void* DCurlHttp::WorkThread(void* arg)
{
    NodeParam* param = (NodeParam *)arg;
    DCurlHttp* instance = (DCurlHttp* )param->data;
    
    int ret = curl_easy_perform(param->curl);
    if (0 != ret)
    {
        std::cout << "curl_easy_perform failed, ret:" << ret << std::endl;
    }

    instance->DecrThreadNum();
    curl_easy_cleanup(param->curl);
    delete param;

    return NULL;
}