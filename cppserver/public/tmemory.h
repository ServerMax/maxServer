#ifndef __tmemory_h__
#define __tmemory_h__

#include "multisys.h"
#include "autolock.h"

#ifdef _DEBUG
#include <set>
#endif //_DEBUG

namespace tlib {

    enum eMemoryState {
        inuse,
        freed
    };

    struct oMemoryInfo {
        eMemoryState _state;
        int _size;
        oMemoryInfo * _link_pre;
        oMemoryInfo * _link_next;

        oMemoryInfo * _sort_pre;
        oMemoryInfo * _sort_next;
    };

    template<int template_size>
    class tmemory {
    public:
        tmemory() : _info(new(_memory)oMemoryInfo), _allocer(nullptr) {
            _info->_size = template_size - sizeof(oMemoryInfo);
            _info->_state = eMemoryState::freed;
            
            _info->_link_pre = nullptr;
            _info->_link_next = nullptr;

            _info->_sort_pre = nullptr;
            _info->_sort_next = nullptr;

            _allocer = _info;
        }

        inline void * talloc(const int size) {
            oMemoryInfo* allocer = _allocer;
            while (allocer) {
                if (allocer->_size < size) {
                    allocer = allocer->_sort_next;
                    continue;
                }

                allocer->_size = size;
                allocer->_state = eMemoryState::inuse;

                
            }

            return nullptr;
        }

        inline void trecover(void * m) {

        }

    private:
//         inline void link_correct(oFreeInfo * ofinfo) {
//         }

    private:
        char _memory[template_size];
        const oMemoryInfo * const _info;
        oMemoryInfo * _allocer;
#ifdef _DEBUG
        std::set<oMemoryInfo *> _unit_set;
#endif //_DEBUG
    };
}

#endif //__tmemory_h__
