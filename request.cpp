#include "request.h"

Request::Request(CURL* handle, string URL,
                 map<string, string> MapHeaders,
                 map<string, string> MapCookies)
{
    curl_easy_setopt(handle, CURLOPT_URL, URL.c_str());

    char curlErrorBuffer[CURL_ERROR_SIZE];
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, curlErrorBuffer);
    curl_slist* headers = NULL;
    for (auto now : MapHeaders){
        headers = curl_slist_append(headers, (now.first+": "+now.second).c_str());
    }
    //curl_easy_setopt(handle, CURLOPT_HEADER, headers);
    stringstream cookies;
    for (auto now: MapCookies){
        cookies << now.first << "=" << now.second << "; ";
    }
    //curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookie.txt");
    char* info_cookies;
    //

    //
    int res=curl_easy_perform(handle);
    std::cout << res << " " << curlErrorBuffer << '\n';
    printf("Recived Cookies:\n %s\n", info_cookies);

    if (headers)
    printf("Headers:\n %s\n", headers->data);
    curl_easy_cleanup(handle);
    //curl_easy_setopt(handle, CURLOPT_HEADER, hea)
}

Request::Request(CURL* handle, string URL,
                 curl_slist* headers,
                 map<string, string> MapCookies)
{
    curl_easy_setopt(handle, CURLOPT_URL, URL.c_str());

    //curl_easy_setopt(handle, CURLOPT_HEADER, hea)
}

