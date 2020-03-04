/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2019, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * Download a given URL into a local file named page.out.
 * </DESC>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <curl/curl.h>

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int main(int argc, char *argv[])
{
    const char *request_url = "https://github.com/JuJin1324/CLion_stater/blob/master/README.md";
    const char *save_filename = "README.md";
    CURL *curl_handle;
    FILE *save_file;

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* request url 설정 */
    curl_easy_setopt(curl_handle, CURLOPT_URL, request_url);
    /* 상세 정보 보기(콘솔에 request/body 관련 정보가 출력된다.) */
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    /* cert 검증 : 내가 가진 ARM 기기에서 할 수 없었음. */
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
    /* disable progress meter, set to 0L to enable it */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    /* open the file */
    save_file = fopen(save_filename, "wb");
    if(save_file) {

        /* write the page body to this file handle */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, save_file);

        /* get it! */
        curl_easy_perform(curl_handle);

        /* close the header file */
        fclose(save_file);
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    curl_global_cleanup();

    return 0;
}