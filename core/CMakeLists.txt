set(DORY_MODULE_NAME ${PROJECT_NAME}-core)
add_library(${DORY_MODULE_NAME} STATIC)

target_sources(${DORY_MODULE_NAME}
        PRIVATE
        engine/services/fileService/fileService.cpp
        PUBLIC
            FILE_SET CXX_MODULES
                FILES
                    typeTraits/typeTraits.ixx
                    engine/services/fileService/fileServiceInterface.ixx
        engine/services/fileService/fileService.ixx
)