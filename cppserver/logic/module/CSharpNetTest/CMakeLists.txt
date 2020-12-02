#cmake file for project core
#author : mx
#created : 2020-12-01 09:14:04

set(CSharpNetTest
    "${PROJECT_SOURCE_DIR}/logic/module/CSharpNetTest/header.h"
    "${PROJECT_SOURCE_DIR}/logic/module/CSharpNetTest/CSharpNetTest.h"
    "${PROJECT_SOURCE_DIR}/logic/module/CSharpNetTest/CSharpNetTest.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/CSharpNetTest/main.cpp"
)

source_group(api FILES ${api})
source_group(common FILES ${common})
source_group(interface FILES ${interface})
source_group(public FILES ${public})
source_group(proto FILES ${proto})
source_group(\\ FILES ${CSharpNetTest})

add_library(CSharpNetTest SHARED
    ${CSharpNetTest}
    ${api}
    ${public}
    ${proto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(CSharpNetTest
    ${libprotobuf}
)

set_property(TARGET CSharpNetTest PROPERTY FOLDER "test")
