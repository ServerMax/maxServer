#include <stdio.h>
#include <map>
#include <assert.h>
#include <string>
#include "Header.h"
#include "MD5.h"
#include "filer.h"
#include "json/json.h"
using namespace std;

static map<string, string> _args;

enum eOption {
    encrypt,
    deciphering
};

int main(int argc, const char ** args, const char ** env) {
    for (int i = 1; i < argc; ++i) {
        if (strncmp(args[i], "--", 2) == 0) {
            const char * start = args[i] + 2;
            const char * equal = strstr(start, "=");
            if (equal != nullptr) {
                std::string name(start, equal);
                std::string val(equal + 1);
                _args[name] = val;
            } else if (strlen(args[i]) > 2) {
                _args[args[i] + 2] = "";
            }
        }
    }

    if (_args.find("pause") != _args.end()) {
        getchar();
    }

    string path = std::string(tools::file::getApppath()) + "/" + _args["path"];
    tools::file::opaths pathes, names;
    int count;

    if (_args.find("enc") != _args.end()) {
        string extension = _args["extension"];
        eOption type;
        if (strcmp(_args["type"].c_str(), "encrypt") == 0) {
            type = eOption::encrypt;
        } else if (strcmp(_args["type"].c_str(), "deciphering") == 0) {
            type = eOption::deciphering;
        } else {
            assert(false);
            return -1;
        }

        if (tools::file::getfiles(path.c_str(), extension.c_str(), pathes, names, count)) {
            for (int i = 0; i < count; i++) {
                filer filer;
                if (!filer.open(pathes[i].c_str())) {
                    assert(false);
                    continue;
                }
                char * data = filer.getContent();
                int size = filer.size();
                for (int i = 0; i < size; i++) {
                    if (i <= 100 || i % 2 == 0 || i % 3 == 0 || i % 6 == 0 || i % 7 == 0) {
                        continue;
                    }

                    if (type == eOption::encrypt) {
                        data[i] += i;
                    } else {
                        data[i] -= i;
                    }
                }

                filer.save();
                filer.close();
            }
        }
    }


    if (_args.find("md5") != _args.end()) {
        pathes.clear();
        names.clear();

        std::string folder = _args["folder"];
        Json::Value root;
        root["version"] = folder.c_str();

        Json::Value json;
        if (tools::file::getfiles(path.c_str(), "*", pathes, names, count)) {
            for (int i = 0; i < count; i++) {
                printf("%s : %s\n", pathes[i].c_str() + path.size(), tlib::md5file(pathes[i].c_str()).c_str());
                json[pathes[i].c_str() + path.size()] = tlib::md5file(pathes[i].c_str()).c_str();
            }
        }
        root["md5"] = json;

        std::string resmd5es = root.toStyledString();
        //printf("%s\n", .c_str());
        std::string md5filepath = string(tools::file::getApppath()) + "/" + _args["md5file"];
        FILE * md5file = fopen(md5filepath.c_str(), "w");
        if (nullptr == md5file) {
            tassert(md5file, "wtf");
            return false;
        }

        fwrite(resmd5es.c_str(), resmd5es.size(), 1, md5file);
        fflush(md5file);
        fclose(md5file);
    }
    
    return 0;
}
