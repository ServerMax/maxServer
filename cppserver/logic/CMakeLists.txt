#cmake file for project core
#author:T
#created:2015-05-12 15:07:04
include_directories(
    "${PROJECT_SOURCE_DIR}/logic/common"
    "${PROJECT_SOURCE_DIR}/logic/protobuffer/"
    "${PROJECT_SOURCE_DIR}/logic/protobuffer/cpp/"
    "${PROJECT_SOURCE_DIR}/logic/interface"
)

set(common

)

set(proto
    "${PROJECT_SOURCE_DIR}/logic/protobuffer/cpp/test.pb.h"
    "${PROJECT_SOURCE_DIR}/logic/protobuffer/cpp/test.pb.cc"
)

set(interface
    ${interface}
    "${PROJECT_SOURCE_DIR}/logic/interface/iSample.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iNavigation.h"
)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/module/navigation)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/module/sample)