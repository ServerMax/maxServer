#ifndef __cargs_h__
#define __cargs_h__

#include "multisys.h"
#include "tools.h"

namespace tlib {
#define default_arg_temp 64
#define error_value 0xffffffff

    enum eCargType {
        __int64__,
        __double__,
        __bool__,
        __blob__,
        __string__,

        __nil__
    };

    class carg {
    public:
        virtual ~carg() {
            (*_reference)--;
            if (0 == (*_reference)) {
                if (_data && _data != _temp) {
                    tdel _data;
                }
                tdel _reference;
            }
        }

        carg(const eCargType type) : _type(type), _data(nullptr), _size(0), _reference(tnew int(1)) { }
        carg(const carg& t) : _type(t._type), _reference(t._reference) {
            (*_reference)++;
            
            _size = t._size;
            if (t._data == t._temp) {
                _data = _temp;
                tools::memery::safeMemcpy(_data, sizeof(_temp), t._data, t._size);
            } else {
                _data = t._data;
            }
        }

        carg& operator=(const carg& t) {
            tools::memery::safeMemcpy(this, sizeof(*this), &t, sizeof(t));
            (*_reference)++;
            return *this;
        }

        inline eCargType getType() const {
            return _type;
        }

        inline void setInt64(const s64 value) {
            tassert(_type == eCargType::__int64__, "args type error");
            setData((void*)& value, sizeof(value));
        }

        inline void setDouble(const double value) {
            tassert(_type == eCargType::__double__, "args type error");
            setData((void*)& value, sizeof(value));
        }

        inline void setBool(const bool value) {
            tassert(_type == eCargType::__bool__, "args type error");
            setData((void*)&value, sizeof(value));
        }

        inline void setString(const std::string & value) {
            tassert(_type == eCargType::__string__, "args type error");
            setData((void*)value.c_str(), value.length());
        }

        inline void setBlob(void * data, size_t size) {
            tassert(_type == eCargType::__blob__, "args type error");
            setData(data, size);
        }

        inline s64 getInt64() const {
            tassert(_type == eCargType::__int64__, "args type error");
            if (_type != eCargType::__int64__ || _data == nullptr) {
                return error_value;
            }

            return *(s64*)_data;
        }

        inline double getDouble() const {
            tassert(_type == eCargType::__double__, "args type error");
            if (_type != eCargType::__double__ || _data == nullptr) {
                return error_value;
            }

            return *(double*)_data;
        }

        inline bool getBool() const {
            tassert(_type == eCargType::__bool__, "args type error");
            if (_type != eCargType::__bool__ || _data == nullptr) {
                return "";
            }

            return *(bool *)_data;
        }

        inline std::string getString() const {
            tassert(_type == eCargType::__string__, "args type error");
            if (_type != eCargType::__string__ || _data == nullptr) {
                return "";
            }

            return std::string((const char*)_data, _size);
        }
        
        inline void* getBlob(size_t& size) const {
            size = _size;
            tassert(_type == eCargType::__blob__, "args type error");
            if (_type != eCargType::__blob__ || _data == nullptr) {
                return nullptr;
            }

            return _data;
        }

        inline void* getData(size_t& size) const {
            size = _size;
            return _data;
        }

    private:
        inline void setData(void* data, size_t size) {
            _size = size;
            if (size <= sizeof(_temp)) {
                _data = _temp;
            } else {
                _data = tmalloc(size);
            }

            tools::memery::safeMemcpy(_data, size, data, size);
        }

    private:
        char _temp[default_arg_temp];
        mutable int * _reference;

        const eCargType _type;

        void* _data;
        size_t _size;
    };

    struct cblob {
        void* const _data;
        const size_t _len;
        
        cblob(void * data, size_t len) : _data(data), _len(len) {}
    };

    class cargs {
    public:
        virtual ~cargs() {}

        inline int getCount() const {
            return _args.size();
        }

        inline eCargType getArgType(const int index) const {
            if (index >= _args.size()) {
                return eCargType::__nil__;
            }

            return _args[index].getType();
        }

        inline s64 getInt64(const int index) const {
            if (index >= _args.size() || _args[index].getType() != eCargType::__int64__) {
                return error_value;
            }

            return _args[index].getInt64();
        }

        inline double getDouble(const int index) const {
            if (index >= _args.size() || _args[index].getType() != eCargType::__double__) {
                return error_value;
            }

            return _args[index].getDouble();
        }

        inline bool getBool(const int index) const {
            if (index >= _args.size() || _args[index].getType() != eCargType::__bool__) {
                return false;
            }

            return _args[index].getBool();
        }

        inline std::string getString(const int index) const {
            if (index >= _args.size() || _args[index].getType() != eCargType::__string__) {
                return "";
            }

            return _args[index].getString();
        }

        inline void* geBlob(const int index, size_t& size) const {
            size = 0;
            if (index >= _args.size() || _args[index].getType() != eCargType::__blob__) {
                return nullptr;
            }

            return _args[index].getBlob(size);
        }

        inline void* getData(const int index, size_t& size) const {
            return _args[index].getData(size);
        }

        cargs& operator << (const int value) {
            pushInt64(value);
            return *this;
        }

        cargs& operator << (const long long value) {
            pushInt64(value);
            return *this;
        }

        cargs& operator << (const unsigned int value) {
            pushInt64(value);
            return *this;
        }

        cargs& operator << (const unsigned long long value) {
            pushInt64(value);
            return *this;
        }

        cargs& operator << (const double value) {
            pushDouble(value);
            return *this;
        }

        cargs& operator << (const bool value) {
            pushBool(value);
            return *this;
        }

        cargs& operator << (const std::string & value) {
            pushString(value);
            return *this;
        }

        cargs& operator << (const cblob& value) {
            pushBlob(value._data, value._len);
            return *this;
        }

        cargs& operator << (const tlib::carg& value) {
            _args.push_back(value);
            return *this;
        }

        inline void clear() {
            _args.clear();
        }

    private:
        inline void pushInt64(const s64 value) {
            carg arg(eCargType::__int64__);
            arg.setInt64(value);
            _args.push_back(arg);
        }

        inline void pushBool(const bool value) {
            carg arg(eCargType::__bool__);
            arg.setBool(value);
            _args.push_back(arg);
        }

        inline void pushDouble(const double value) {
            carg arg(eCargType::__double__);
            arg.setDouble(value);
            _args.push_back(arg);
        }

        inline void pushString(const std::string& value) {
            carg arg(eCargType::__string__);
            arg.setString(value);
            _args.push_back(arg);
        }

        inline void pushBlob(void* value, const size_t size) {
            carg arg(eCargType::__blob__);
            arg.setBlob(value, size);
            _args.push_back(arg);
        }

    private:
        std::vector<carg> _args;
    };
}

#endif //__cargs_h__
