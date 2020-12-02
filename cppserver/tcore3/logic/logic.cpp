#include "logic.h"
#include "core.h"
#include "tools.h"
#include "tinyxml/tinyxml2.h"

#include <string>
#include <map>
#include <list>
typedef std::list<iModule *> MODULE_LIST;
typedef std::map<std::string, iModule *> MODULE_MAP;
static MODULE_LIST static_module_list;
static MODULE_MAP static_module_map;

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif //linux

namespace tcore {
    logic * logic::getInstance() {
        static logic * p = nullptr;
        if (nullptr == p) {
            p = tnew logic;
            p->launch();
        }

        return p;
    }

    bool logic::launch() {
        core * core = core::getInstance();

        if (core->getArgs("modules")) {
            loadModuleFromArgs();
        } else {
            std::string configpath;
            configpath.append(tools::file::getApppath()).append("/server_config/").append(core->getCorename()).append("/core/module.xml");
            loadModuleFromXML(configpath.c_str());
        }

        return true;
    }

    void logic::shutdown() {
        MODULE_LIST::iterator itor = static_module_list.begin();
        MODULE_LIST::iterator iend = static_module_list.end();

        while (itor != iend) {
            (*itor)->destroy(core::getInstance());
            itor++;
        }

        while (itor != iend) {
            tdel *itor;
            itor++;
        }
    }

    iModule * logic::findModule(const char * name) {
        MODULE_MAP::iterator itor = static_module_map.find(name);
        if (itor == static_module_map.end() || nullptr == itor->second) {
            return nullptr;
        }

        return itor->second;
    }

    iModule * logic::loadModule(const std::string & path, const std::string & name) {
#ifdef WIN32
        std::string modulepath = path + "/" + name + ".dll";
        HINSTANCE hinst = ::LoadLibrary(modulepath.c_str());
        if (!hinst) {
            printf("load %s error %s\n", modulepath.c_str(), strerror(::GetLastError()));
        }
        getModule fun = (getModule)::GetProcAddress(hinst, "getModules");
#else
        std::string modulepath = path + "/lib" + name + ".so";
        void * handle = dlopen(modulepath.c_str(), RTLD_LAZY);
        if (!handle) {
            printf("load %s error %s\n", modulepath.c_str(), dlerror());
        }
        getModule fun = (getModule)dlsym(handle, "getModules");
#endif //WIN32
        tassert(fun, "get function:GetLogicModule error");
        iModule * logic = fun();
        tassert(logic, "can't get module from lib%s.so", path.c_str());
        return logic;
    }

    bool logic::loadModuleFromXML(const std::string & xmlpath) {
        tinyxml2::XMLDocument config;
        if (tinyxml2::XML_SUCCESS != config.LoadFile(xmlpath.c_str())) {
            tassert(false, "where is %s", xmlpath.c_str());
            return false;
        }

        tinyxml2::XMLElement * root = config.RootElement();
        tassert(root, "wtf");
        const char * path = root->Attribute("path");
        if (nullptr == path) {
            path = "";
        }

        tinyxml2::XMLElement * module = root->FirstChildElement("module");
        while (module) {
            iModule * m = loadModule(tools::file::getApppath(), module->Attribute("name"));
            while (m) {
                MODULE_MAP::iterator itor = static_module_map.find(m->getName());
                if (itor != static_module_map.end()) {
                    tassert(false, "module %s is exists", m->getName());
                    continue;
                }

                static_module_map.insert(std::make_pair(m->getName(), m));
                static_module_list.push_back(m);
                m->initialize(core::getInstance());
                m = m->getNext();
            }


            module = module->NextSiblingElement("module");
        }

        MODULE_LIST::iterator itor = static_module_list.begin();
        MODULE_LIST::iterator iend = static_module_list.end();

        while (itor != iend) {
            (*itor)->launch(core::getInstance());
            itor++;
        }
    }

    bool logic::loadModuleFromArgs() {
        std::string module_path = tools::file::getApppath();
        if (core::getInstance()->getArgs("modules_path")) {
            module_path = core::getInstance()->getArgs("modules_path");
        }

        std::string module_string = core::getInstance()->getArgs("modules");

        tools::osplitres res;
        const s32 count = tools::split(module_string, ";", res);
        for (s32 i = 0; i < count; i++) {
            iModule * m = loadModule(module_path, res[i]);
            if (nullptr == m) {
                return false;
            }

            static_module_map.insert(std::make_pair(m->getName(), m));
            static_module_list.push_back(m);
        }

        MODULE_LIST::iterator itor = static_module_list.begin();
        MODULE_LIST::iterator iend = static_module_list.end();
        while (itor != iend) {
            (*itor)->initialize(core::getInstance());
            itor++;
        }

        itor = static_module_list.begin();
        while (itor != iend) {
            (*itor)->launch(core::getInstance());
            itor++;
        }

        return true;
    }
}
