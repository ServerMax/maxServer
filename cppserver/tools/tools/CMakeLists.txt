#cmake file for project core
#author:alax
#created:2015-05-12 15:07:04

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/api"
    "${PROJECT_SOURCE_DIR}/public")
    
SET(tools
    "${PROJECT_SOURCE_DIR}/tools/tools/header.h"
    "${PROJECT_SOURCE_DIR}/tools/tools/filer.h"
    "${PROJECT_SOURCE_DIR}/tools/tools/filer.cpp"
    "${PROJECT_SOURCE_DIR}/tools/tools/main.cpp"
)

SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${tools})

ADD_EXECUTABLE(tools
    ${api}
    ${tinyxml}
    ${public}
    ${json}
    ${tools}
)

SET_PROPERTY(TARGET tools PROPERTY FOLDER "tools")
