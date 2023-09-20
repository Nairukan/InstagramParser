#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <bits/stdc++.h>
//#include "instagramparser.h"
#include "request.h"

using namespace std;

















/*string VBS_EXEL_HAS_FILE =
(string)"Set obj = createobject(\"Excel.Application\")"+"\n"+
"set fso = CreateObject(\"Scripting.FileSystemObject\")"+"\n"+
"curDir = fso.GetAbsolutePathName(\".\")"+"\n"+
"Set obj2 = obj.Workbooks.Open( curDir & \"\\Protocol.xls\")"+"\n"+
"Set obj1 = obj2.Worksheets(1)"+"\n"+
"obj1.Cells(1,1).Value=\"Hello!!\""+"\n"+
"obj1.Save"+"\n"+
"obj2.Close"+"\n"+
"obj.Quit"+"\n"+
"Set obj1=Nothing"+"\n"+
"Set obj=Nothing"+"\n"+
""+"\n";

string VBS_EXEL_DONT_HAS_FILE =
(string)"Set obj = createobject(\"Excel.Application\")"+"\n"+
"set fso = CreateObject(\"Scripting.FileSystemObject\")"+"\n"+
"curDir = fso.GetAbsolutePathName(\".\")"+"\n"+
"Set obj2 = obj.Workbooks.Add()"+"\n"+
"Set obj1 = obj2.Worksheets(1)"+"\n"+
"obj1.Cells(1,1).Value=\"Hello!!\""+"\n"+
"obj1.SaveAs curDir & \"\\Protocol.xls\""+"\n"+
"obj2.Close"+"\n"+
"obj.Quit"+"\n"+
"Set obj1=Nothing"+"\n"+
"Set obj=Nothing"+"\n"+
""+"\n";
*/





int main(int argc, char** argv){
    /*InstagramParser IP;
    try{
        IP.Processing(argc, argv);
    }
    catch(const char* error_message){
        cout << error_message;
    }*/

    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, "https://edu.grsu.by/pluginfile.php/");
    char curlErrorBuffer[CURL_ERROR_SIZE];
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, curlErrorBuffer);

    int status=curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    cout << "status " << status << " " << curlErrorBuffer;

    //Request(curl_easy_init(), "https://edu.grsu.by/pluginfile.php/", map<string, string>(), {});
}
