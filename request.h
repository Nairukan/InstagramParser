#ifndef REQUEST_H
#define REQUEST_H

#include <curl/curl.h>
#include <map>
#include <string>
#include <format>
#include <bits/stdc++.h>

using std::map;
using std::string;
using std::stringstream;
using std::pair;


static uint Request_count_max_ms=5000;

class Request
{
public:
    string URL;
    Request(CURL* , string ,
            map<string, string>& ,
            map<string, string>& , stringstream* =nullptr, bool =false); //Curl Handle, Url, headers map, cookies map, body(optional), isPost(optional)

    Request(CURL* handle, string URL,
            curl_slist* headers,
            map<string, string> cookies);

    static size_t header_write(char *ptr, size_t size, size_t nmemb,
                    pair<map<string, string>&, map<string, string>& > &metadata);


    static size_t string_write(char *ptr, size_t size, size_t nmemb, string &str);

};

#endif // REQUEST_H
