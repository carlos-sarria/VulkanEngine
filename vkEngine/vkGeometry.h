#ifndef VKGEOMETRY_H
#define VKGEOMETRY_H

#include "tiny_gltf.h"

#include "vkStructs.h"
#include "vkMemory.h"

static bool myTextureLoadingFunction(tinygltf::Image *image, const int image_idx, std::string * err,
        std::string * warn, int req_width, int req_height,
        const unsigned char * bytes, int size, void* user_data)
{
    return true;
}

/// <summary>Defines the vertices of a simple triangle which can be passed to the vertex shader to be rendered on screen</summary>
inline void _initVertexBuffers(AppManager& appManager)
{
#if 0
    // Calculate the size of the vertex buffer to be passed to the vertex shader.
    appManager.vertexBuffer.size = sizeof(Vertex) * 6;

    // Set the values for the triangle's vertices.
    Vertex triangle[6];
    // triangle[0] = { -0.5f, -0.288f, 0.0f, 1.0f, 0.0f, 0.0f };
    // triangle[1] = { .5f, -.288f, 0.0f, 1.0f, 1.0f, 0.0f };
    // triangle[2] = { 0.0f, .577f, 0.0f, 1.0f, 0.5f, 1.0f };
    triangle[0] = { 1.000000f, -1.000000f, 0.0f, 1.0f, 0.0f, 0.0f };
    triangle[1] = { -1.000000f, 1.000000f, 0.0f, 1.0f, 1.0f, 0.0f };
    triangle[2] = { -1.000000f, -1.000000f, 0.0f, 1.0f, 0.5f, 1.0f };
    triangle[3] = { 1.000000f, -1.000000f, 0.0f, 1.0f, 0.0f, 0.0f };
    triangle[4] = { 1.000000f, 1.000000f, 0.0f, 1.0f, 1.0f, 0.0f };
    triangle[5] = { -1.000000f, 1.000000f, 0.0f, 1.0f, 0.5f, 1.0f };

    appManager.vertexCount = 6;

    // Create the buffer that will hold the data and be passed to the shaders.
    _createBuffer(appManager, appManager.vertexBuffer, reinterpret_cast<uint8_t*>(triangle), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
#else
    // Load GLTF
    tinygltf::Model model;
    tinygltf::TinyGLTF gltf_ctx;
    std::string err;
    std::string warn;
    const char* file_name = "..\\..\\suzanne.glb";

    gltf_ctx.SetImageLoader(myTextureLoadingFunction, nullptr);

    gltf_ctx.SetStoreOriginalJSONForExtrasAndExtensions(false);

    bool ret = false;
    ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn, file_name);

    if(!ret){
        Log(true, ("GLTF - "+err).c_str());
        exit(1);
    }

    Vertex *Geometry;

    const tinygltf::Scene &scene = model.scenes[model.defaultScene];
    for(tinygltf::Node node : model.nodes)
    {
        Log(false, ("NODE NAME "+node.name).c_str());

        tinygltf::Mesh mesh = model.meshes[node.mesh];
        Log(false, ("MESH NAME "+mesh.name).c_str());


        for (tinygltf::Primitive primitive : mesh.primitives)
        {
            const tinygltf::Accessor accessor_indices = model.accessors[primitive.indices];
            const tinygltf::BufferView bufferView_indices = model.bufferViews[accessor_indices.bufferView];
            const short* buffer_indices = reinterpret_cast<const short*>(&model.buffers[bufferView_indices.buffer].data[0] + bufferView_indices.byteOffset);

            Geometry = (Vertex *)malloc(sizeof(Vertex)*accessor_indices.count);
            appManager.vertexBuffer.size = sizeof(Vertex) * accessor_indices.count;
            appManager.vertexCount = accessor_indices.count;

            const tinygltf::Accessor accessor_pos = model.accessors[primitive.attributes["POSITION"]];
            const tinygltf::Accessor accessor_nor = model.accessors[primitive.attributes["NORMAL"]];
            const tinygltf::Accessor accessor_tex = model.accessors[primitive.attributes["TEXCOORD_0"]];

            const tinygltf::BufferView bufferView_pos = model.bufferViews[accessor_pos.bufferView];
            const tinygltf::BufferView bufferView_nor = model.bufferViews[accessor_nor.bufferView];
            const tinygltf::BufferView bufferView_tex = model.bufferViews[accessor_tex.bufferView];

            const float* buffer_pos = reinterpret_cast<const float*>(&model.buffers[bufferView_pos.buffer].data[0] + bufferView_pos.byteOffset);
            const float* buffer_nor = reinterpret_cast<const float*>(&model.buffers[bufferView_nor.buffer].data[0] + bufferView_nor.byteOffset);
            const float* buffer_tex = reinterpret_cast<const float*>(&model.buffers[bufferView_tex.buffer].data[0] + bufferView_tex.byteOffset);

            for (unsigned int i=0; i<accessor_indices.count; i++)
            {
                unsigned int index = buffer_indices[i];

                Geometry[i].x = buffer_pos[index * 3 + 0]; // VEC3
                Geometry[i].y = buffer_pos[index * 3 + 1];
                Geometry[i].z = buffer_pos[index * 3 + 2];
                Geometry[i].w = 1.0f;
                Geometry[i].u = buffer_tex[index * 2 + 0]; // VEC2
                Geometry[i].v = buffer_tex[index * 2 + 1];

                // Log(false, ("INDEX " + std::to_string(i) + " "+ std::to_string(index) +
                //     " x: " + std::to_string(Geometry[i].x) +
                //     " y: " + std::to_string(Geometry[i].y) +
                //     " z: " + std::to_string(Geometry[i].z)
                //     ).c_str());
            }
        }

    }
    _createBuffer(appManager, appManager.vertexBuffer, reinterpret_cast<uint8_t*>(Geometry), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
#endif
}

#endif // VKGEOMETRY_H
