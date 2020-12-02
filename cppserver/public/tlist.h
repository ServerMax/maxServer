#ifndef __tlist_h__
#define __tlist_h__

#include "multisys.h"
#include "tpool.h"
#include "autolock.h"

#define recouver_list_size 64

namespace tlib {
    template<typename T>
    class node {
    public:
        node(const T & value) : _next(nullptr), _value(value) {}

        inline void link(node * previous, node * next) {
            autolocker(lock, _lock);
            _previous = previous;
            _next = next;
        }

        inline node * get_next() {
            return _next;
        }

        inline node * get_previous() {
            return _previous;
        }

        inline const T & value() {
            return _value;
        }

    private:
        T _value;
        node * _previous;
        node * _next;
        spinlock _lock;
    };

    template<typename T, int size = 64>
    class tlist {
    public:
        inline void push(const T & t) {
            autolocker(begin_lock, _lock);
            if (nullptr == _begin) {
                tassert(nullptr == _end, "wtf");
                _begin = create_node(t);
                _begin->link(nullptr, nullptr);
                _end = _begin;
            }
            else {
                node<T> * pusher = create_node(t);
                pusher->link(nullptr, _begin);
                _begin->link(pusher, _begin->get_next());
                _begin = pusher;
            }
        }

        inline bool pop(T & t) {
            autolocker(lock, _lock);
            if (nullptr == _end) {
                return false;
            }

            node<T> * poper = _end->get_previous();
            t = _end->value();
            recover_node(_end);
            _end = poper;
            if (_end) {
                _end->link(_end->get_previous(), nullptr);
            }
            else {
                _begin = _end;
            }

            return true;
        }

        tlist() : _begin(nullptr), _end(nullptr) {

        }

    private:
        inline node<T> * create_node(const T & value) {
#ifdef _DEBUG
            node<T> * n = tnew node<T>(value);
#else
            node<T> * n = create_from_pool(_node_pool, value);
#endif //_DEBUG
            return n;
        }

        inline void recover_node(node<T> * n) {
#ifdef _DEBUG
            tdel n;
#else
            recover_to_pool(_node_pool, *i);
#endif
        }
        
    private:
        node<T> * _begin;
        node<T> * _end;
        spinlock _lock;

        tlib::tpool<node<T>> _node_pool;
        std::list<node<T> *> _recover;
    };
}

#endif //__tlist_h__
