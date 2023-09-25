#include "parser.h"

InstagramParser::InstagramParser()
{

}

void InstagramParser::Update_tokens(){
    if (TOKEN_MUTEX.try_lock()){
        fstream f;
        cout << "Wait response by update token...\n";
        system("curl -X POST https://www.instagram.com/ -A \"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0\" --output temp/log.json");
        cout << "Response taken by update token\n";

        //extract json
        f.open("temp/log.json", fstream::in);
        string temp;
            for (int i=0; i<25; i++) getline(f, temp);
        f.close();
        int k;
        string finding="\"define\"";
        int len=(string(finding)).length();
        for (k=0; k<temp.length()-len; k++){
            if (temp.substr(k, len) == finding) break;
        }
        int s=k;
        finding="\"instances\"";
        len=(string(finding)).length();
        for (k; k<temp.length()-len; k++){
            if (temp.substr(k, len) == finding) break;
        }

        f.open("temp/log.json", fstream::out);
            f << "{" <<temp.substr(s, k-s-1) << "}";
        f.close();

        //extract interesting data
        f.open("temp/log.json", fstream::in);
        json data = json::parse(f);
        f.close();
        system("del temp\\log.json");
        for (auto b = data.begin(); b!=data.end(); ++b){
            for (auto now = (*b).begin(); now!=(*b).end(); ++now)
                if ((*now)[0]=="XIGSharedData"){
                   //extract csrf_token
                   stringstream ss; ss << (*now)[2];
                   string inter = ss.str();
                   int k;
                   string finding="\"raw\"";
                   int len=(string(finding)).length();
                   for (k=0; k<inter.length()-len; k++){
                       if (inter.substr(k, len) == finding) break;
                   }
                   finding="\"csrf_token\\\":\\\"";
                   len=(string(finding)).length();
                   for (k=0; k<inter.length()-len; k++){
                       if (inter.substr(k, len) == finding) break;
                   }
                   int s=k+len;
                   finding="\",\\\"viewer\\\"";
                   len=(string(finding)).length();
                   for (k; k<inter.length()-len; k++){
                       if (inter.substr(k, len) == finding) break;
                   }
                   inter=inter.substr(s, k-s-1);
                   csrf_token = inter;
                }else if ((*now)[0]=="RelayAPIConfigDefaults"){
                   //extract X-IG-App-ID
                   stringstream ss; ss << (*now)[2];
                   string inter = ss.str();
                   int k;
                   string finding="\"X-IG-App-ID\":\"";
                   int len=(string(finding)).length();
                   for (k=0; k<inter.length()-len; k++){
                       if (inter.substr(k, len) == finding) break;
                   }
                   int s=k+len;
                   finding=",\"X-IG-D\"";
                   len=(string(finding)).length();
                   for (k; k<inter.length()-len; k++){
                       if (inter.substr(k, len) == finding) break;
                   }
                   inter=inter.substr(s, k-s-1);
                   X_IG_App_ID = inter;

                }
        }
        OUTPUT_MUTEX.lock();
        cout << "Tokens updated\n";
        cout << "csrf_token " << csrf_token << "\n";
        cout << "X-IG-App-ID " << X_IG_App_ID << "\n";
        OUTPUT_MUTEX.unlock();
        TOKEN_MUTEX.unlock();
    }else{
        int i=0;
        while(!TOKEN_MUTEX.try_lock()){
            if (++i>1000000000){
                i=0;
                OUTPUT_MUTEX.lock();
                cout << "Wait to update tokens\n";
                OUTPUT_MUTEX.unlock();
            }
        }
        TOKEN_MUTEX.unlock();
        return;
    }
}

void InstagramParser::authorithation(){
    if (AUTH_MUTEX.try_lock()){
        system("pause");
        cout << "Wait response by authorization...\n";
        string curl_com="curl -X POST https://www.instagram.com/api/v1/web/accounts/login/ajax/ -A \"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0\" -H \"X-CSRFToken: " + csrf_token + "\" -H \"X-Instagram-AJAX: 1007182281\" -H \"X-IG-App-ID: " + X_IG_App_ID + "\" --data-raw \"enc_password=#PWD_INSTAGRAM_BROWSER:0:"+int_to_str(time(0))+":"+PASSWORD_PARSING_ACC+"&username="+USERNAME_PARSING_ACC+"&queryParams={}&optIntoOneTap=false&trustedDeviceRecords={}\" --output temp/log1.json -i";
        system(curl_com.c_str());
        //ofstream cur_out("curl.json");
        //cur_out << curl_com;
        //cur_out.close();
        cout << "Response taken by authorization\n";
        fstream f;
        f.open("temp/log1.json", fstream::in);
        cout << "Responce opened\n";
        stringstream ss1;
        json data;
        int co=0;
        string some;
        int SessionID_line=-1;
        while(!f.eof()){
            co++;
            getline(f, some);
            //cout << some << "\n";
            if (SessionID_line==-1){
                int k;
                string finding="Set-Cookie: sessionid=";
                int len=(string(finding)).length();
                int m=(int)some.length()-len;
                //cout << "for (k=0; k<" << m << "; k++)\n";
                for (k=0; k<m; k++){
                    if (some.substr(k, len) == finding) break;
                }
                //cout << k << "\n";
                if (k==0 && m>k){
                    SessionID_line=co;
                    //cout << co << "\n";
                    int s=k+len;
                    for (k; k<some.length(); k++){
                        if (some[k] == ';') break;
                    }
                    some=some.substr(s, k-s);
                    sessionID = some; //hui
                }
            }
        }
        //cout << "sessionID " << sessionID << "\n";
        ss1 << some;
        f.close();
        f.open("temp/log1.json", fstream::out);
        f << ss1.str();
        f.close();
        ifstream jso("temp/log1.json");
        data = json::parse(jso);
        jso.close();
        system("del temp\\log1.json");
        string status=data["status"];
        OUTPUT_MUTEX.lock();
        cout << "Authorithation log:" << status << "\n";
        OUTPUT_MUTEX.unlock();
        if (status =="fail"){
            OUTPUT_MUTEX.lock();
            cout << "Error Authorization!!!!!!!\n";
            OUTPUT_MUTEX.unlock();
            //Update_tokens();
            //system(("curl -sX POST https://www.instagram.com/api/v1/web/accounts/login/ajax/ -A \"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0\" -H \"X-CSRFToken: " + csrf_token + "\" -H \"X-Instagram-AJAX: 1007182281\" -H \"X-IG-App-ID: " + X_IG_App_ID + "\" --data-raw \"enc_password=#PWD_INSTAGRAM_BROWSER:0:1679776612:Rq7QtT5Hvt7ARec&username=somebotmail040@gmail.com&queryParams={}&optIntoOneTap=false&trustedDeviceRecords={}\" --output log1.json").c_str());
  }
        AUTH_MUTEX.unlock();
    }else{
        while(!AUTH_MUTEX.try_lock());
        AUTH_MUTEX.unlock();
        return;
    }
}

string InstagramParser::get_user_id(string username, int ind){
    string com="curl -sL -A \"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0\" -H \"Cookie:ds_user_id="+ID_PARSING_ACC+";csrftoken="+csrf_token+";sessionid="+sessionID+"\" -H \"X-CSRFToken: " + csrf_token +"\" -H \"X-IG-App-ID:"+ X_IG_App_ID +"\" --output temp/log1"+ int_to_str(ind) +".json  https://www.instagram.com/api/v1/users/web_profile_info/?username="+username;
    system((com).c_str());
 /*OUTPUT_MUTEX.lock();
    cout << username << " Callback accept\n";
    OUTPUT_MUTEX.unlock();
 */
    ifstream f(("temp/log1"+int_to_str(ind)+".json").c_str());
    //cout << "Open stream\n";
    json data = json::parse(f);
    //cout << "Parsed\n";
    f.close();
    //cout << "Closed\n";
    this_thread::yield();
    system(("del temp\\log1"+int_to_str(ind)+".json").c_str());
    string status=data["status"];
    OUTPUT_MUTEX.lock();
    cout << "get_user_id log:" << status << "\n";
    OUTPUT_MUTEX.unlock();
    if (status !="ok"){
        OUTPUT_MUTEX.lock();
        cout << "Try Auth again\n";
        OUTPUT_MUTEX.unlock();
        authorithation();
        return get_user_id(username, ind);
    }else{
        string id=data["data"]["user"]["id"];
        return id;
    }
}

tuple<vector<string>,string, string> InstagramParser::get_reels_info(string username, time_t startT, time_t endT, int ind, string max_id="", string id=""){
    if (id=="")
       id=get_user_id(username, ind);
    string curl_com;
    if (max_id=="")
        curl_com="curl -sL -A \"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0\" -H \"Cookie:ds_user_id="+ID_PARSING_ACC+";csrftoken="+csrf_token+";sessionid="+sessionID+"\" -H \"X-CSRFToken: " + csrf_token + "\" -H \"X-IG-App-ID: " + X_IG_App_ID + "\" --output temp/log2"+int_to_str(ind)+".json --data \"target_user_id="+id+"&page_size=40&include_feed_video=true\" -X POST https://www.instagram.com/api/v1/clips/user/?";
    else
        curl_com="curl -sL -A \"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0\" -H \"Cookie:ds_user_id="+ID_PARSING_ACC+";csrftoken="+csrf_token+";sessionid="+sessionID+"\" -H \"X-CSRFToken: " + csrf_token + "\" -H \"X-IG-App-ID: " + X_IG_App_ID + "\" --output temp/log2"+int_to_str(ind)+".json --data \"target_user_id="+id+"&page_size=40&max_id="+max_id+"&include_feed_video=true\" -X POST https://www.instagram.com/api/v1/clips/user/?";
    system((curl_com).c_str());
    ifstream f(("temp/log2"+int_to_str(ind)+".json").c_str());
    json data = json::parse(f);
    f.close();
    this_thread::yield();
    system(("del temp\\log2"+int_to_str(ind)+".json").c_str());
    string status=data["status"];
    OUTPUT_MUTEX.lock();
    cout << "get_reels_info log:" << status << "\n";
    OUTPUT_MUTEX.unlock();
    if (status !="ok"){
        OUTPUT_MUTEX.lock();
        cout << "Try Auth again\n";
        OUTPUT_MUTEX.unlock();
        authorithation();
        return get_reels_info(username, startT, endT, ind, max_id, id);
    }else{
        time_t end_post_time=time(0);
        vector<string> vect;
        //stringstream ss;
        //ss << "INFO ABOUT " << username << " REELS YOUNGLY THAN " << days_coming << " DAYS\n\n";
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
               l_post=formatData(t);
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
                vect.push_back(views);
                //ss << "\nReels " << ctime(&t) << "    caption = " << (*media)["media"]["caption"]["text"] << " |\n   views =" << (*media)["media"]["play_count"] << " | likes =" << (*media)["media"]["like_count"] << " |\n_______________________________________________________\n";
                count++;
            }else{
                //OUTPUT_MUTEX.lock();
                //cout << coun << ". " << t << " " << startT << "\n";
                //system("pause");
                //OUTPUT_MUTEX.unlock();
                break;
            }
            coun++;
        }
        stringstream sq2;
        f_post=count!=0 ? formatData(end_post_time) : "not find reels";
        stringstream sq3;
        sq3 << data["paging_info"]["more_available"];
        if (coun==dat.size() && sq3.str()=="true"){
            OUTPUT_MUTEX.lock();
            cout << username << " : Download more Reels...\n";
            OUTPUT_MUTEX.unlock();
            stringstream sq4;
            sq4 << data["paging_info"]["max_id"];
            auto nw = get_reels_info(username, startT, endT, ind, sq4.str(), id);
            auto v=get<0>(nw);
            if (get<1>(nw)!="not find reels")
               f_post=get<1>(nw);
            vect.insert(vect.end(), v.begin(), v.end());
        }
        OUTPUT_MUTEX.lock();
        cout << username <<" : Complete writing info about " << count << " Reels\n";
        OUTPUT_MUTEX.unlock();
        return tuple<vector<string>, string, string>{vect, f_post, l_post};
    }
}

void InstagramParser::Parsing_ACC(string username, int index){
    OUTPUT_MUTEX.lock();
    cout << username << " " << index << "\n";
    OUTPUT_MUTEX.unlock();
    tuple<vector<string>,string,string> answer=get_reels_info(username, Date_t_start, Date_t_stop, index);
    auto vect=get<0>(answer);
    vect.insert(vect.begin(), username);
    vect.insert(++vect.begin(), get<2>(answer));
    vect.push_back(get<1>(answer));
    OUTPUT_MUTEX.lock();
    totalInfo.push_back(vect);
    if (vect.size()>maxLen) maxLen=vect.size();
    //protocol << answer << "\n\n";
    OUTPUT_MUTEX.unlock();
}


