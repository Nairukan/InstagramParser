#ifndef INSTAGRAMUTILS_H
#define INSTAGRAMUTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <format>
#include "nlohmann/json.hpp"


using json = nlohmann::json;
using std::string;
using std::stringstream;
using std::vector;

class InstagramUtils
{
public:
    inline static std::map<string, unsigned int> subsribers={};
    InstagramUtils() = delete;

    static bool ExtractPrimeTokens(stringstream *buffer, std::map<std::string, std::string>& headers){
        std::string temp;
        int count = 0;
        for (int i=0; !buffer->eof() && count!=3; i++){
#ifdef DEBUG
            std::cout << i << "\n";
#endif
            std::getline((*buffer), temp);
            int k;
            if (!temp.length()) continue;

            //find X-Instagram-AJAX
            std::string finding="\"consistency\":{\"rev\":";
            int len=(std::string(finding)).length();
            if ((headers["X-Instagram-AJAX"]=="" || headers["X-Instagram-AJAX"]==" ")){
                for (k=0; k<temp.length()-len; k++){
                    if (temp.substr(k, len) == finding) break;
                }
                if (k!=temp.length()-len){
                    int s=k+len;
                    finding="}";
                    len=(std::string(finding)).length();
                    for (k; k<temp.length()-len; k++){
                        if (temp.substr(k, len) == finding) break;
                    }
                    headers["X-Instagram-AJAX"]=temp.substr(s, k-s);
#ifdef DEBUG
                    std::cout <<  i+1 << " X-Instagram-AJAX -- FOUND\n" << headers["X-Instagram-AJAX"] << "\n";
#endif
                    ++count;
                    continue;
                }
            }

            //find XIGSharedData && csrf_token
            finding="\"define\"";
            len=(std::string(finding)).length();
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
                for (auto now = (*b).begin(); now!=(*b).end() && count!=3; ++now){
                    auto root=(*now)[0];
                    if (root=="PolarisSiteData"){
                        std::cout << "COUNTRY: " << (*now)[2] << "\n"; std::cout.flush();
                    }else
                    if (root=="PolarisSecurityConfig" || root=="InstagramSecurityConfig"){

                        if ((headers["X-CSRFToken"]=="" || headers["X-CSRFToken"]==" ")){
                            //extract csrf_token
                            std::stringstream ss; ss << (*now)[2];
                            std::string inter = ss.str();
                            //std::cout << inter << "\n";
                            int k;
                            std::string finding="\"raw\"";
                            finding="\"csrf_token\":\"";
                            len=(std::string(finding)).length();
                            for (k=0; k<inter.length()-len; k++){
                                if (inter.substr(k, len) == finding) break;
                            }
                            int s=k+len;
                            finding="}";
                            len=(std::string(finding)).length();
                            for (k; k<inter.length()-len; k++){
                                if (inter.substr(k, len) == finding) break;
                            }
                            inter=inter.substr(s, k-s-1);
                            headers["X-CSRFToken"] = inter;
#ifdef DEBUG
                            std::cout << i+1 << " X-CSRFToken -- FOUND\n" << headers["X-CSRFToken"] << "\n";
#endif
                            ++count;
                        }
#ifdef DEBUG
                        else{
                            std::cout << std::format("Extract CSRF skipped - couse of headers[\"X-CSRFToken\"]=\"{}\"\n", headers["X-CSRFToken"]);
                        }
#endif
                    }else if ((headers["X_IG_App_ID"]=="" || headers["X_IG_App_ID"]==" ") && (*now)[0]=="RelayAPIConfigDefaults"){
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
                        headers["X_IG_App_ID"] = inter;
#ifdef DEBUG
                        std::cout <<  i+1 << " X_IG_App_ID -- FOUND\n" << headers["X_IG_App_ID"] << "\n";
#endif
                        ++count;
                    }
                }
            }
        }
        std::cout << count << " - count\n"; std::cout.flush();
        return count>=3;
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

    static bool ExtractSubscribe_ParsingAcc(std::stringstream *buffer, unsigned int& subscribe_out){
        json data = json::parse(*buffer);
        if (data["status"]!="ok") return false;
        try{
            subscribe_out=data["data"]["user"]["edge_followed_by"]["count"];
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

    static inline string uint_to_str(uint num){
        string answer="";
        if (!num) return "0";
        while(num){
            answer=char(num%10+'0')+answer;
            num/=10;
        }
        return answer;
    }

    static std::string _fmt;

    static uint ProcessingResponceOfParsing(std::stringstream* buffer, const time_t& startT, const time_t& endT, string& maxid, std::vector<std::vector<string>>& answer, uint& counter, string& _fmt, std::map<std::string, int> & ignor, const string current_author){

        json data;
        try{
            data= json::parse(*buffer);
            if (data["status"]!="ok") return false;
        }catch(...){
            std::cout << "error of parsing responce\n";
            return 1;
        }
        *buffer=stringstream();
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
            std::cout << data["num_results"] << "\n";
            auto media=dat.begin();

            for (; media!=dat.end(); media++){
#ifdef Reels
                t=(time_t)(*media)["media"]["taken_at"];
#elif Posts
                t=(time_t)(*media)["taken_at"];
                //std::cout << std::format("t={} endT={}\n", InstagramUtils::formatData(t), InstagramUtils::formatData(endT));
                stringstream sk2; sk2 << (*media)["timeline_pinned_user_ids"];
                if (sk2.str()!="null"){
                    if (t<=endT && t>=startT){
                        string views;
                        stringstream ss;
                        string _counter=uint_to_str(++counter);
                        string _data=formatData(t, true); //Поменть на true для полной даты в рилсах
                        string _link=string("https://www.instagram.com/p/")+string((*media)["code"])+"/";
                        stringstream sk1; sk1 << (*media)["like_count"];
                        string _likes=sk1.str();
                        vect.push_back({_data, _link,  _likes, _counter});
                    }["coauthor_producers"]
                    coun++;
                    continue;
                }
#endif
                if (t<=endT){
                    stringstream sq;
                    l_post=InstagramUtils::formatData(t);
                    break;
                }
                coun++;
            }
            for (; media!=dat.end(); media++){
#ifdef Reels
                t=(time_t)(*media)["media"]["taken_at"];
#elif Posts
                t=(time_t)(*media)["taken_at"];
#endif
                if (t>=startT){
                    end_post_time=t;
                    string views;
                    stringstream ss;
                    //vect.push_back(formatData(t));
/*
                    string current_auth=(*media)["media"]["user"]["username"];
                    vector<string> couathors;
                    auto couath = (*media)["media"]["coauthor_producers"];
                    uint local_sum=InstagramUtils::subsribers[current_auth];
                    uint total_sum=local_sum;
                    for (auto coauthor : couath){
                        if (coauthor["username"]!=current_author) couathors.push_back(coauthor["username"]);
                        if (!InstagramUtils::subsribers.contains(coauthor["username"])) std::cout << "Unknown coauthor - " << coauthor["username"] << "\n";
                        total_sum+=InstagramUtils::subsribers[coauthor["username"]];
                    }
                    if (current_auth!=current_author) couathors.push_back(current_auth);
                    local_sum=InstagramUtils::subsribers[current_author];
*/
                    double coef=1;//double(local_sum)/total_sum;
/*
                    std::cout << "author subs: " << local_sum << "\n" << "coauthors: ";
                    for (auto cou : couathors){
                       std::cout << cou << ", ";
                    }
                    std::cout << "\ntotal_sum=" << total_sum << "; coef=" << coef << std::endl;
*/
                    string _counter=uint_to_str(++counter);
                    string _data=formatData(t, true); //Поменть на true для полной даты в рилсах
#ifdef Reels
                    if ((*media)["media"]["play_count"].is_number()){
                        double view=int((*media)["media"]["play_count"])*coef;
                        ss << (view-int(view)<0.49 ? int(view) : int(view)+1);
                        views=ss.str();
                    }else{
                        stringstream sk1;
                        if ((*media)["media"]["view_count"].is_number()){
                            double view=int((*media)["media"]["view_count"])*coef;
                            ss << (view-int(view)<0.49 ? int(view) : int(view)+1);
                        }else{
                            ss << "null";
                        }
                        views = ss.str();
                    }
                    double like=int((*media)["media"]["like_count"])*coef;
                    stringstream sk1; sk1 << (like-int(like)<0.49 ? int(like) : int(like)+1);
                    string _likes=sk1.str();
                    string _link=string("https://www.instagram.com/reel/")+string((*media)["media"]["code"])+"/";

#elif Posts
                    string _link=string("https://www.instagram.com/p/")+string((*media)["code"])+"/";

                    stringstream sk1; sk1 << (*media)["like_count"];
                    string _likes=sk1.str();

#endif
                    //DlVLC
                    /* D - Full format Data
                     * d - Short format Data
                     * l - Link
                     * V - Count of Views
                     * L - Count of Likes
                     * C - Counter
                     */
#ifdef Reels
                    vector<string> tmp;
                    for(auto elem: _fmt){
                        switch (elem) {
                        case 'D':
                            tmp.push_back(formatData(t, true));
                            break;
                        case 'd':
                            tmp.push_back(formatData(t, false));
                            break;
                        case 'l':
                            tmp.push_back(_link);
                            break;
                        case 'W':
                            tmp.push_back(" ");
                            break;
                        case 'V':
                            tmp.push_back(views);
                            break;
                        case 'L':
                            tmp.push_back(_likes);
                            break;
                        case 'C':
                            tmp.push_back(_counter);
                            break;
                        default:
                            break;
                        }
                    }
                    if (ignor[_link]!=1)  vect.push_back(tmp);
                    else{ std::cout << "Ignoring\n"; std::cout.flush();}
#elif Posts
                    vector<string> tmp;
                    for(auto elem: _fmt){
                        switch (elem) {
                        case 'D':
                            tmp.push_back(formatData(t, true));
                            break;
                        case 'd':
                            tmp.push_back(formatData(t, false));
                            break;
                        case 'l':
                            tmp.push_back(_link);
                            break;
                        case 'V':
                            tmp.push_back(views);
                            break;
                        case 'L':
                            tmp.push_back(_likes);
                            break;
                        case 'C':
                            tmp.push_back(_counter);
                            break;
                        default:
                            break;
                        }
                    }
                    if (ignor[_link]!=1)  vect.push_back(tmp);
                    else{ std::cout << "Ignoring\n"; std::cout.flush();}
#endif
                    count++;
                }else{
                    break;
                }
                coun++;
            }
            stringstream sq2;
            f_post=count!=0 ? formatData(end_post_time) : "not find reels";
            stringstream sq3;
#ifdef Reels
            sq3 << data["paging_info"]["more_available"];
#elif Posts
            sq3 << data["more_available"];
#endif
            maxid="";
            if (coun==dat.size() && sq3.str()=="true"){
#ifdef Reels
                maxid=data["paging_info"]["max_id"];
#elif Posts
                maxid=data["next_max_id"];
#endif
            }
            //std::cout << formatData((time_t)(*dat.begin())["media"]["taken_at"]) << " " << formatData((time_t)(*dat.rbegin())["media"]["taken_at"]) << "\n";
            std::cout << coun << " = count in reponce; " << count << " = count of parsing\n";
            answer=vect;
            return 2;
        }catch(std::runtime_error e){
            std::cout << "Error in Parsing responce\n";
            std::cout << e.what() << "\n"; std::cout.flush();
            return false;
        }
    }

};

#endif // INSTAGRAMUTILS_H
