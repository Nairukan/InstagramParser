#include "request.h"

string strip(string text){
    text.resize(text.find_last_not_of(" ")+1);
    int p=text.find_first_not_of(" ");
    return text.substr(p, text.length()-p);
}

size_t Request::string_write(char *ptr, size_t size, size_t nmemb, string &str)
{
    size_t total = size * nmemb;
    if (total)
        str.append(ptr, total);

    return total;
}

size_t Request::header_write(char *ptr, size_t size, size_t nmemb,
                           pair<map<string, string>&, map<string, string>& > &metadata)
{
    string str;

    size_t total = size * nmemb;
    if (total)
        str.append(ptr, total);

    if (str.back() == '\n')
        str.resize(str.size() - 1);
    if (str.back() == '\r')
        str.resize(str.size() - 1);

    int separator=str.find(":");
    if (separator>0){
        string key=str.substr(0, separator);
        string value=str.substr(separator+2, str.length()-separator-2);
        if (key=="Set-Cookie" || key=="set-cookie"){
            //std::cout << value << "\n";
            int p1=0, p2=value.find(";", p1);
            while (p2!=-1){
                string OneCookieRAW=strip(value.substr(p1, p2-p1));
                separator=OneCookieRAW.find("=");
                if (separator!=-1){
                    key=OneCookieRAW.substr(0, separator);
                    string cookievalue=OneCookieRAW.substr(separator+1, OneCookieRAW.length()-separator-1);
                    if (metadata.second.find(key)!=metadata.second.end()){
                        if (metadata.second[key]!=cookievalue){
                            //std::cout << "Cookie changed -- " << key << "=" << metadata.second[key] << " -> " << key << "=" << cookievalue << "\n";
                        }
                    }else{
                        //std::cout << "Cookie add -- " << key << "=" << cookievalue << "\n";
                    }
                    if (cookievalue!="" && cookievalue!=" ")
                        metadata.second[key]=cookievalue;
                }
                p1=p2+1;
                p2=value.find(";", p1);
            }
            //MoodleSession=08n8jt76qmr5833fn9uf2tba11; path=/; secure
        }else{
            if (metadata.first.find(key)!=metadata.first.end()){
                if (metadata.first[key]!=value){
                    //std::cout << "  Header changed -- " << key << "=" << metadata.first[key] << " -> " << key << "=" << value << "\n";
                }else{
                    //std::cout << "  Header -- " << key << "=" << metadata.first[key] << "\n";
                }
            }else{
                //std::cout << "  Header add -- " << key << "=" << value << "\n";
            }
            if (value!="" && value!=" ")
                metadata.first[key]=value;
        }
    }
    return total;
}

Request::Request(CURL* handle, string URL,
                 map<string, string>& MapHeaders,
                 map<string, string>& MapCookies, stringstream* responce, bool isPost)
{
    std::pair<map<string, string>&, map<string, string>&> metadata({MapHeaders, MapCookies}); //Headers + Cookie
    string str;

    if (isPost)
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "POST");
    else
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(handle, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(handle, CURLOPT_DEFAULT_PROTOCOL, "https");
    //curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

    char curlErrorBuffer[CURL_ERROR_SIZE];
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, curlErrorBuffer);

    curl_slist* headers = NULL;
    for (const auto &now : MapHeaders){
        headers = curl_slist_append(headers, (now.first+": "+now.second).c_str());
    }
    stringstream cookies;
    for (const auto &now: MapCookies){
        cookies << now.first << "=" << now.second << "; ";
    }
    if (cookies.str().length())
    headers = curl_slist_append(headers, std::format("Cookie: {}", cookies.str().substr(0, cookies.str().length()-2)).c_str());

    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, string_write);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &str);
    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, header_write);
    curl_easy_setopt(handle, CURLOPT_HEADERDATA, &metadata);
    curl_easy_setopt(handle, CURLOPT_AUTOREFERER, 1L);

    if (MapHeaders.find("User-Agent")!=MapHeaders.end())
        curl_easy_setopt(handle, CURLOPT_USERAGENT, (MapHeaders)["User-Agent"].c_str());

    //curl_easy_setopt(handle, CURLOPT_MAXREDIRS , 100L);
    //curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "")
    //curl_easy_setopt(handle, CURLOPT_READDATA, )
    //curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    //curl_easy_setopt(handle, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);


#ifdef DEBUG
    std::cout << format("URL: {}\nBODY: {}\n", URL, responce->str());
    std::cout << "Headers:\n";
    auto temp=headers;
    while (temp){
        std::cout << temp->data << "\n";
        temp=temp->next;
    }
    //std::cout << responce->str();
    std::cout << "End Headers\n\n";
#endif
    string some=responce->str();
    const char* data=some.c_str();

    if (!responce->str().empty() && isPost){
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
    }

    int res=curl_easy_perform(handle);
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &res);
#ifdef DEBUG
    std::cout << res << "\n";
#endif
    if (res==0){
        //Request(handle, URL, MapHeaders, MapCookies, responce, isPost);
    }
    /*
    for (auto &h : metadata.first) {
        std::cout << h.first << ": " << h.second << "\n";
    }
    */
    int headers_size;
    curl_easy_getinfo(handle, CURLINFO_HEADER_SIZE, &headers_size);
    if(responce!=nullptr) *responce=stringstream(str);//.substr(headers_size));
    //curl_easy_getinfo(handle, CURLINFO_COOKIELIST, &str);
    //printf("Recived Cookies:\n %s\n", info_cookies);
    //curl_easy_getinfo(handle, CURLINFO_HEADER_SIZE, &res);
    //if (headers)
    //printf("Headers:\n %s\n", headers->data);
    //std::cout << "HeadersSize: " << res << "\n";
    curl_slist_free_all(headers);
    //curl_easy_cleanup(handle);

    //curl_easy_setopt(handle, CURLOPT_HEADER, hea)
}

Request::Request(CURL* handle, string URL,
                 curl_slist* headers,
                 map<string, string> MapCookies)
{
    curl_easy_setopt(handle, CURLOPT_URL, URL.c_str());

    //curl_easy_setopt(handle, CURLOPT_HEADER, hea)
}

