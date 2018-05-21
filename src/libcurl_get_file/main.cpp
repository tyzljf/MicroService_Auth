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
    
    int ret = curl_global_init(CURL_GLOBAL_ALL);
    if (0 != ret)
    {
        std::cout << "curl_global_init failed." << std::endl;
        return 1;
    }
    
    //download 
    std::string url("http://www.baidu.com/index.html");
    curlRest->Download(url, "W020080822221006461534.jpg");
    
    return 0;
}
