#include "filer.h"
#include <malloc.h>

filer::~filer() {
    close();
}

bool filer::open(const char * filename) {
    if (0 != fopen_s(&_fp, filename, "rb+") || nullptr == _fp) {
        return false;
    }

    fseek(_fp, 0L, SEEK_END);
    _size = ftell(_fp);
    fseek(_fp, 0L, 0);

    _data = (char *)malloc(_size);
    fread(_data, _size, (size_t)1, _fp);
    return true;
}

void filer::close() {
    if (_fp) {
        fclose(_fp);
        _fp = nullptr;
    }

    if (_data) {
        free(_data);
        _data = nullptr;
    }

    _size = 0;
}

bool filer::save() {
    fseek(_fp, 0L, 0);
    int size = fwrite(_data, _size, (size_t)1, _fp);
    if (size != 1) {
        return false;
    }
    fflush(_fp);
    return true;
}

char * filer::getContent() {
    return _data;
}

int filer::size() {
    return _size;
}
