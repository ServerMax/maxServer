#ifndef __cbuffer_h__
#define __cbuffer_h__

#include "tools.h"

#define mid_tester 5001

namespace tlib{
    class cbuffer {
    public:
        cbuffer(const unsigned int size) : _size(size), _in(0), _out(0) {
            tassert(size > 0, "wtf");
            _data = tnew char[size];
        }

        ~cbuffer() {
            tdel[] _data;
        }

        inline bool in(const void * data, const unsigned int size) {
            if (size == 0) {
                return true;
            }

            tassert(_in >= _out && _in <= _size, "wtf");
            if (_size - getLength() < size) {
                char * old_data = _data;
                const int old_size = _size;

                while (_size - getLength() < size) {
                    _size *= 2;
                }

                _data = tnew char[_size];

                if (_in - _out > 0) {
                    tools::memery::safeMemcpy(_data, _size, old_data + _out, _in - _out);
                }

                _in -= _out;
                _out = 0;
                tdel old_data;

            }

            if (_size - _in < size) {
                if (_in == _out) {
                    _in = 0;
                    _out = 0;
                } else {
                    printf("cbuffer mem move _size %d, _in %d, _out %d\n", _size, _in, _out);
                    tools::memery::safeMemmove(_data, _size, _data + _out, _in - _out);

                    _in -= _out;
                    _out = 0;
                }
            }

            tassert(_in < _size && _out < _size, "wtf");
            tools::memery::safeMemcpy(_data + _in, _size - getLength(), data, size);
            _in += size;
            tassert(_in <= _size, "wtf");

            return true;
        }

        inline bool out(const unsigned int size) {
            tassert(_out + size <= _in && _in <= _size, "wtf");
            if (_out + size > _in) {
                return false;
            }

            _out += size;
            return true;
        }

        inline const void * getData() const {
            return _data + _out;
        }

        inline const unsigned int getLength() const {
            tassert(_in >= _out && _in <= _size, "wtf");
            return _in - _out;
        }

    private:
        unsigned int _size;
        unsigned int _in;
        unsigned int _out;
        char * _data;
    };
}

#endif //__CircularBuffer_h__
