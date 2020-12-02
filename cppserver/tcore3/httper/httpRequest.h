#ifndef __httpRequest_h__
#define __httpRequest_h__

#include "api.h"
#include "tpool.h"
#include "httpData.h"

namespace tcore {
    using namespace api;

    class httpRequest : public iHttpRequest {
    public:
        const u64 _delivery;

        static httpRequest * create(const u64 delivery, const s32 id, const char * url, iHttpResponse * response, const iContext & context);
        void release();

        const char * getParams() { return _params.c_str(); }
        const char * getUrl() { return _url.c_str(); }
        void setError(const s32 error) { _error = error; }
        void onResponse();

        const iContext & GetContext() { return _context; }

        httpData * gethttpDataCache() { return &_data; }

        virtual void postParam(const char * key, const char * value);
        virtual void postData(const std::string & data);
        virtual void doRequest();


    private:
        httpRequest(const u64 delivery, const s32 id, const char * url, iHttpResponse * response, const iContext & context)
            : _delivery(delivery), _id(id), _url(url), _reponse(response), _context(context), _error(0) {
            tassert(_reponse, "wtf");
            _params.clear();
        }

        friend class tlib::tpool<httpRequest>;

    private:
        const s32 _id;
        const std::string _url;
        const iContext _context;
        iHttpResponse * _reponse;
        std::string _params;
        httpData _data;
        s32 _error;
    };
}

#endif //__httpRequest_h__
