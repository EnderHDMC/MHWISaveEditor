#pragma once

#include <curl/curl.h>

class NetworkQuery {
  CURL* hnd;
  QByteArray data = NULL;
  bool finished = false;

  static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    NetworkQuery* query = (NetworkQuery*)userp;
    return query->writeCallback(contents, size, nmemb);
  }

  size_t writeCallback(void* contents, size_t size, size_t nmemb) {
    size_t realsize = size * nmemb;
    data.append((const char*)contents, realsize);
    finished = true;
    return realsize;
  }

public:
  NetworkQuery() {
    hnd = curl_easy_init();
  }

  int get(const QString& url) {
    CURLcode ret;

    QByteArray url_data = url.toUtf8();
    const char* curl_url = url_data.constData();
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, curl_url);
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.4.0");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, this);

    ret = curl_easy_perform(hnd);

    curl_easy_cleanup(hnd);
    hnd = NULL;

    return (int)ret;
  }

  QByteArray receivedData() {
    return data;
  }
};
