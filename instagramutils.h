#ifndef INSTAGRAMUTILS_H
#define INSTAGRAMUTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include "nlohmann/json.hpp"


using json = nlohmann::json;
using std::string;
using std::stringstream;
using std::vector;

class InstagramUtils
{
public:
    InstagramUtils() = delete;

    static bool ExtractPrimeTokens(stringstream *buffer, std::map<std::string, std::string>& cookie){
        std::string temp;
        int count = 0;
        for (int i=0; !buffer->eof() && count!=2; i++){
#ifdef DEBUG
            std::cout << i << "\n";
#endif
            std::getline((*buffer), temp);
            int k;
            if (!temp.length()) continue;
            std::string finding="\"define\"";
            int len=(std::string(finding)).length();
            //if (temp.length()-len<0)
            for (k=0; k<temp.length()-len; k++){
                if (temp.substr(k, len) == finding) break;
            }
            if (k==temp.length()-len) continue;
            int s=k;
            finding="\"instances\"";
            len=(std::string(finding)).length();
            for (k; k<temp.length()-len; k++){
                if (temp.substr(k, len) == finding) break;
            }
            if (s==k) continue;

            *buffer=std::stringstream("{"+temp.substr(s, k-s-1)+"}");
            json data = json::parse(*buffer);
            for (auto b = data.begin(); b!=data.end() && count!=2; ++b){
                for (auto now = (*b).begin(); now!=(*b).end() && count!=2; ++now)
                    if ((*now)[0]=="XIGSharedData"){
                        //extract csrf_token
                        std::stringstream ss; ss << (*now)[2];
                        std::string inter = ss.str();
                        int k;
                        std::string finding="\"raw\"";
                        int len=(std::string(finding)).length();
                        for (k=0; k<inter.length()-len; k++){
                            if (inter.substr(k, len) == finding) break;
                        }
                        finding="\"csrf_token\\\":\\\"";
                        len=(std::string(finding)).length();
                        for (k=0; k<inter.length()-len; k++){
                            if (inter.substr(k, len) == finding) break;
                        }
                        int s=k+len;
                        finding="\",\\\"viewer\\\"";
                        len=(std::string(finding)).length();
                        for (k; k<inter.length()-len; k++){
                            if (inter.substr(k, len) == finding) break;
                        }
                        inter=inter.substr(s, k-s-1);
                        cookie["X-CSRFToken"] = inter;
                        count++;
                    }else if ((*now)[0]=="RelayAPIConfigDefaults"){
                        //extract X-IG-App-ID
                        std::stringstream ss; ss << (*now)[2];
                        std::string inter = ss.str();
                        int k;
                        std::string finding="\"X-IG-App-ID\":\"";
                        int len=(std::string(finding)).length();
                        for (k=0; k<inter.length()-len; k++){
                            if (inter.substr(k, len) == finding) break;
                        }
                        int s=k+len;
                        finding=",\"X-IG-D\"";
                        len=(std::string(finding)).length();
                        for (k; k<inter.length()-len; k++){
                            if (inter.substr(k, len) == finding) break;
                        }
                        inter=inter.substr(s, k-s-1);
                        cookie["X_IG_App_ID"] = inter;
                        count++;
                    }
            }
        }
        return count==2;
    }



    static bool ExtractId_ParsingAcc(std::stringstream *buffer, std::string& id){
        json data = json::parse(*buffer);
        if (data["status"]!="ok") return false;
        try{
            id=data["data"]["user"]["id"];
            return true;
        }catch(...){
            return false;
        }
    }

    const static inline string formatData(time_t t, bool isFull=false) {
        time_t     now = t;
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), isFull ? "%Y_%m_%d %H:%M:%S" : "%Y_%m_%d", &tstruct);

        return buf;
    }


    static inline long str_to_int(string text){
        long ans=0;
        for (int i=0; i<text.length(); i++){
            ans=ans*10+(text[i]-'0');
        }
        return ans;
    }


    static bool ProcessingResponceOfParsing(std::stringstream* buffer, const time_t& startT, const time_t& endT, string& maxid, std::vector<std::vector<string>>& answer){
        json data = json::parse(*buffer);
        if (data["status"]!="ok") return false;
        try{
            time_t end_post_time=time(0);
            vector<vector<string>> vect;
            //stringstream ss;
            auto dat=data["items"];
            string f_post="not find reels", l_post="not find reels";
            int count=0;
            int coun=0;
            bool end=false;
            time_t t;
            auto media=dat.begin();
            for (; media!=dat.end(); media++){
                t=(time_t)(*media)["media"]["taken_at"];
                if (t<=endT){
                    stringstream sq;
                    l_post=InstagramUtils::formatData(t);
                    break;
                }
                coun++;
            }
            for (; media!=dat.end(); media++){
                t=(time_t)(*media)["media"]["taken_at"];
                if (t>=startT){
                    end_post_time=t;
                    string views;
                    stringstream ss;
                    //vect.push_back(formatData(t));
                    ss <<(*media)["media"]["play_count"];
                    if (ss.str()=="null"){
                        stringstream sk1;
                        sk1 << (*media)["media"]["view_count"];
                        views = sk1.str();
                    }else views=ss.str();
                    vect.push_back({views, formatData(t, true), string("https://www.instagram.com/reel/")+string((*media)["media"]["code"])+"/"});
                    count++;
                }else{
                    break;
                }
                coun++;
            }
            stringstream sq2;
            f_post=count!=0 ? formatData(end_post_time) : "not find reels";
            stringstream sq3;
            sq3 << data["paging_info"]["more_available"];
            maxid="";
            if (coun==dat.size() && sq3.str()=="true"){
                maxid=data["paging_info"]["max_id"];
            }
            //std::cout << formatData((time_t)(*dat.begin())["media"]["taken_at"]) << " " << formatData((time_t)(*dat.rbegin())["media"]["taken_at"]) << "\n";
            std::cout << coun << " = count in reponce; " << count << " = count of parsing\n";
            answer=vect;
            return true;
        }catch(...){
            return false;
        }
    }

};

#endif // INSTAGRAMUTILS_H
