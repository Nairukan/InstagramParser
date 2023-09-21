#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <bits/stdc++.h>
//#include "instagramparser.h"
#include "request.h"
#include "instagramutils.h"
#include <format>

//#define DEBUG 1



using namespace std;

string int_to_str(unsigned long long num){
    string ans="";
    if (num==0) return "0";
    while (num){
        ans=char(num%10+'0')+ans;
        num/=10;
    }
    return ans;
}

template <class T1, class T2>
void part_map(map<T1, T2>* dictionary, initializer_list<T1> keys){
    map<T1, T2> temp;
    for (auto now: keys){
        temp[now]=(*dictionary)[now];
    }
    *dictionary=temp;
}

string USERNAME_PARSING_ACC, PASSWORD_PARSING_ACC;
time_t Date_t_start, Date_t_stop;

int str_to_int(string text){
    int ans=0;
    for (int i=0; i<text.length(); i++){
        ans=ans*10+(text[i]-'0');
    }
    return ans;
}

time_t GetUnixTime(string strDate, bool endOfDay=false){
    struct tm timeinfo;
    int year=str_to_int(strDate.substr(6,4)), month=str_to_int(strDate.substr(3,2)), day=str_to_int(strDate.substr(0,2));
    timeinfo.tm_year   = year - 1900;
    timeinfo.tm_mon    = month - 1;    //months since January - [0,11]
    timeinfo.tm_mday   = day;//day of the month - [1,31]
    timeinfo.tm_hour   = endOfDay ? 23 : 0;         //hours since midnight - [0,23]
    timeinfo.tm_min    = endOfDay ? 59 : 0;          //minutes after the hour - [0,59]
    timeinfo.tm_sec    = endOfDay ? 59 : 0;
    return mktime ( &timeinfo );
}

const string formatData(time_t t, bool isShort=false) {
    time_t     now = t;
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), isShort? "%m/%d" :"%Y_%m_%d", &tstruct);

    return buf;
}


// <worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:mx=\"http://schemas.microsoft.com/office/mac/excel/2008/main\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" xmlns:mv=\"urn:schemas-microsoft-com:mac:vml\" xmlns:x14=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/main\" xmlns:x15=\"http://schemas.microsoft.com/office/spreadsheetml/2010/11/main\" xmlns:x14ac=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac\" xmlns:xm=\"http://schemas.microsoft.com/office/excel/2006/main\"><sheetPr><outlinePr summaryBelow=\"0\" summaryRight=\"0\"/></sheetPr><sheetViews><sheetView workbookViewId=\"0\"/></sheetViews><sheetFormatPr customHeight=\"1\" defaultColWidth=\"12.63\" defaultRowHeight=\"15.75\"/>";

int main(int argc, char** argv){

    //************ Is Normal Settings of Parser ************//
    if (!filesystem::is_directory("./temp")) filesystem::create_directory("temp");
    if (!filesystem::is_directory("./result")) filesystem::create_directory("result");
    if (!filesystem::exists("parsing_acc.txt") || filesystem::is_empty("parsing_acc.txt")){
        ofstream("parsing_acc.txt") << "<1 in row Account for parsing>";
        throw("Error with Setting of Parser(file \"parsing_acc.txt\" must contain list of parsing accounts)\n");
    }
    if (!filesystem::exists("Init.ini") || filesystem::is_empty("Init.ini")){
        (ofstream("Init.ini")) << "<Username ParserAcc> <Password ParserAcc> <ID ParserAcc>";
        throw("Error with Setting of Parser(file \"Init.ini\" must contain login info about [Parser account])\n");
    }

    //************ Read Settings for Parsing ************//
    ifstream Parsing_Acc("Init.ini");
    Parsing_Acc >> USERNAME_PARSING_ACC >> PASSWORD_PARSING_ACC;
    Parsing_Acc.close();
    string Date;
    int maxLen=0;
    vector<vector<string>> totalInfo;
    ifstream accounts("parsing_acc.txt");
    string temp;
    vector<string> username={};

    while (!accounts.eof()){
        getline(accounts, temp);
        if (temp=="") break;
        username.push_back(temp);
    }
    accounts.close();

    cout << "Enter start date in format \"DD.MM.YYYY\":\n";
    cin >> Date;
    Date_t_start=GetUnixTime(Date);
    cout << "Enter end date in format \"DD.MM.YYYY\":\n";
    cin >> Date;
    Date_t_stop=GetUnixTime(Date, true);
    cout << Date_t_start << " " << Date_t_stop << "\n";
    string dirpath="result/ParsingProtocol_start"+formatData(Date_t_start)+"_end"+formatData(Date_t_stop);
    if (!filesystem::is_directory(dirpath)) filesystem::create_directory(dirpath);

    curl_global_init(CURL_GLOBAL_ALL);
    //Headers
    map<string, string> headers({
        {"User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/113.0"},
        {"X-Instagram-AJAX", "1008126642"}
    });
    map<string, string> cookies({});
    CURL* handle=curl_easy_init();


    stringstream* buffer = new stringstream;

//Tokens begin
    Request(handle, "https://www.instagram.com/", headers, cookies, buffer, true);
#ifdef DEBUG
    ofstream tokens("tokens.log");
    tokens << "\n\n" << buffer->str();
#endif  
    InstagramUtils::ExtractPrimeTokens(buffer, headers);
//Tokens end


//Authorization begin
    *buffer=stringstream(std::format("enc_password=%23PWD_INSTAGRAM_BROWSER%3A0%3A{}%3A{}&username={}&queryParams={}&optIntoOneTap=false&trustedDeviceRecords={}",
                                  int_to_str(time(0)), PASSWORD_PARSING_ACC, USERNAME_PARSING_ACC, "%7B%7D", "%7B%7D"));

    cookies["csrftoken"]=headers["X-CSRFToken"];
    part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
    headers["Content-Type"]="application/x-www-form-urlencoded";
    headers["X-Requested-With"]="XMLHttpRequest";
    Request(handle, "https://www.instagram.com/api/v1/web/accounts/login/ajax/", headers, cookies, buffer, true);
    headers["X-CSRFToken"]=cookies["csrftoken"];
#ifdef DEBUG
    ofstream authorization("authorization.log");
    authorization << "\n\n" << buffer->str();
    authorization.close();
#endif
//Authorization end


//Parsing begin

    for (auto now: username){
        cout << now << ":\n";
        part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
        part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
        headers["X-CSRFToken"]=cookies["csrftoken"];
        *buffer=stringstream();
        Request(handle, "https://www.instagram.com/api/v1/users/web_profile_info/?username="+now, headers, cookies, buffer, false);
#ifdef DEBUG
        ofstream get_id("get_id.log");
        get_id << "\n\n" << buffer->str();
        get_id.close();
#endif
        string id;
        if (!InstagramUtils::ExtractId_ParsingAcc(buffer, id)){
            cout << now << " - error get_id. Skiped...\n";
            break;
        }
        time_t startT=Date_t_start, endT=Date_t_stop;
        string max_id="";
        part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
        part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
        headers["X-CSRFToken"]=cookies["csrftoken"];
        *buffer=stringstream("target_user_id="+id+"&page_size=100&include_feed_video=true");
        Request(handle, "https://www.instagram.com/api/v1/clips/user/?", headers, cookies, buffer, true);

        vector<vector<string>> answer;
        if (!InstagramUtils::ProcessingResponceOfParsing(buffer, startT, endT, max_id, answer)){
            cout << now << " - error parsing reels. Skiped...\n";
            continue;
        }
        while(max_id!=""){
            cout << "Download more Reels....\n";
            part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
            part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
            headers["X-CSRFToken"]=cookies["csrftoken"];
            *buffer=stringstream("target_user_id="+id+"&page_size=100&max_id="+max_id+"&include_feed_video=true");
            Request(handle, "https://www.instagram.com/api/v1/clips/user/?", headers, cookies, buffer, true);
            vector<vector<std::string>> t_answer;
            if (!InstagramUtils::ProcessingResponceOfParsing(buffer, startT, endT, max_id, t_answer)){
                cout << now << " - error parsing reels. Skiped...\n";
                break;
            }
            auto v=t_answer;
            answer.insert(answer.end(), v.begin(), v.end());
        }
        cout << "Merge\n";
        reverse(answer.begin(), answer.end());
        auto vect=answer;
        if (answer.size()==0) answer={{"no found posts", "no found posts", "no found posts"}};
        vect.insert(vect.begin(), {"Views "+formatData(time(0), true),"Date","Publication link"});
        vect.insert(vect.begin(), {"", "", ""});

        vect[0][0]="https://www.instagram.com/"+now+"/?hl=ru";
        cout << format("Record {}\n", now);

        ofstream unoacc_protocol(dirpath+"/"+now+".csv");
        for (auto line=vect.begin(); line!=vect.end(); line++){
            for (int j=0; j<line->size()-1; j++){
                unoacc_protocol << (*line)[j] << ";";
            }
            unoacc_protocol << (*line)[line->size()-1] << "\n";
        }
        unoacc_protocol.close();
    }
//Parsing end

//Logout begin
    part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
    part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
    headers["X-CSRFToken"]=cookies["csrftoken"];
    *buffer=stringstream(format("one_tap_app_login=0&user_id={}", cookies["ds_user_id"]));
    cout << "Logout\n";
    if (cookies["sessionid"]=="" || cookies["sessionid"]=="\"\"") cout << "Logout not required\n";
    else Request(handle, "https://www.instagram.com/api/v1/web/accounts/logout/ajax/", headers, cookies, buffer, true);
#ifdef DEBUG
    ofstream logout("logout.log");
    logout << "\n\n" << buffer->str();
    logout.close();
#endif
    cout << "Resources Cleanig\n";
    curl_global_cleanup();
    delete buffer;
}


