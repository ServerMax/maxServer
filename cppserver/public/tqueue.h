#ifndef __tqueue_h__
#define __tqueue_h__
#include "multisys.h"
#include "autolock.h"

enum {
    not_exists_data = 0,
    exists_data = 1
};

#define queue_block_size 1024

namespace tlib {
    template <typename type>
    class tqueue {
        struct tspace {
            s8 _sign;
            type _content;
            tspace() : _sign(0) {}
        };
    public:
        tqueue() : _rindex(0), _windex(0), _rcount(0), _wcount(0), _size(queue_block_size) {
            _queue = tnew tspace[_size];
        }

        ~tqueue() {}

        inline void push(type src) {
            autolocker(lock, _write_lock);
            while (_queue[_windex]._sign != not_exists_data) {
                double_queue_size();
            }

            _queue[_windex]._content = src;
            _queue[_windex++]._sign = exists_data;
            _wcount++;
            if (_windex >= _size) {
                _windex = 0;
            }
        }

        inline bool tryPull(type & value) {
            autolocker(lock, _read_lock);
            if (_queue[_rindex]._sign != exists_data) {
                return false;
            }

            value = _queue[_rindex]._content;
            _queue[_rindex++]._sign = not_exists_data;
            _rcount++;

            if (_rindex >= _size) {
                _rindex = 0;
            }

            return true;
        }

        inline bool isEmpty() {
            return (_rcount == _wcount);
        }

        inline int count() {
            return _wcount - _rcount;
        }

    private:
        void double_queue_size() {
            autolocker(rlock, _read_lock);
            autolocker(wlock, _write_lock);

            u32 old_size = _size;
            tspace * old_queue = _queue;

            _size += queue_block_size;
            _queue = tnew tspace[_size];

            if (_rcount != _wcount) {
                if (_windex > _rindex) {
                    //tools::memery::safeMemcpy(_queue, _size * sizeof(tspace), old_queue + _rindex, sizeof(tspace) * (_windex - _rindex));
                    for (int i = 0; i < _windex - _rindex; i++) {
                        _queue[i] = old_queue[_rindex];
                    }
                }
                else if (_windex <= _rindex) {
                    for (int i = 0; i < old_size - _rindex; i++) {
                        _queue[i] = old_queue[_rindex + i];
                    }

                    for (int i = 0; i < _windex; i++) {
                        _queue[old_size - _rindex + i] = old_queue[i];
                    }
                }
                _windex = _wcount - _rcount;
                _rindex = 0;
            }
            tdel[] old_queue;
        }
    private:
        u32 _size;
        tspace * _queue;
        tlib::spinlock _read_lock;
        tlib::spinlock _write_lock;

        u32 _rindex;
        u32 _windex;
        u32 _rcount;
        u32 _wcount;
    };
}
#endif //CQUEUE_H
