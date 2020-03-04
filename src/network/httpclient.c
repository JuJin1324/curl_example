//
// Created by Ju-Jin Yoo on 2020/02/13.
//

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include <sds/sds.h>
#include "httpclient.h"

#define PROTOCOL_HTTP   "http"
#define PROTOCOL_HTTPS  "https"

#define HTTP_SUCCESS   0
#define HTTP_ERROR     1
#define MINUTE         60

URL_COMPONENT str_to_url_component(const char *url) {
    URL_COMPONENT component;
    component.domain = sdsempty();
    component.port = 0;
    component.uri = sdsempty();

    char protocol[64];
    char host[2048];
    char path[2048];
    char port[6];

    if (sscanf(url, "%50[^:/]://%2000[^:]:%5[^/]/%s", protocol, host, port, path) == 4) {
        component.domain = sdscpy(component.domain, host);
        component.uri = sdscpy(component.uri, path);
        component.port = (int) strtol(port, NULL, 10);

    } else if (sscanf(url, "%50[^:/]://%2000[^/]/%s", protocol, host, path) == 3) {
        component.domain = sdscpy(component.domain, host);
        component.uri = sdscpy(component.uri, path);

        /* checking the protocol specified */
        if (strstr(protocol, PROTOCOL_HTTPS) != NULL) {
            component.port = 443;
        } else if (strstr(protocol, PROTOCOL_HTTP) != NULL) {
            component.port = 80;
        }
    }
    return component;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *) stream);
    return written;
}

int http_request_get_file(const char *request_url) {
    CURL *curl;
    FILE *save_file;
    URL_COMPONENT component;
    CURLcode res;

    sds uri = sdsempty();
    const char *separator = "/";
    sds *tokens;
    int count;

    component = str_to_url_component(request_url);
    uri = sdscpy(uri, component.uri);
    tokens = sdssplitlen(uri, (int) sdslen(uri), separator, (int) strlen(separator), &count);
    /* uri를 / 기준으로 분리한 마지막이 파일 명 */
    sds filename = tokens[count - 1];

    curl = curl_easy_init();
    /* request url 설정 */
    curl_easy_setopt(curl, CURLOPT_URL, request_url);
    /* 상세 정보 보기(콘솔에 request/body 관련 정보가 출력된다.) */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    /* redirect 된 경우 해당 경로를 따라가도록 함 */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    /* 타임아웃(second) 설정 */
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3 * MINUTE);
    /* cert 검증 안함 : 텔라딘에서 할 수 없음. */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    /* send all data to this function  */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

    /* open the file */
    save_file = fopen(filename, "wb");
    if (save_file) {
        /* write the page body to this file handle */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, save_file);
        /* get it! */
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return HTTP_ERROR;
        }
        /* close the header file */
        fclose(save_file);
    }
    /* cleanup curl stuff */
    curl_easy_cleanup(curl);

    sdsfreesplitres(tokens, count);
    sdsfree(uri);

    return HTTP_SUCCESS;
}