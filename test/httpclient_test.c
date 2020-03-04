//
// Created by Ju-Jin Yoo on 2020/03/03.
//

#include <stdio.h>
#include <network/httpclient.h>

int main() {
    const char *request_url = "https://github.com/JuJin1324/CLion_stater/blob/master/README.md";
    int res = http_request_get_file(request_url);
    printf("res : %d\n", res);
}
