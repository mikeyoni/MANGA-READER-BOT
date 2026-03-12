#include <iostream>
#include <string>
#include <vector>
//#include <algorithm>
#include <fstream>
#include <filesystem>
#include <curl/curl.h>
#include "json.hpp"
#include <thread>
#include <chrono>

using namespace std;
using json = nlohmann::json;

vector<string> url;


class webscpaer
{
    public:
    CURL* engine;
    CURLcode error;
    string html;
    struct curl_slist *headers = nullptr; // Modern C++ way
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

    string scrapurl( const string &purl)
    {
            html.clear();
            if (!engine) return "Engine Error";
        {
            curl_easy_reset(engine); // Start fresh for every URL

                // 1. You MUST set a realistic User-Agent for AllManga
                curl_easy_setopt(engine, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");

                // 2. Enable Redirects (Crucial for search pages)
                curl_easy_setopt(engine, CURLOPT_FOLLOWLOCATION, 1L);

                // 3. Set the URL
                curl_easy_setopt(engine, CURLOPT_URL, purl.c_str());

                // 4. Standard boilerplate
                curl_easy_setopt(engine, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(engine, CURLOPT_WRITEDATA, &html);

                // 5. SSL Fix (Sometimes Fedora needs this for specific certs)
                curl_easy_setopt(engine, CURLOPT_SSL_VERIFYPEER, 0L);


                // 7. PERFORM (This line stops the code and WAITS for the internet)
                error = curl_easy_perform(engine);

                if (error == CURLE_OK)
                {
                    cout << "\n--- SUCCESS [" << purl << "] ---\n";
                    // Now that we have the full string, we can return it
                    return html;
                }
                else{
                    cout<<"\n\n\n FAILURE !!!\n\n" <<error<<endl;
                }
        }
        return "Error";
    }

    ~webscpaer() {
        if (headers) curl_slist_free_all(headers);
        if (engine) curl_easy_cleanup(engine);
        curl_global_cleanup();
    }

};

void jsontoarray(const string &file)
{
    fstream data(file);

    json index;

    if (data.is_open())
    {
        data >> index;
        data.close();
    }
    else
    {
        cout<<"\n\n\n FAILURE !!! File is not fund in path \n\n";
    }

   for (auto & i : index)
   {
       url.push_back(i["url"]);
   }

}

int main()
{
    cout << "--- Starting Manga Scraper ---" << endl;

    webscpaer hi;

    // 1. Fill your 'url' vector from index.json
    jsontoarray("../index.json");

    if (url.empty()) {
        cout << "Error: No URLs found in index.json!" << endl;
        return 1;
    }

    for (int i = 0; i < url.size(); i++)
    {
        cout << "Scraping [" << i << "]: " << url[i] << " ..." << endl;

        // 2. Get the HTML data
        string result = hi.scrapurl(url[i]);

        // 3. Create a unique filename for each URL
        string fileName = "output_" + to_string(i) + ".html";

        // 4. Save to file
        ofstream out(fileName);
        if (out.is_open()) {
            out << result;
            out.close();
            cout << "Saved to: " << fileName << " (" << result.length() << " bytes)" << endl;
        } else {
            cout << "Failed to create file: " << fileName << endl;
        }

        // 5. Be nice to the servers so they don't ban your IP
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    cout << "--- All Tasks Finished! ---" << endl;
    return 0;
}