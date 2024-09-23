#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <thread>
#include <mutex>
#include <bits/stdc++.h>
#include "instagramutils.h"
#include <format>
#include <ExelWorkLib/exelworklib.h>
#include <NetworkRequestLib/request.h>

//#define DEBUG 1
#define _PAGE_SIZE "140"
#define Accs_PER_SESSION 1000




using namespace std;
using namespace request;

string int_to_str(unsigned long long num){
    string ans="";
    if (num==0) return "0";
    while (num){
        ans=char(num%10+'0')+ans;
        num/=10;
    }
    return ans;
}

template < typename Type >
std::string to_str (const Type & t)
{
  std::ostringstream os;
  os << t;
  return os.str ();
}

template <class T1, class T2>
void part_map(map<T1, T2>* dictionary, initializer_list<T1> keys){
    map<T1, T2> temp;
    for (auto now: keys){
        temp[now]=(*dictionary)[now];
    }
    *dictionary=temp;
}

static string USERNAME_PARSING_ACC, PASSWORD_PARSING_ACC;
time_t Date_t_start, Date_t_stop;

int str_to_int(string text){
    int ans=0;
    for (int i=0; i<text.length(); i++){
        if (text[i]<'0' || text[i]>'9') throw "12";
        ans=ans*10+(text[i]-'0');
    }
    return ans;
}

static vector<string> split(const string& s, char delim) {
        stringstream ss(s);
        string item;
        vector<string> elems;
        while (getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
}

int str_to_double(string text){
    int ans=0;
    int i=0;
    for (; i<text.length() && text[i]!='.'; i++){
        if (text[i]<'0' || text[i]>'9') throw "12";
        ans=ans*10+(text[i]-'0');
    }
    double multi=0.1;
    for (i=i+1; i<text.length(); ++i){
        if (text[i]<'0' || text[i]>'9') throw "12";
        ans+=multi*(text[i]-'0');
        multi/=10;
    }
    return ans;
}


time_t GetUnixTime(string strDate, bool endOfDay=false){
    struct tm timeinfo;
    int year=str_to_int(strDate.substr(6,4)), month=str_to_int(strDate.substr(3,2)), day=str_to_int(strDate.substr(0,2));
    timeinfo.tm_year   = year - 1900;
    timeinfo.tm_mon    = month - 1;    //months since January - [0,11]
    timeinfo.tm_mday   = day;//day of the month - [1,31]

    if (strDate.length()==19){
        int hour=str_to_int(strDate.substr(11,2)), min=str_to_int(strDate.substr(14,2)), sec=str_to_int(strDate.substr(17,2));
        timeinfo.tm_hour   = hour;         //hours since midnight - [0,23]
        timeinfo.tm_min    = min;          //minutes after the hour - [0,59]
        timeinfo.tm_sec    = sec;
        return mktime ( &timeinfo );
    }
    timeinfo.tm_hour   = endOfDay ? 23 : 0;         //hours since midnight - [0,23]
    timeinfo.tm_min    = endOfDay ? 59 : 0;          //minutes after the hour - [0,59]
    timeinfo.tm_sec    = endOfDay ? 58 : 0;
    return mktime ( &timeinfo );
}

time_t GetUnixTime_ForResource(string strDate){
    struct tm timeinfo;
    int year=str_to_int(strDate.substr(0,4)), month=str_to_int(strDate.substr(5,2)), day=str_to_int(strDate.substr(8,2));
    int hour=(strDate.length()<=11 ? 0 : str_to_int(strDate.substr(11,2))), min=strDate.length()<=11 ? 0 : str_to_int(strDate.substr(14,2)), sec=strDate.length()<=11 ? 0 : str_to_int(strDate.substr(17,2));

    timeinfo.tm_year   = year - 1900;
    timeinfo.tm_mon    = month - 1;    //months since January - [0,11]
    timeinfo.tm_mday   = day;//day of the month - [1,31]
    timeinfo.tm_hour   = hour;         //hours since midnight - [0,23]
    timeinfo.tm_min    = min;          //minutes after the hour - [0,59]
    timeinfo.tm_sec    = sec;
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


void PrimeTokens(CURL* handle, map<string, string>& headers, map<string, string>& cookies){
    headers["User-Agent"]="Mozilla/5.0 (X11; Linux x86_64; rv:120.0) Gecko/20100101 Firefox/120.0";
    stringstream * buffer = new stringstream;
    Request(handle, "https://www.instagram.com/", headers, cookies, buffer, true).exec();
#ifdef DEBUG_FILE
    ofstream tokens("tokens.log");
    tokens << "\n\n" << buffer->str();
#endif
    if (!InstagramUtils::ExtractPrimeTokens(buffer, headers)){
        for(auto now: headers){
            cout << format("headers[{}]={}\n", now.first, now.second);
        }
        cout.flush();
        throw "error";
    }
    delete buffer;
  cout << "Tokens complete\n"; cout.flush();
}

void Authorizate(CURL* handle, map<string, string>& headers, map<string, string>& cookies){
    stringstream * buffer = new stringstream;
    *buffer=stringstream(format("enc_password=%23PWD_INSTAGRAM_BROWSER%3A0%3A{}%3A{}&username={}&queryParams={}&optIntoOneTap=false&trustedDeviceRecords={}",
                                  int_to_str(time(0)), PASSWORD_PARSING_ACC, USERNAME_PARSING_ACC, "%7B%7D", "%7B%7D"));

    cookies["csrftoken"]=headers["X-CSRFToken"];
    cout << "csrftoken " << cookies["csrftoken"] << "\n";
    part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
    headers["Content-Type"]="application/x-www-form-urlencoded";
    headers["X-Requested-With"]="XMLHttpRequest";
    Request(handle, "https://www.instagram.com/api/v1/web/accounts/login/ajax/", headers, cookies, buffer, true).exec();
    headers["X-CSRFToken"]=cookies["csrftoken"];
    if (cookies["sessionid"]=="" || cookies["sessionid"]==" ") cout << "!!! Not Authorizated !!!\n\n";
    else cout << "Good Authorizated :)\n\n";
    cout.flush();
#ifdef DEBUG_FILE
    ofstream authorization("authorization.log");
    authorization << "\n\n" << buffer->str();
    authorization.close();
#endif
    delete buffer;
}

void Logout(CURL* handle, map<string, string>& headers, map<string, string>& cookies){
    stringstream * buffer = new stringstream;
    part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
    part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
    headers["X-CSRFToken"]=cookies["csrftoken"];
    *buffer=stringstream(format("one_tap_app_login=0&user_id={}", cookies["ds_user_id"]));
    cout << "Logout\n";
    if (cookies["sessionid"]=="" || cookies["sessionid"]=="\"\"") cout << "Logout not required\n";
    else Request(handle, "https://www.instagram.com/api/v1/web/accounts/logout/ajax/", headers, cookies, buffer, true).exec();
    cookies["sessionid"]="";
#ifdef DEBUG
    ofstream logout("logout.log");
    logout << "\n\n" << buffer->str();
    logout.close();
#endif
    delete buffer;
}

static bool relogin(request::Request *rq){
    if (rq->code>=400 && rq->code<500){
        std::cout << "relogin check\n";
        json data;
        try{
            data= json::parse(rq->get_temp_responce());
            if (data["status"]!="ok" && data["require_login"]==true && data["message"]=="Please wait a few minutes before you try again."){
                //Count request limit need relogin

                //auto buffer=rq->result();
                //auto url=rq->URL;
                auto handle=rq->get_curl_handle();
                pair<map<string,string>&, map<string, string>&> metadata = rq->get_metadata();

                Logout(handle, metadata.first, metadata.second);
                metadata.first.clear(); metadata.second.clear();
                PrimeTokens(handle, metadata.first, metadata.second);
                Authorizate(handle, metadata.first, metadata.second);
                rq->exec(metadata.first, metadata.second);
                return true;
            }
        }catch(...){
            return false;
        }
    }
    return false;
}

static bool checkpoint(request::Request *rq){
    if (rq->code>=400 && rq->code<500){

        std::cout << "checkpoint check\n";
        json data;
        try{
            string temp=rq->get_temp_responce();
            data= json::parse(temp);
            if (data["status"]!="ok" && data["lock"]==true && data["message"]=="checkpoint_required"){
                //need pass checkpoint
                cout << format("Pass Checkpoint on {} acc, and press Enter\n", USERNAME_PARSING_ACC);
                getline(cin, temp);
                getline(cin, temp);
                rq->exec();
                return true;
            }
        }catch(...){
            return false;
        }
    }
    return false;
}

// <worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:mx=\"http://schemas.microsoft.com/office/mac/excel/2008/main\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" xmlns:mv=\"urn:schemas-microsoft-com:mac:vml\" xmlns:x14=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/main\" xmlns:x15=\"http://schemas.microsoft.com/office/spreadsheetml/2010/11/main\" xmlns:x14ac=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac\" xmlns:xm=\"http://schemas.microsoft.com/office/excel/2006/main\"><sheetPr><outlinePr summaryBelow=\"0\" summaryRight=\"0\"/></sheetPr><sheetViews><sheetView workbookViewId=\"0\"/></sheetViews><sheetFormatPr customHeight=\"1\" defaultColWidth=\"12.63\" defaultRowHeight=\"15.75\"/>";

int main(int argc, char** argv){
    int ViewColumn=-1;
    bool ExelOnly=false;
    bool IgnorList=false;
    string resource_dir="";
    ExelFile* RESOURCE_BUILD;
    if (argc > 1){
        if (string(argv[1])=="-ExelOnly") ExelOnly=true;
        else if (string(argv[1])=="-IgnorList") IgnorList=true;
        else if (string(argv[1])=="-FromResource"){
            resource_dir=argv[2];
        }
    }

    cout << format("argc: {}\nargv:\n", argc);
    for (int i=0; i<argc; i++) cout << format("{}. {}\n", i, argv[i]);
    //return 0;


    //************ Is Normal Settings of Parser ************//
    if (!filesystem::is_directory("./temp")) filesystem::create_directory("temp");
    if (!filesystem::is_directory("./result")) filesystem::create_directory("result");
    if (!filesystem::is_directory("./result/xlsx_s")) filesystem::create_directory("result/xlsx_s");
    if (!filesystem::is_directory("./result/csv_s")) filesystem::create_directory("result/csv_s");
    if (!filesystem::exists("parsing_acc.txt") || filesystem::is_empty("parsing_acc.txt")){
        ofstream("parsing_acc.txt") << "<1 in row Account for parsing>";
        throw runtime_error("Error with Setting of Parser(file \"parsing_acc.txt\" must contain list of parsing accounts)\n");
    }
    if (!filesystem::exists("Init.ini") || filesystem::is_empty("Init.ini")){
        (ofstream("Init.ini")) << "<Username ParserAcc> <Password ParserAcc> <ID ParserAcc>";
        throw runtime_error("Error with Setting of Parser(file \"Init.ini\" must contain login info about [Parser account])\n");
    }
    //request::Request_count_max_ms=8000;


    //************ Read Settings for Parsing ************//
    ifstream Parsing_Acc("Init.ini");

#ifdef Test_AutoAcc_DEV

    Parsing_Acc.close();
    return 0;
#endif

    Parsing_Acc >> USERNAME_PARSING_ACC >> PASSWORD_PARSING_ACC;
    Parsing_Acc.close();
    string Date;
    int maxLen=0;
    vector<vector<string>> totalInfo;
    ifstream accounts("parsing_acc.txt");
    string temp;
    vector<string> username={};
    std::string fmt;
    while (!accounts.eof()){
        getline(accounts, temp);
        if (temp=="") break;
        username.push_back(temp);
    }
    accounts.close();
    //IgnorList=true;
    map<string, int> ignor;
    if (IgnorList){
        if (!filesystem::exists("ignor.txt")){
            (ofstream("ignor.txt")) << "<link of ignor reels>";
            throw runtime_error("Error with Setting of Parser(file \"ignor.txt\" must to be with flag -IgnorList)\n");
        }
        ifstream r1("ignor.txt");
        string t;
        while(!r1.eof()){
            getline(r1, t);
            if (t=="") continue;
            ignor[t]=1;
        }
    }

    if (!ExelOnly){
        cout << format("Enter format of responce\n{}\n{}\n{}\n{}\n{}\n{}:\n",
                " * D - Full format Data",
                " * d - Short format Data",
                " * l - Link",
                " * V - Count of Views",
                " * L - Count of Likes",
                " * C - Counter");
               //a - list of couathors (for raw info)
        cin >> fmt;
        fmt+="a";
        {
            string temp="DdlVLCWa";
            unsigned int size=fmt.length();
            for (int i=fmt.length()-1; i>=0; i--){
                if (temp.find(fmt[i])==temp.length()){
                    for (int j=i; j<fmt.length()-1; j++)
                        fmt[j]=fmt[j+1];
                    --size;
                }
            }
            fmt.resize(size);
        }
        std::cout << fmt << "\n";

    }

    vector<string> resource_paths;
    for (int i=0; i<username.size(); i++) resource_paths.push_back(resource_dir+"/"+username[i]+".csv");
    RESOURCE_BUILD=ExelFile::read_CSVs(resource_paths);

    cout << "Enter start date in format \"DD.MM.YYYY\" or \"DD.MM.YYYY_HH:mm:ss\":\n";
    cin >> Date;
    Date_t_start=GetUnixTime(Date);
    cout << "Enter end date in format \"DD.MM.YYYY\" or \"DD.MM.YYYY_HH:mm:ss\":\n";
    cin >> Date;
    Date_t_stop=GetUnixTime(Date, true);
#ifdef DEBUG
    cout << Date_t_start << " " << Date_t_stop << "\n";
#endif
    const string name=format("ParsingProtocol_start{}_end{}", formatData(Date_t_start), formatData(Date_t_stop));
    string dirpath="result/csv_s/"+name;
    if (!filesystem::is_directory(dirpath)) filesystem::create_directory(dirpath);
    if (!filesystem::is_directory("result/xlsx_s/"+name)) filesystem::create_directory("result/xlsx_s/"+name);

    if (!ExelOnly){
        map<string, string> headers,cookies;
        CURL* handle;
        request::Request::auto_producers_all.push_back(relogin);
        request::Request::auto_producers_all.push_back(checkpoint);
        request::Request::Request_count_max_ms=30000;
        stringstream* buffer;
        if(resource_dir==""){
            curl_global_init(CURL_GLOBAL_ALL);
            //Headers
            headers={
                {"User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:120.0) Gecko/20100101 Firefox/120.0"},
            };
            cookies={};
            handle=curl_easy_init();

            buffer = new stringstream;
        }
    //Tokens
    //PrimeTokens(handle, headers, cookies);


    //Authorization

    //Authorizate(handle, headers, cookies);


    //Parsing begin

        int countA=0;


        for (auto now: username){
            if(resource_dir==""){
                ++countA;
                if ((countA-1)%Accs_PER_SESSION==0){
                    headers.clear(); cookies.clear();
                    PrimeTokens(handle, headers, cookies);
                    Authorizate(handle, headers, cookies);
                }
            }
            cout << now << ":\n";
            unsigned int counter=0;
            unsigned int res=0;
            string max_id="";
            time_t startT=Date_t_start, endT=Date_t_stop;
            vector<vector<string>> answer;
            string id;
            if (resource_dir!=""){
                if(!RESOURCE_BUILD->SheetNames.contains(now)){
                    cout << now << " - error get_id. Skiped...\n";
                    continue;
                }
                //В ресурсе есть записи
                auto Sheet = (*RESOURCE_BUILD)[now];
                int FullDataColumn=0;
                for (auto qwe : fmt){
                    if (qwe=='D' || qwe=='d') break;
                    ++FullDataColumn;
                }
                int line=3;
                for (line=3; line<=Sheet.heigth() && GetUnixTime_ForResource(Sheet[{FullDataColumn+1, line}]->val())+2<startT; ++line);
                vector<vector<string>> vect;
                for (line; line<=Sheet.heigth() && GetUnixTime_ForResource(Sheet[{FullDataColumn+1, line}]->val())+2<=endT; ++line){
                    vector<string> temporary_line(fmt.length());
                    for (int column=1; column<=fmt.length(); ++column){
                        temporary_line[column-1]=Sheet[{column, line}]->val();
                    }
                    vect.push_back(temporary_line);
                }
                unsigned int locale_counter=0;
                vector<string> tmp;
                for(auto elem: fmt){
                    switch (elem) {
                    case 'D':
                        tmp.push_back("Date");
                        break;
                    case 'd':
                        tmp.push_back("Date");
                        break;
                    case 'l':
                        tmp.push_back("Publication link");
                        break;
                    case 'W':
                        tmp.push_back("Publication link");
                        break;
                    case 'a':
                        tmp.push_back("Coauthors list");
                        break;
                    case 'V':
                        tmp.push_back("Views "+formatData(time(0), true));
                        ViewColumn=locale_counter;
                        break;
                    case 'L':
                        tmp.push_back("Likes "+formatData(time(0), true));
                        break;
                    case 'C':
                        tmp.push_back("№");
                        break;
                    default:
                        break;
                    }
                    ++locale_counter;
                }

                vect.insert(vect.begin(), tmp);

                vect.insert(vect.begin(), vector<string>(fmt.length(), ""));

                vect[0][0]="https://www.instagram.com/"+now+"/?hl=ru";
                ofstream unoacc_protocol(dirpath+"/"+now+".csv");
                for (auto line=vect.begin(); line!=vect.end(); line++){
                    for (int j=0; j<line->size()-1; j++){
                        unoacc_protocol << (*line)[j] << ";";
                    }
                    unoacc_protocol << (*line)[line->size()-1] << "\n";
                }
                unoacc_protocol.close();
                continue;
            }
            part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
            part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
            headers["X-CSRFToken"]=cookies["csrftoken"];
            *buffer=stringstream();
            Request(handle, "https://www.instagram.com/api/v1/users/web_profile_info/?username="+now, headers, cookies, buffer, false).exec();
    #ifdef DEBUG_FILE
            ofstream get_id("get_id.json");
            get_id << "\n\n" << buffer->str();
            get_id.close();
    #endif
            if (!InstagramUtils::ExtractId_ParsingAcc(buffer, id)){
                cout << now << " - error get_id. Skiped...\n";
                continue;
            }
            while(res < 2){


                part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
                part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
                headers["X-CSRFToken"]=cookies["csrftoken"];
    #ifdef Reels
                *buffer=stringstream("target_user_id="+id+"&page_size="+_PAGE_SIZE+"&include_feed_video=true");
                Request(handle, "https://www.instagram.com/api/v1/clips/user/?", headers, cookies, buffer, true).exec();
    #elif Posts
                *buffer=stringstream();
                Request(handle, format("https://www.instagram.com/api/v1/feed/user/{}/?{}", id, "count=33"), headers, cookies, buffer, true).exec();
    #endif


    #ifdef DEBUG_FILE
                //std::ofstream ReelsLOG("Reels.json");
                //  ReelsLOG << "\n\n" << buffer->str();
    #endif
                if (res=InstagramUtils::ProcessingResponceOfParsing(buffer, startT, endT, max_id, answer, counter, fmt, ignor, now); !res){
                    cout << now << " - error parsing reels. Skiped...\n";
                    break;
                }
                else if (res==1){
                    cout << "Repeat download...\n";
                    continue;
                }
            }
            while(max_id!="" && res){
                if (res!=1) cout << "Download more ....\n";
                else cout << "Repeat download...\n";
                part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
                part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
                headers["X-CSRFToken"]=cookies["csrftoken"];
    #ifdef Reels
                *buffer=stringstream("target_user_id="+id+"&page_size="+_PAGE_SIZE+"&max_id="+max_id+"&include_feed_video=true");
                request::Request(handle, "https://www.instagram.com/api/v1/clips/user/?", headers, cookies, buffer, true).exec();
    #elif Posts

                *buffer=stringstream();
                Request(handle, format("https://www.instagram.com/api/v1/feed/user/{}/?{}", id, "count=33&max_id="+max_id), headers, cookies, buffer, true).exec();
    #endif
                vector<vector<std::string>> t_answer;
                if (res=InstagramUtils::ProcessingResponceOfParsing(buffer, startT, endT, max_id, t_answer, counter, fmt, ignor, now); !res){
                    cout << now << " - error parsing reels. Skiped...\n";
                    break;
                }else if(res==1){
                    continue;
                }

                auto v=t_answer;
                answer.insert(answer.end(), v.begin(), v.end());
            }
            cout << "Merge\n";
            reverse(answer.begin(), answer.end());
            if (answer.size()==0) answer=vector<vector<string>>({vector<string>(fmt.length(), "no found posts")});
            auto vect=answer;

            unsigned int _COUNER_POS=-1;
            for (int i=0; i<fmt.length(); i++)
                if (fmt[i]=='c') _COUNER_POS=i;
            if (_COUNER_POS!=-1)
                for (int i=0; i<vect.size()/2; i++){
                    swap(vect[i][_COUNER_POS], vect[answer.size()-i-1][_COUNER_POS]);
                }
            vector<string> tmp;
            unsigned int locale_counter=0;
            for(auto elem: fmt){
                switch (elem) {
                case 'D':
                    tmp.push_back("Date");
                    break;
                case 'd':
                    tmp.push_back("Date");
                    break;
                case 'l':
                    tmp.push_back("Publication link");
                    break;
                case 'W':
                    tmp.push_back("Publication link");
                    break;
                case 'a':
                    tmp.push_back("Coauthors list");
                    break;
                case 'V':
                    tmp.push_back("Views "+formatData(time(0), true));
                    ViewColumn=locale_counter;
                    break;
                case 'L':
                    tmp.push_back("Likes "+formatData(time(0), true));
                    break;
                case 'C':
                    tmp.push_back("№");
                    break;
                default:
                    break;
                }
                ++locale_counter;
            }

            vect.insert(vect.begin(), tmp);

            vect.insert(vect.begin(), vector<string>(fmt.length(), ""));

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
            if ((countA-1)%Accs_PER_SESSION==Accs_PER_SESSION-1){
                Logout(handle, headers, cookies);
            }
        }
    if ((countA-1)%Accs_PER_SESSION!=Accs_PER_SESSION-1 && resource_dir!=""){
        Logout(handle, headers, cookies);
    }
    //Parsing end

    //Logout
       //Logout(handle, headers, cookies);

        cout << "Resources Cleanig\n";
        if (resource_dir==""){
            curl_global_cleanup();
            delete buffer;
        }

        ofstream conf(dirpath+"/config.txt");
        conf << fmt;
        conf.close();
    }else{
        const string name=format("ParsingProtocol_start{}_end{}", formatData(Date_t_start), formatData(Date_t_stop));
        string dirpath="result/csv_s/"+name;
        ifstream i(dirpath+"/config.txt");
        i >> fmt;
    }
    cout << "StartExel\n";
    for (int i=0; i<username.size(); i++) username[i]=dirpath+"/"+username[i]+".csv";
    ExelFile* result=ExelFile::read_CSVs(username);
    result->make_XLXS(format("result/xlsx_s/{}/raw_with_list_couathors.xlsx", name));
    unsigned int _VIEWS_POS=-1;
    unsigned int _LIKES_POS=-1;
    unsigned int _LINK_POS=-1;
    unsigned int _TIME_POS=-1;
    for (int i=0; i<fmt.length(); i++)
        if (fmt[i]=='L') _LIKES_POS=i;
        else if (fmt[i]=='V') _VIEWS_POS=i;
        else if (fmt[i]=='l') _LINK_POS=i;
        else if (fmt[i]=='d' || fmt[i]=='D') _TIME_POS=i;

    ExelFile* newExel=new ExelFile();
        map<string, unsigned int> now_lines;
        set<string> links;
        map<string, time_t> now_times;
        for (auto now: result->SheetNames){
            now_lines[now.first]=3;
            if (3<=(*result)[now.first].heigth())
                now_times[now.first]=GetUnixTime_ForResource((*result)[now.first][{_TIME_POS+1, 3}]->val());
            else{
                now_lines.erase(now.first);
            }
        }
        time_t max_time=0;
        string now_record_parent;
        unsigned int currentLine=2;
        while(true){
            if (now_lines.size())
                now_record_parent=now_lines.begin()->first;
            else break;
            for (auto now : now_lines){
                if (now_times[now.first]<now_times[now_record_parent]){
                    now_record_parent=now.first;
                }
            }
            string current_link=(*result)[now_record_parent][{_LINK_POS+1, now_lines[now_record_parent]}]->val();
            unsigned int old_size=links.size();
            if (links.insert(current_link); old_size!=links.size()){
                for (int j=1; j<=(*result)[now_record_parent].width(); ++j)
                    (*(*newExel)["AllReels"][{j, currentLine}])=(*result)[now_record_parent][{j, now_lines[now_record_parent]}]->val();
                ++currentLine;
            }
            ++now_lines[now_record_parent];
            if (now_lines[now_record_parent]<=(*result)[now_record_parent].heigth())
                now_times[now_record_parent]=GetUnixTime_ForResource((*result)[now_record_parent][{_TIME_POS+1, now_lines[now_record_parent]}]->val());
            else{
                now_lines.erase(now_record_parent);
            }
        }

        newExel->make_XLXS(format("result/xlsx_s/{}/One_List.xlsx", name));
        delete newExel;

    //Take global list of used acc

    if (_VIEWS_POS!=-1 || _LIKES_POS!=-1 || true){
        set<string> all_authors;
        for (auto now: result->SheetNames){
            for (int i=3; i<=(*result)[now.first].heigth(); ++i){
                auto list=split((*result)[now.first][{(*result)[now.first].width(), i}]->val(), '|');
                for (auto acc: list){
                    acc.erase(std::remove(acc.begin(), acc.end(), '\"'), acc.end());
                    all_authors.insert(acc);
                }

            }
        }
        map<string, string> headers,cookies;
        CURL* handle;
        stringstream* buffer;
        curl_global_init(CURL_GLOBAL_ALL);
        headers={
                {"User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:120.0) Gecko/20100101 Firefox/120.0"},
            };
            cookies={};
            handle=curl_easy_init();

            buffer = new stringstream;

        PrimeTokens(handle, headers, cookies);
        Authorizate(handle, headers, cookies);
        for (auto now: all_authors){
            cout << "take " << now << " subscribers\n";
            part_map<string, string>(&headers, {"User-Agent", "X-CSRFToken", "X-Instagram-AJAX", "X_IG_App_ID"});
            part_map<string, string>(&cookies, {"sessionid", "csrftoken", "ds_user_id"});
            headers["X-CSRFToken"]=cookies["csrftoken"];
            *buffer=stringstream();
            Request(handle, "https://www.instagram.com/api/v1/users/web_profile_info/?username="+now, headers, cookies, buffer, false).exec();
    #ifdef DEBUG_FILE
            ofstream get_id("get_id.json");
            get_id << "\n\n" << buffer->str();
            get_id.close();
    #endif
            unsigned int buffer_sub=0;
            if (!InstagramUtils::ExtractSubscribe_ParsingAcc(buffer, buffer_sub)){
                InstagramUtils::subsribers[now]=0;
            }else InstagramUtils::subsribers[now]=buffer_sub;

        }
        Logout(handle, headers, cookies);

        //update views and likes

        for (auto now: result->SheetNames){
            double ost_views=0.0, ost_likes=0.0;
            for (int i=3; i<=(*result)[now.first].heigth(); ++i){
                auto list=split((*result)[now.first][{(*result)[now.first].width(), i}]->val(), '|');

                unsigned int local_sum=InstagramUtils::subsribers[now.first];
                unsigned int total_sum=0;
                for (auto coauthor : list){
                    coauthor.erase(std::remove(coauthor.begin(), coauthor.end(), '\"'), coauthor.end());
                    total_sum+=InstagramUtils::subsribers[coauthor];
                }
                if (total_sum==0) total_sum=local_sum;
                double coef=local_sum/double(total_sum);
                auto tmp=to_str(coef*100);
                std::replace(tmp.begin(), tmp.end(), '.', ',');
                (*(*result)[now.first][{(*result)[now.first].width(), i}])=tmp+"%";
                if (_VIEWS_POS!=-1){
                    string views_val=(*result)[now.first][{_VIEWS_POS+1, i}]->val();
                    int view=str_to_int(views_val);
                    double true_new=view*coef;
                    view=true_new;
                    ost_views+=true_new-view;
                    view+=ost_views;
                    ost_views-=int(ost_views);
                    (*(*result)[now.first][{_VIEWS_POS+1, i}])=int_to_str(view);
                }
                if (_LIKES_POS!=-1){
                    string likes_val=(*result)[now.first][{_LIKES_POS+1, i}]->val();
                    int likes=str_to_int(likes_val);
                    double true_new=likes*coef;
                    likes=true_new;
                    ost_likes+=true_new-likes;
                    likes+=ost_likes;
                    ost_likes-=int(ost_likes);
                    (*(*result)[now.first][{_LIKES_POS+1, i}])=int_to_str(likes);
                }
            }
            (*(*result)[now.first][{(*result)[now.first].width(), 2}])="Part Auditory";
        }

        result->make_XLXS(format("result/xlsx_s/{}/true_statistics.xlsx", name));
    }

    long double suma=0;
    for (auto now : result->SheetNames){
        unsigned int height=(*result)[now.first].heigth();
        long double psuma=suma;
        //string prev_data="";
        //unsigned int count_per_day=0;
        if (_VIEWS_POS==-1) _VIEWS_POS=2;
            //cout << "\n\n" << now.first << ":\n";
            for (int i=3; i<=height; ++i){
                /*
                string val_data=(*result)[now.first][{1, i}]->val();
                if (val_data!=prev_data){
                    if (prev_data!=""){
                        cout << prev_data << " " << count_per_day << " reels\n";
                    }
                    prev_data=val_data;
                    count_per_day=1;
                }
                else ++count_per_day;
                */
                string val=(*result)[now.first][{_VIEWS_POS+1, i}]->val();
                //if (now.first=="goldshark.tm" || now.first=="insta_draki" || now.first=="jastreb.pub" || now.first=="stydoba_tv" || now.first=="vine.or" || now.first=="wolf.pub"){
                //    *((*result)[now.first][{ViewColumn+1, i}])=int_to_str(round(str_to_int(val)/5.0));
                //    val=(*result)[now.first][{ViewColumn+1, i}]->val();
                //}
                suma+=str_to_double(val);

            }
            /*
            if (prev_data!=""){
                cout << prev_data << " " << count_per_day << " reels\n";
            }
            */
        cout << int(suma-psuma) << "\n";
        cout << format("{0} - {1} reels\n", now.first, height-2);
        //(*(*result)[now.first][{(unsigned int)(1), (unsigned int)(height+2)}])=format("=SUM(A3:A{})", height);
    }
    cout << "SUMMA IS " << int(suma) << "\n";

    cout << format("result of Parsing located in result/xlsx_s/{0}/ folder\n", name);
    delete result;
}

