#ifndef __logic_h__
#define __logic_h__

#include "api.h"

namespace tcore {
    class logic {
    public:
        static logic * getInstance();

        virtual bool launch();
        virtual void proces(const s32 overtiem) {}
        virtual void shutdown();

        virtual iModule * findModule(const char * name);

    private:
        iModule * loadModule(const std::string & path, const std::string & name);
        bool loadModuleFromXML(const std::string & xmlpath);
        bool loadModuleFromArgs();
    };
}


#endif //__logic_h__
