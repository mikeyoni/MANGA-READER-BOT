#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <curl/curl.h>
#include "json.hpp"

using namespace std;

class webscpaer
{
    public:
    CURL* engine;
    CURLcode error;
    string html;

    webscpaer() : engine(nullptr), error(CURLE_OK)
    {
        curl_global_init(CURL_GLOBAL_ALL);
        engine = curl_easy_init();

        if (!engine)
        {
            cout<<" Faild to inishalized the engine ";
        }

    }

    static size_t write_data(void *content , size_t bites , size_t pakage , std::string *data)
    {
        size_t total_size = bites * pakage;
        if (data)
        {
            data -> append(static_cast<char*>(content), total_size) ;
            return total_size;
        }
        else
        {
            cout<<"\n\n\n DATA CURUPTION !!!\n\n";
        }
        return 0;

    }

    void scrapurl( const string &url)
    {
        html.clear();

        if (engine)
        {
            curl_easy_setopt(engine, CURLOPT_URL, url.c_str());
            curl_easy_setopt(engine, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(engine, CURLOPT_WRITEDATA, &html);

            error = curl_easy_perform(engine);

        }

        if (!error)
        {
            cout<<"\n\n\n SUCCESS !!!\n\n";
            cout<<html;

            html.clear();
            curl_easy_cleanup(engine);
            engine = nullptr;
            curl_global_cleanup();
        }
        else
        {
            cout<<"\n\n\n FAILURE !!!\n\n" <<error<<endl;
        }

    }



};

int main()
{
    cout << "Hello World!!!" << endl;

    webscpaer hi;

    hi.scrapurl("https://www.google.com");


    return 0;
}
