#include "navigation.h"
#include "navmesh.h"
#include "Header.h"
#include <map>
tcore::api::iCore * g_core = nullptr;

static std::map<std::string, iNavmesh *> s_mesh_map;

bool navigation::initialize(api::iCore * core) {
    g_core = core;

    char cPath[512] = { 0 };
    safesprintf(cPath, sizeof(cPath), "%s/%s/navmesh/", tools::file::getApppath(), core->getEnv());
    tools::file::opaths paths;
    oNameArray names;
    s32 count = 0;

    if (tools::file::getfiles(cPath, ".navmesh", paths, names, count)) {
        tassert(paths.size() == count && names.size() == count, "wtf");
        for (s32 i = 0; i < count; i++) {
            debug(g_core, "mesh file : %s, %s", paths[i].c_str(), names[i].c_str());
            navmesh * mesh = navmesh::create(paths[i].c_str());
            tassert(mesh, "wtf");
            s_mesh_map.insert(std::make_pair(names[i].c_str(), mesh));
            trace(g_core, "load mesh %s", names[i].c_str());
        }
    }
    return true;
}

bool navigation::launch(api::iCore * core) {
    return true;
}

bool navigation::destroy(api::iCore * core) {
    return true;
}

iNavmesh * navigation::findNavmesh(const char * name) {
    std::map<std::string, iNavmesh *>::iterator itor = s_mesh_map.find(name);
    if (itor == s_mesh_map.end()) {
        return nullptr;
    }

    return itor->second;
}
