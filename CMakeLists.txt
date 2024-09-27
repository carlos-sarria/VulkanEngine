cmake_minimum_required(VERSION 3.18)
project(VulkanEngine)

if(NOT CMAKE_BUILD_TYPE) #Default to release if the user passes nothing.
	set(CMAKE_BUILD_TYPE "Release")
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VulkanEngine)

set(INCLUDE_DIRECTORIES ../include/)
set(PLATFORM_LIBS ${CMAKE_DL_LIBS})
set(SRC_FILES
    MainWindows.cpp MainWindows.h
    vk_getProcAddrs.h vk_getProcAddrs.cpp
    VulkanEngine.cpp VulkanEngine.h VertShader.h FragShader.h)

add_executable(VulkanEngine WIN32 ${SRC_FILES}

    vkLayers.h
    vkExtensions.h
    vkInit.h
    vkQueues.h
    vkSurfaces.h
    vkTextures.h
    vkMemory.h
    vkShaders.h
    vkGeometry.h
    vkRenderPass.h
    vkDescriptor.h
    vkPipeline.h
    vkCommandBuffer.h
    vkFences.h
    vkEngine.h
    vkStructs.h
)

set_target_properties(VulkanEngine PROPERTIES CXX_STANDARD 14)

target_link_libraries(VulkanEngine ${PLATFORM_LIBS}) # for dlopen and dlclose
target_include_directories(VulkanEngine PRIVATE ${INCLUDE_DIRECTORIES})
target_compile_definitions(VulkanEngine PRIVATE $<$<CONFIG:Debug>:DEBUG=1> $<$<NOT:$<CONFIG:Debug>>:RELEASE=1> ) #Defines DEBUG=1 or RELEASE=1


