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

class Request
{
public:
    string URL;
    Request(CURL* handle, string URL,
            map<string, string> headers,
            map<string, string> cookies);

};

#endif // REQUEST_H
