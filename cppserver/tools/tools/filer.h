#ifndef __filer_h__
#define __filer_h__

#include <stdio.h>

class filer {
public:
    filer() : _fp(nullptr), _data(nullptr), _size(0) {}
    ~filer();

    bool open(const char * filename);
    void close();
    bool save();

    char * getContent();
    int size();

private:
    FILE * _fp;
    char * _data;
    int _size;
};

#endif //__Filer_h__
