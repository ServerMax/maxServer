#cmake file for project core
#author : max
#created : 2020-12-02 16:17:10

set(sample
    "${PROJECT_SOURCE_DIR}/logic/module/sample/header.h"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/sample.h"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/sample.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/tcpSession.h"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/tcpSession.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/tcpServer.h"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/tcpServer.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/timerTest.h"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/timerTest.cpp"
    "${PROJECT_SOURCE_DIR}/logic/module/sample/main.cpp"
)

source_group(api FILES ${api})
source_group(common FILES ${common})
source_group(interface FILES ${interface})
source_group(public FILES ${public})
source_group(\\ FILES ${sample})

add_library(sample SHARED
    ${sample}
    ${api}
    ${public}
    ${interface}
    ${common}
)

set_property(TARGET sample PROPERTY FOLDER "test")
