#cmake file for project kernel
#author:^
#created:%

include_directories(
    "${PROJECT_SOURCE_DIR}/logic/module/navigation"
)

SET(recast
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourAlloc.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourAlloc.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourAssert.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourAssert.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourCommon.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourCommon.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourMath.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourNavMesh.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourNavMesh.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourNavMeshBuilder.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourNavMeshBuilder.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourNavMeshQuery.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourNavMeshQuery.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourNode.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourNode.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourStatus.h"

    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourTileCache.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourTileCache.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourTileCacheBuilder.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/DetourTileCacheBuilder.cpp"

    "${PROJECT_SOURCE_DIR}/logic/module/navigation/Recast.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/Recast.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastAlloc.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastAlloc.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastArea.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastAssert.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastContour.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastFilter.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastLayers.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastMesh.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastMeshDetail.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastRasterization.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastRegion.cpp"    
    
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/RecastAssert.h"
)

SET(navigation
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/header.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/navigation.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/navigation.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/navmesh.h"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/navmesh.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/navigation/main.cpp"
)

source_group(api FILES ${api})
source_group(common FILES ${common})
source_group(interface FILES ${interface})
source_group(public FILES ${public})
source_group(public\\tinyxml FILES ${tinyxml})
source_group(proto FILES ${proto})
source_group(recast FILES ${recast})    
source_group(\\ FILES ${navigation}) 

ADD_LIBRARY(navigation SHARED
    ${navigation}
    ${recast}
    ${api}
    ${common}
    ${interface}
    ${tinyxml}
    ${public}
)

SET_PROPERTY(TARGET navigation PROPERTY FOLDER "support")
