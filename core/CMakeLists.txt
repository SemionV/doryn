set(DORY_MODULE_NAME ${PROJECT_NAME}-core)
add_library(${DORY_MODULE_NAME} STATIC)

target_sources(${DORY_MODULE_NAME} PUBLIC
        FILE_SET core_modules TYPE CXX_MODULES
        FILES
        typeTraits/typeTraits.cppm
        engine/services/fileService/fileServiceInterface.cppm
        engine/services/fileService/fileService.cppm
)

target_sources(${DORY_MODULE_NAME} PRIVATE
        engine/services/fileService/fileService.cpp
)

target_compile_features(${DORY_MODULE_NAME} PUBLIC cxx_std_23)