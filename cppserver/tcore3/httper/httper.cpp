#include "httper.h"
#include "tqueue.h"
#include "core.h"
#include "curl/curl.h"
#include "tinyxml/tinyxml2.h"
#include "autolock.h"

namespace tcore {
    static tlib::tqueue<httpRequest *> * s_request_queue;
    static tlib::tqueue<httpRequest *> s_response_queue;

    static s32 s_current_thread_count;
    static bool s_termination;


    httper * httper::getInstance() {
        static httper * s_httper = nullptr;
        if (nullptr == s_httper) {
            std::string path;
            path.append(tools::file::getApppath()).append("/server_config/").append(core::getInstance()->getCorename()).append("/core/core.xml");
            tinyxml2::XMLDocument config;
            tinyxml2::XMLElement * root = nullptr;
            tinyxml2::XMLElement * http = nullptr;
            if (tinyxml2::XML_SUCCESS != config.LoadFile(path.c_str())
                || nullptr == (root = config.RootElement())
                || nullptr == (http = root->FirstChildElement("http"))) {
                s_current_thread_count = 1;
            } else {
                s_current_thread_count = tools::stringAsInt(http->Attribute("thread_count"));
            }

            s_httper = tnew httper;
            s_termination = false;
            s_request_queue = tnew tlib::tqueue<httpRequest *>[s_current_thread_count];
            s_httper->start(s_current_thread_count);
        }

        return s_httper;
    }

    static size_t HttpResponseWrite(void* buffer, size_t size, size_t nmemb, void* param) {
        httpData * data = (httpData *)param;
        if (nullptr == data || nullptr == buffer) {
            return -1;
        }
        data->pushData(buffer, size * nmemb);
        return nmemb;
    }

    void httper::run() {
        CURL * handle = curl_easy_init();
        tassert(handle, "wtf");
        static tlib::spinlock s_init_lock;
        static u32 s_index = 0;
        u32 index = 0;
        {
            autolocker(lock, s_init_lock);
            index = s_index++;
            //trace(Core::getInstance(), "http thread %d init", index);
        }


        while (true) {
            if (s_request_queue[index].isEmpty()) {
                static tlib::spinlock s_shutdown_lock;

                if (s_termination) {
                    autolocker(lock, s_shutdown_lock);
                    s_current_thread_count--;
                    break;
                }
                msleep(1);
            }

            httpRequest * request;
            while (s_request_queue[index].tryPull(request)) {
                curl_easy_reset(handle);
                curl_easy_setopt(handle, CURLOPT_TIMEOUT, 10);
                curl_easy_setopt(handle, CURLOPT_VERBOSE, 0L);
                curl_easy_setopt(handle, CURLOPT_URL, request->getUrl());
                curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
                curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, HttpResponseWrite);
                curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)request->gethttpDataCache());

                curl_easy_setopt(handle, CURLOPT_POST, 1);
                curl_easy_setopt(handle, CURLOPT_POSTFIELDS, request->getParams());

                s32 code = curl_easy_perform(handle);
                while (CURLE_OPERATION_TIMEDOUT == code) {
                    code = curl_easy_perform(handle);
                }

                if (code != CURLE_OK) {
                    request->setError(code);
                } else {
                    char a = 0;
                    request->gethttpDataCache()->pushData(&a, 1);
                }

                static tlib::spinlock s_response_lock;
                {
                    autolocker(lock, s_response_lock);
                    s_response_queue.push(request);
                }
            }
        }

        curl_easy_cleanup(handle);
        handle = nullptr;
    }

    void httper::terminate() {
        s_termination = true;
        while (s_current_thread_count != 0) {
            msleep(1);
        }

        httpRequest * request;
        while (s_response_queue.tryPull(request)) {
            request->onResponse();
            request->release();
            request = nullptr;
        }

        tdel[] s_request_queue;
    }

    httpRequest * httper::getHttpRequest(const u64 delivery, const s64 id, const char * url, iHttpResponse * response, const iContext & context) {
        httpRequest * request = httpRequest::create(delivery, id, url, response, context);
        return request;
    }

    void httper::push(httpRequest * request) {
        tassert(s_current_thread_count != 0, "wtf");
        if (s_current_thread_count != 0) {
            s_request_queue[::abs((s64)(request->_delivery) % s_current_thread_count)].push(request);
        }
    }

    void httper::update() {
        s64 tick = tools::time::getMicrosecond();
        httpRequest * request;
        while (s_response_queue.tryPull(request)) {
            request->onResponse();
            request->release();
            request = nullptr;

            if (tools::time::getMicrosecond() - tick >= 1000) {
                return;
            }
        }
    }

    void httper::shutdown() {
        terminate();
        tdel this;
    }

}
