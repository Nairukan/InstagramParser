#ifndef INSTAGRAMPARSER_H
#define INSTAGRAMPARSER_H

#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <bits/stdc++.h>
#include <filesystem>

using namespace std;
using json = nlohmann::json;
//Banned
//string USERNAME_PARSING_ACC="nikita_dev050", PASSWORD_PARSING_ACC="Chihuahua13", ID_PARSING_ACC="59045068227";
//Timed banned
//string USERNAME_PARSING_ACC="nikita_dev060", PASSWORD_PARSING_ACC="Chihuahua13", ID_PARSING_ACC="59129752873";


class InstagramParser
{
public:
    string USERNAME_PARSING_ACC, PASSWORD_PARSING_ACC, ID_PARSING_ACC;

    mutex AUTH_MUTEX, TOKEN_MUTEX, OUTPUT_MUTEX;

    string X_IG_App_ID;
    string csrf_token;
    string sessionID;

    string int_to_str(unsigned long long num){
        string ans="";
        if (num==0) return "0";
        while (num){
            ans=char(num%10+'0')+ans;
            num/=10;
        }
        return ans;
    }

    int str_to_int(string text){
        int ans=0;
        for (int i=0; i<text.length(); i++){
            ans=ans*10+(text[i]-'0');
        }
        return ans;
    }

    const string formatData(time_t t) {
        time_t     now = t;
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y_%m_%d", &tstruct);

        return buf;
    }

    const std::string currentDateTime() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y_%m_%d", &tstruct);
        return buf;
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

    vector<thread*> Threads;

    time_t Date_t_start, Date_t_stop;
    int maxLen=0;
    vector<vector<string> > totalInfo;
    //ofstream protocol("ParsingProtocol.txt");

    void Update_tokens();
    void authorithation();
    string get_user_id(string username, int ind);
    tuple<vector<string>,string, string> get_reels_info(string username, time_t startT, time_t endT, int ind, string max_id, string id);
    void Parsing_ACC(string username, int index);
    void logout(){
         system(("curl -X POST https://www.instagram.com/api/v1/web/accounts/logout/ajax/ -A \"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0\" -H \"Cookie:ds_user_id="+ID_PARSING_ACC+";csrftoken="+csrf_token+";sessionid="+sessionID+"\" -H \"X-CSRFToken: " + csrf_token +"\" -H \"X-IG-App-ID:"+ X_IG_App_ID +"\"  --data \"one_tap_app_login=0&user_id="+ID_PARSING_ACC+"\"").c_str());
    }



    void Processing(int argc, char** argv){

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
        Parsing_Acc >> USERNAME_PARSING_ACC >> PASSWORD_PARSING_ACC >> ID_PARSING_ACC;
        Parsing_Acc.close();
        string Date;
        cout << "Enter start date in format \"DD.MM.YYYY\":\n";
        cin >> Date;
        Date_t_start=GetUnixTime(Date);
        cout << "Enter end date in format \"DD.MM.YYYY\":\n";
        cin >> Date;
        Date_t_stop=GetUnixTime(Date, true);


        ofstream protocol(("results/ParsingProtocol_start"+formatData(Date_t_start)+"_end"+formatData(Date_t_stop)+".csv").c_str());
        //cout << Date_t_start << " " << Date_t_stop << "\n";
        Update_tokens();
        //authorithation();
        cout << "sessionID " << sessionID << "\n";
        ifstream accounts("parsing_acc.txt");
        string temp;
        while (!accounts.eof()){
            getline(accounts, temp);
            if (temp=="") break;
            Threads.push_back(new thread([&, this]{this->Parsing_ACC(temp, Threads.size());}));
        }
        for (int i=0; i<Threads.size(); i++){
            Threads[i]->join();
            delete Threads[i];
        }
        logout();
        for (int i=0; i<totalInfo.size(); i++){
            try{
                system(("del temp\\log1"+int_to_str(i)+".json").c_str());
            }catch(...){

            }
            try{
                system(("del temp\\log2"+int_to_str(i)+".json").c_str());
            }catch(...){

            }
        }
        cout << "START EXPORT TO EXEL\n";
        for (int i=0; i<maxLen-1; i++){
            for (int j=0; j<totalInfo.size(); j++){
                if (totalInfo[j].size()-1<=i) protocol << "-";
                else protocol << totalInfo[j][i];
                if (j!=totalInfo.size()-1) protocol << ";";
            }
            protocol << "\n";
        }
        for (int j=0; j<totalInfo.size(); j++){
            protocol << totalInfo[j][totalInfo[j].size()-1];
            if (j!=totalInfo.size()-1) protocol << ";";
        }
        protocol.close();
        cout << "END OF PARSING";
        return;
    }

    InstagramParser();
};

#endif // INSTAGRAMPARSER_H
