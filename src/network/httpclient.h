//
// Created by Ju-Jin Yoo on 2020/02/13.
//

#ifndef CT_AGENT_HTTPCLIENT_H
#define CT_AGENT_HTTPCLIENT_H

#include <stdio.h>

#include <sds/sds.h>

typedef struct url_component {
    sds domain;
    int port;
    sds uri;
} URL_COMPONENT;

URL_COMPONENT str_to_url_component(const char *url);
int http_request_get_file(const char *request_url);

#endif //CT_AGENT_HTTPCLIENT_H
