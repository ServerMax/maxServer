#ifndef __tparser_h__
#define __tparser_h__

#include "multisys.h"
#include <map>

namespace tlib {
    template<typename... args>
    class ianalysis {
    public:
        virtual ~ianalysis() {}
        //typedef void(*fCaller)(args... a, const void * data, const int len);

        virtual void onAnalysis(const void * data, const int len, args... a) = 0;
    };

    template<typename s, typename sf,  typename... args>
    class tanalysis : public ianalysis<args...> {
        typedef void(*fCaller)(const s & v, args... a);
        typedef bool(*fParse)(const void * data, const int len, sf & pk);
    public:
        tanalysis(const fCaller caller, const fParse parser) : _caller(caller), _parser(parser){}
        virtual ~tanalysis() {}

        virtual void onAnalysis(const void * data, const int len, args... a) {
            s package;
            if (len > 0) {
                if (!_parser(data, len, package)) {
                    tassert(false, "wtf");
                    return;
                }
            }
            
            _caller(package, a...);
        }

        const fCaller _caller;
        const fParse _parser;
    };

    template<typename... args>
    class canalysis : public ianalysis<args...> {
        typedef void(*fCaller)(const void * data, const int len, args... a);
    public:
        canalysis(const fCaller caller) : _caller(caller) {}
        virtual ~canalysis() {}

        virtual void onAnalysis(const void * data, const int len, args... a) {
            _caller(data, len, a...);
        }

        const fCaller _caller;
    };

    template<typename k, typename sf, typename... args>
    class tparser {
        typedef bool(*fParse)(const void * data, const int len, sf & pk);
    public:
        virtual ~tparser() {}
        tparser(const fParse parser) : _parse(parser) {}

        template<typename s>
        void treg(const k & key, void(*caller)(const s & v, args... a), const char * debug) {
            tanalysis<s, sf, args...> * p = tnew tanalysis<s, sf, args...>(caller, _parse);
            _parser_map.insert(std::make_pair(key, p));
        }

        void reg(const k & key, void(*caller)(const void * data, const int len, args... a), const char * debug) {
            canalysis<args...> * p = tnew canalysis<args...>(caller);
            _parser_map.insert(std::make_pair(key, p));
        }

        void unreg(const k & key) {
            auto itor = _parser_map.find(key);
            if (itor != _parser_map.end()) {
                tdel itor->second;
                _parser_map.erase(itor);
            }
        }

        bool trigger(const k & key, const void * data, const int len, args... a) {
            auto itor = _parser_map.find(key);
            if (_parser_map.end() == itor) {
                return false;
            }

            itor->second->onAnalysis(data, len, a...);
            return true;
        }

    private:
        const fParse _parse;
        std::map<k, ianalysis<args...> *> _parser_map;
    };
}

#endif //__tparser_h__
