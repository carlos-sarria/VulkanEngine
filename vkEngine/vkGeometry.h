#ifndef VKGEOMETRY_H
#define VKGEOMETRY_H

#include "tiny_gltf.h"

#include "vkStructs.h"
#include "vkMemory.h"


// Callback function required for tiny_gltf
static bool myTextureLoadingFunction(tinygltf::Image *image, const int image_idx, std::string * err,
        std::string * warn, int req_width, int req_height,
        const unsigned char * bytes, int size, void* user_data)
{
    // GLTF does not support DDS
    // The data received here is just the PNG exported by Blender
    // we will need to use the texture name and work out the DDS file
    Log(false, ("TEXTURE NAME: "+image->name).c_str());

    return true;
}

static void getTransform(Transform& transform, const tinygltf::Node node)
{
    int size = node.translation.size();
    transform.translation.x = (size>0) ? node.translation[0]: 0.0f;
    transform.translation.y = (size>0) ? node.translation[1]: 0.0f;
    transform.translation.z = (size>0) ? node.translation[2]: 0.0f;

    size = node.rotation.size();
    transform.rotation.x = (size>0) ? node.rotation[0]: 0.0f;
    transform.rotation.y = (size>0) ? node.rotation[1]: 0.0f;
    transform.rotation.z = (size>0) ? node.rotation[2]: 0.0f;
    transform.rotation.w = (size>0) ? node.rotation[3]: 1.0f;

    size = node.scale.size();
    transform.scale.x = (size>0) ? node.scale[0] : 1.0f;
    transform.scale.y = (size>0) ? node.scale[1] : 1.0f;
    transform.scale.z = (size>0) ? node.scale[2] : 1.0f;
}


/// <summary>Defines the vertices of a simple triangle which can be passed to the vertex shader to be rendered on screen</summary>
inline void _loadGLTF(AppManager& appManager, const char* fileName)
{
    // Load GLTF
    tinygltf::Model model;
    tinygltf::TinyGLTF gltf_ctx;
    std::string err;
    std::string warn;

    gltf_ctx.SetImageLoader(myTextureLoadingFunction, nullptr);

    gltf_ctx.SetStoreOriginalJSONForExtrasAndExtensions(false);

    bool ret = false;
    ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn, fileName);

    if(!ret){
        Log(true, ("GLTF - "+err).c_str());
        exit(1);
    }

    const tinygltf::Scene &scene = model.scenes[model.defaultScene];
    for(tinygltf::Node node : model.nodes)
    {
        Log(false, ("NODE NAME "+node.name).c_str());

        if (node.camera > -1)
        {
            appManager.cameras.push_back(Camera());
            int index = appManager.cameras.size() - 1;

            getTransform(appManager.cameras[index].transform, node);
            appManager.cameras[index].type = 0;
            appManager.cameras[index].aspectRatio = model.cameras[index].perspective.aspectRatio;
            appManager.cameras[index].yfov = model.cameras[index].perspective.yfov;
            appManager.cameras[index].zfar = model.cameras[index].perspective.zfar;
            appManager.cameras[index].znear = model.cameras[index].perspective.znear;

        }

        if (node.light > -1)
        {
            appManager.lights.push_back(Light());
            int index = appManager.lights.size() - 1;

            getTransform(appManager.lights[index].transform, node);
            appManager.lights[index].type = 0;
        }

        if (node.mesh > -1)
        {

            tinygltf::Mesh mesh = model.meshes[node.mesh];
            Log(false, ("MESH NAME "+mesh.name).c_str());

            Vertex *Geometry;
            uint16_t* bufferIndices;
            unsigned int numIndices = 0;
            unsigned int numVertices = 0;


            for (tinygltf::Primitive primitive : mesh.primitives)
            {
                const tinygltf::Accessor accessor_indices = model.accessors[primitive.indices];
                const tinygltf::BufferView bufferView_indices = model.bufferViews[accessor_indices.bufferView];
                bufferIndices = reinterpret_cast<uint16_t*>(&model.buffers[bufferView_indices.buffer].data[0] + bufferView_indices.byteOffset);

                Geometry = (Vertex *)malloc(sizeof(Vertex)*accessor_indices.count);
                numIndices = accessor_indices.count;

                const tinygltf::Accessor accessor_pos = model.accessors[primitive.attributes["POSITION"]];
                const tinygltf::Accessor accessor_nor = model.accessors[primitive.attributes["NORMAL"]];
                const tinygltf::Accessor accessor_tex = model.accessors[primitive.attributes["TEXCOORD_0"]];

                const tinygltf::BufferView bufferView_pos = model.bufferViews[accessor_pos.bufferView];
                const tinygltf::BufferView bufferView_nor = model.bufferViews[accessor_nor.bufferView];
                const tinygltf::BufferView bufferView_tex = model.bufferViews[accessor_tex.bufferView];

                const float* buffer_pos = reinterpret_cast<const float*>(&model.buffers[bufferView_pos.buffer].data[0] + bufferView_pos.byteOffset);
                const float* buffer_nor = reinterpret_cast<const float*>(&model.buffers[bufferView_nor.buffer].data[0] + bufferView_nor.byteOffset);
                const float* buffer_tex = reinterpret_cast<const float*>(&model.buffers[bufferView_tex.buffer].data[0] + bufferView_tex.byteOffset);

                numVertices = accessor_pos.count;

                //for (unsigned int i=0; i<accessor_indices.count; i++)
                for (unsigned int i=0; i<numVertices; i++)
                {
                    Geometry[i].pos.x = buffer_pos[i * 3 + 0]; // VEC3
                    Geometry[i].pos.y = buffer_pos[i * 3 + 1];
                    Geometry[i].pos.z = buffer_pos[i * 3 + 2];
                    Geometry[i].pos.w = 1.0f;
                    Geometry[i].nor.x = buffer_nor[i * 3 + 0]; // VEC3
                    Geometry[i].nor.y = buffer_nor[i * 3 + 1];
                    Geometry[i].nor.z = buffer_nor[i * 3 + 2];
                    Geometry[i].tex.u = buffer_tex[i * 2 + 0]; // VEC2
                    Geometry[i].tex.v = buffer_tex[i * 2 + 1];

                    // Log(false, ("INDEX " + std::to_string(i) + " "+ std::to_string(index) +
                    //     " x: " + std::to_string(Geometry[i].x) +
                    //     " y: " + std::to_string(Geometry[i].y) +
                    //     " z: " + std::to_string(Geometry[i].z)
                    //     ).c_str());
                }
            }

            appManager.meshes.push_back(Mesh());
            int index = appManager.meshes.size() - 1;

            getTransform(appManager.meshes[index].transform, node);

            appManager.meshes[index].indexBuffer.size = sizeof(uint16_t) * numIndices;
            _createBuffer(appManager, appManager.meshes[index].indexBuffer, reinterpret_cast<uint8_t*>(bufferIndices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

            appManager.meshes[index].vertexBuffer.size = sizeof(Vertex) * numVertices;
            _createBuffer(appManager, appManager.meshes[index].vertexBuffer, reinterpret_cast<uint8_t*>(Geometry), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

            appManager.meshes[index].vertexCount = numIndices;

            free(Geometry);
        }
    }

}

#endif // VKGEOMETRY_H
