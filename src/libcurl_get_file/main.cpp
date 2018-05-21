#include "DCurlHttp.h"
#include "DCurlRest.h"

int main()
{
    DCurlRest *curlRest = new DCurlHttp();
    if(NULL == curlRest)
    {
        std::cout << "new DCurlHttp failed." << std::endl;
        return 1;
    }
    
    //多线程环境下，先调用curl_global_init
    int ret = curl_global_init(CURL_GLOBAL_ALL);
    if (0 != ret)
    {
        std::cout << "curl_global_init failed." << std::endl;
        return 1;
    }
    
    //下载
    std::string url("http://ardownload.adobe.com/pub/adobe/reader/win/11.x/11.0.01/en_US/AdbeRdr11001_en_US.exe");
    curlRest->Download(url, "AdbeRdr11001_en_US.exe");
    
    return 0;
}
