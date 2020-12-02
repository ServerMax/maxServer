#include "httpRequest.h"
#include "httper.h"
#include "core.h"

#define increase_http_reference(reference) { \
    int ref = (reference) + 1; \
    tools::memery::safeMemcpy((void *)&(reference), sizeof(reference), &ref, sizeof(ref)); \
}

#define reduce_http_increase(reference) { \
    int ref = (reference) - 1; \
    tools::memery::safeMemcpy((void *)&(reference), sizeof(reference), &ref, sizeof(ref)); \
}

namespace tcore {
    tlib::tpool<httpRequest> s_request_pool;

    httpRequest * httpRequest::create(const u64 delivery, const s32 id, const char * url, iHttpResponse * response, const iContext & context) {
        return create_from_pool(s_request_pool, delivery, id, url, response, context);
    }

    void httpRequest::release() {
        recover_to_pool(s_request_pool, this);
    }

    void httpRequest::postParam(const char * key, const char * value) {
        if (_params.length() != 0) {
            _params.append("&");
        }

        _params.append(key).append("=").append(value);
    }

    void httpRequest::postData(const std::string & data) {
        _params = data;
    }

    void httpRequest::doRequest() {
        increase_http_reference(_reponse->_reference);
        httper::getInstance()->push(this);
    }

    void httpRequest::onResponse() {
        if (nullptr == _reponse) {
            return;
        }

        if (_error != 0) {
            trace(core::getInstance(), "%s request error, code %d", _url.c_str(), _error);
            _reponse->onError(_id, _error, _url, _params, _context);
        }
        else {
            s32 len;
            reduce_http_increase(_reponse->_reference);
            const void * data = _data.getBuff(len);
            _reponse->onResponse(_id, _url, _params, data, len, _context);
        }
    }
}
