cmake_minimum_required(VERSION 3.18)
project(VulkanEngine)

if(NOT CMAKE_BUILD_TYPE) #Default to release if the user passes nothing.
        set(CMAKE_BUILD_TYPE "Release")
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VulkanEngine)

set(INCLUDE_DIRECTORIES ../include/ ./vkEngine/)
set(PLATFORM_LIBS ${CMAKE_DL_LIBS})
set(SRC_FILES
    vkEngine/vkEngine.h
    MainWindows.cpp
    vkEngine/vk_getProcAddrs.h vkEngine/vk_getProcAddrs.cpp
    FragShader.frag VertShader.vert
    EngineExample.cpp EngineExample.h)

add_executable(VulkanEngine WIN32 ${SRC_FILES}
    vkEngine/vkStructs.h
    vkEngine/vkLayers.h
    vkEngine/vkExtensions.h
    vkEngine/vkDevice.h
    vkEngine/vkQueues.h
    vkEngine/vkSurfaces.h
    vkEngine/vkMemory.h
    vkEngine/vkTextures.h
    vkEngine/vkShaders.h
    vkEngine/vkGLTF.h
    vkEngine/vkRenderPass.h
    vkEngine/vkDescriptor.h
    vkEngine/vkPipeline.h
    vkEngine/vkFences.h
    vkEngine/vkCommandBuffer.h
    vkEngine/vkCloseDown.h
    vkEngine/dds-ktx.h
    vkEngine/tiny_gltf.h
    vkEngine/vkMath.h
)

add_custom_command(TARGET ${PROJECT_NAME}
    PRE_BUILD
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/VertShader.vert ${CMAKE_CURRENT_SOURCE_DIR}/FragShader.frag
    COMMAND C:/dev/vulcan/spirv-tools/bin/glslangvalidator -V -o ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv --target-env vulkan1.0 -S vert ${CMAKE_CURRENT_SOURCE_DIR}/VertShader.vert
    COMMAND C:/dev/vulcan/spirv-tools/bin/glslangvalidator -V -o ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv --target-env vulkan1.0 -S frag ${CMAKE_CURRENT_SOURCE_DIR}/FragShader.frag
)

set_target_properties(VulkanEngine PROPERTIES CXX_STANDARD 14)

target_link_libraries(VulkanEngine ${PLATFORM_LIBS}) # for dlopen and dlclose
target_include_directories(VulkanEngine PRIVATE ${INCLUDE_DIRECTORIES})
target_compile_definitions(VulkanEngine PRIVATE $<$<CONFIG:Debug>:DEBUG=1> $<$<NOT:$<CONFIG:Debug>>:RELEASE=1> ) #Defines DEBUG=1 or RELEASE=1

