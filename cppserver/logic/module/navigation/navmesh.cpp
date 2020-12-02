#include "navmesh.h"    
#include "tools.h"

navmesh::navmesh() { 
    tools::memery::safeMemcpy(_polys, sizeof(_polys), 0, sizeof(_polys));
}

navmesh::~navmesh() {
    std::vector<dtNavMesh*>::iterator iter = _navmesh_layers.begin();
    for(; iter != _navmesh_layers.end(); iter++)
        dtFreeNavMesh((*iter));

    std::vector<dtNavMeshQuery*>::iterator iter1 = _navmeshQuery_layers.begin();
    for(; iter1 != _navmeshQuery_layers.end(); iter1++)
        dtFreeNavMeshQuery((*iter1));

}

s32 navmesh::findStraightPath(s32 layer, const geometry::vector3 & start, const geometry::vector3 & end, std::vector<geometry::vector3> & paths) const {
    if(layer >= (s32)_navmeshQuery_layers.size()) {
        return -1;
    }

    dtNavMeshQuery* navmeshQuery = _navmeshQuery_layers[layer];
    // dtNavMesh* 

    float spos[3];
    spos[0] = start.x;
    spos[1] = start.y;
    spos[2] = start.z;

    float epos[3];
    epos[0] = end.x;
    epos[1] = end.y;
    epos[2] = end.z;

    dtQueryFilter filter;
    filter.setIncludeFlags(0xffff);
    filter.setExcludeFlags(0);

    const float extents[3] = {1.f, 4.f, 1.f};

    dtPolyRef startRef = INVALID_NAVMESH_POLYREF;
    dtPolyRef endRef = INVALID_NAVMESH_POLYREF;

    float nearestPt[3];
    navmeshQuery->findNearestPoly(spos, extents, &filter, &startRef, nearestPt);
    navmeshQuery->findNearestPoly(epos, extents, &filter, &endRef, nearestPt);

    if (!startRef || !endRef) {
        return NAV_ERROR_NEARESTPOLY;
    }

    dtPolyRef polys[MAX_POLYS];
    s32 npolys;
    float straightPath[MAX_POLYS * 3];
    u8 straightPathFlags[MAX_POLYS];
    dtPolyRef straightPathPolys[MAX_POLYS];
    s32 nstraightPath;
    s32 pos = 0;

    navmeshQuery->findPath(startRef, endRef, spos, epos, &filter, polys, &npolys, MAX_POLYS);
    nstraightPath = 0;

    if (npolys) {
        float epos1[3];
        dtVcopy(epos1, epos);
                
        if (polys[npolys-1] != endRef)
            navmeshQuery->closestPointOnPoly(polys[npolys-1], epos, epos1, 0);
                
        navmeshQuery->findStraightPath(spos, epos, polys, npolys, straightPath, straightPathFlags, straightPathPolys, &nstraightPath, MAX_POLYS);
        for(s32 i = 0; i < nstraightPath * 3; ) {
            geometry::vector3 currpos;
            currpos.x = straightPath[i++];
            currpos.y = straightPath[i++];
            currpos.z = straightPath[i++];
            paths.push_back(currpos);
            pos++;

            //DebugLog(g_core, "navmesh::findStraightPath: {%d}->{%f}, {%f}, {%f}", pos, currpos.x, currpos.y, currpos.z);
        }
    }

    return pos;
}

s32 navmesh::raycast(s32 layer, const geometry::vector3 & start, const geometry::vector3 & end, geometry::vector3 & hit) const {
    dtNavMeshQuery* navmeshQuery = _navmeshQuery_layers[layer];

    float hitPoint[3];
    float spos[3];
    spos[0] = start.x;
    spos[1] = start.y;
    spos[2] = start.z;

    float epos[3];
    epos[0] = end.x;
    epos[1] = end.y;
    epos[2] = end.z;

    dtQueryFilter filter;
    filter.setIncludeFlags(0xffff);
    filter.setExcludeFlags(0);

    const float extents[3] = { 1.f, 2.f, 1.f };

    dtPolyRef startRef = INVALID_NAVMESH_POLYREF;

    float nearestPt[3];
    navmeshQuery->findNearestPoly(spos, extents, &filter, &startRef, nearestPt);

    if (!startRef) {
        hit = start;
        return NAV_ERROR_NEARESTPOLY;
    }

    float t = 0;
    float hitNormal[3];
    memset(hitNormal, 0, sizeof(hitNormal));

    dtPolyRef polys[MAX_POLYS];
    int npolys;

    navmeshQuery->raycast(startRef, spos, epos, &filter, &t, hitNormal, polys, &npolys, MAX_POLYS);

    if (t > 1) {
        hitPoint[0] = epos[0];
        hitPoint[1] = epos[1];
        hitPoint[2] = epos[2];
    } else {
        // Hit
        hitPoint[0] = spos[0] + (epos[0] - spos[0]) * t;
        hitPoint[1] = spos[1] + (epos[1] - spos[1]) * t;
        hitPoint[2] = spos[2] + (epos[2] - spos[2]) * t;
        if (npolys) {
            float h = 0;
            navmeshQuery->getPolyHeight(polys[npolys - 1], hitPoint, &h);
            hitPoint[1] = h;
        }
    }

    hit.x = hitPoint[0];
    hit.y = hitPoint[1];
    hit.z = hitPoint[2];
    return 1;
}

bool navmesh::check(s32 layer, const geometry::vector3 & point) const {
    dtNavMeshQuery* navmeshQuery = _navmeshQuery_layers[layer];

    float pos[3];
    pos[0] = point.x;
    pos[1] = point.y;
    pos[2] = point.z;

    dtQueryFilter filter;
    filter.setIncludeFlags(0xffff);
    filter.setExcludeFlags(0);

    const float extents[3] = { 0.1f, 0.1f, 0.1f };

    dtPolyRef startRef = INVALID_NAVMESH_POLYREF;

    float nearestPt[3];
    navmeshQuery->findNearestPoly(pos, extents, &filter, &startRef, nearestPt);
    if (!startRef || !geometry::t2d::isSamePosition(point.x, point.z, nearestPt[0], nearestPt[2])) {
        return false;
    }
    
    return true;
}

bool navmesh::findnearPoint(const geometry::vector3 & center, geometry::vector3 & target, const float radius) const {
    dtNavMeshQuery* navmeshQuery = _navmeshQuery_layers[0];

    float pos[3];
    pos[0] = center.x;
    pos[1] = center.y;
    pos[2] = center.z;

    dtQueryFilter filter;
    filter.setIncludeFlags(0xffff);
    filter.setExcludeFlags(0);

    const float extents[3] = { radius * 0.75f, radius * 0.75f, radius * 0.75f };

    dtPolyRef startRef = INVALID_NAVMESH_POLYREF;

    float nearestPt[3];
    navmeshQuery->findNearestPoly(pos, extents, &filter, &startRef, nearestPt);
    if (!startRef || !geometry::t2d::calcDistance(center.x, center.z, nearestPt[0], nearestPt[2]) > radius) {
        return false;
    }
    target.x = nearestPt[0];
    target.y = nearestPt[1];
    target.z = nearestPt[2];
    return true;
}

navmesh* navmesh::create(std::string res) {
    if(res.empty())
        return nullptr;
    
    navmesh* pNavMeshHandle = nullptr;
    std::vector<std::string> results;
    results.push_back(res);

    if(results.size() == 0) {
        return nullptr;
    }

    FILE* fp = fopen(res.c_str(), "rb");
    if (!fp) {
        error(g_core, "navmesh::create: open %s is error!", res.c_str());
        return nullptr;
    }

    bool safeStorage = true;
    int pos = 0;
    int size = sizeof(NavMeshSetHeader);

    fseek(fp, 0, SEEK_END);
    size_t flen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    u8* data = new u8[flen];
    size_t readsize = fread(data, 1, flen, fp);
    if (readsize != flen) {
        error(g_core, "navmesh::create: open %s, read size= %d != %d error!", res.c_str(), readsize, flen);
        fclose(fp);
        safe_release_array(data);
        return nullptr;
    }

    if (readsize < sizeof(NavMeshSetHeader)) {
        error(g_core, "navmesh::create: open %s, NavMeshSetHeader is error!", res.c_str());
        fclose(fp);
        safe_release_array(data);
        return nullptr;
    }

    NavMeshSetHeader header;
    memcpy(&header, data, size);
    pos += size;
    if (header.version != navmesh::RCN_NAVMESH_VERSION) {
        error(g_core, "navmesh::create: navmesh version %ld is not match %d!", header.version, (int)navmesh::RCN_NAVMESH_VERSION);
        fclose(fp);
        safe_release_array(data);
        return nullptr;
    }

    dtNavMesh* mesh = dtAllocNavMesh();
    if (!mesh) {
        error(g_core, "navmesh::create: dtAllocNavMesh is failed!");
        fclose(fp);
        safe_release_array(data);
        return nullptr;
    }

    dtStatus status = mesh->init(&header.params);
    if (dtStatusFailed(status)) {
        error(g_core, "navmesh::create: mesh init is error %d !", status);
        fclose(fp);
        safe_release_array(data);
        return nullptr;
    }

    // Read tiles.
    bool success = true;
    for (int i = 0; i < header.tileCount; ++i) {
        NavMeshTileHeader tileHeader;
        size = sizeof(NavMeshTileHeader);
        memcpy(&tileHeader, &data[pos], size);
        pos += size;

        size = tileHeader.dataSize;
        if (!tileHeader.tileRef || !tileHeader.dataSize) {
            success = false;
            status = DT_FAILURE + DT_INVALID_PARAM;
            break;
        }

        unsigned char* tileData = (unsigned char*)dtAlloc(size, DT_ALLOC_PERM);
        if (!tileData) {
            success = false;
            status = DT_FAILURE + DT_OUT_OF_MEMORY;
            break;
        }
        memcpy(tileData, &data[pos], size);
        pos += size;

        status = mesh->addTile(tileData
            , size
            , (safeStorage ? DT_TILE_FREE_DATA : 0)
            , tileHeader.tileRef
            , 0);

        if (dtStatusFailed(status)) {
            success = false;
            break;
        }
    }

    fclose(fp);
    safe_release_array(data);

    if (!success) {
        error(g_core, "navmesh::create:  error %d!", status);
        dtFreeNavMesh(mesh);
        return nullptr;
    }

    dtNavMeshQuery* pMavmeshQuery = new dtNavMeshQuery();
    pMavmeshQuery->init(mesh, 1024);
    pNavMeshHandle = new navmesh();
    pNavMeshHandle->_navmeshQuery_layers.push_back(pMavmeshQuery);
    pNavMeshHandle->_navmesh_layers.push_back(mesh);

    u32 tileCount = 0;
    u32 nodeCount = 0;
    u32 polyCount = 0;
    u32 vertCount = 0;
    u32 triCount = 0;
    u32 triVertCount = 0;
    u32 dataSize = 0;

    const dtNavMesh* navmesh = mesh;
    for (s32 i = 0; i < navmesh->getMaxTiles(); ++i) {
        const dtMeshTile* tile = navmesh->getTile(i);
        if (!tile || !tile->header)
            continue;

        tileCount++;
        nodeCount += tile->header->bvNodeCount;
        polyCount += tile->header->polyCount;
        vertCount += tile->header->vertCount;
        triCount += tile->header->detailTriCount;
        triVertCount += tile->header->detailVertCount;
        dataSize += tile->dataSize;
    }

    return pNavMeshHandle;
}
