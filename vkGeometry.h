#ifndef VKGEOMETRY_H
#define VKGEOMETRY_H

#include "vkStructs.h"
#include "vkMemory.h"

/// <summary>Defines the vertices of a simple triangle which can be passed to the vertex shader to be rendered on screen</summary>
inline void _initVertexBuffers(AppManager& appManager)
{
    // This function defines the vertices of a simple triangle and creates a vertex buffer to hold this data.

    // Calculate the size of the vertex buffer to be passed to the vertex shader.
    appManager.vertexBuffer.size = sizeof(Vertex) * 3;

    // Set the values for the triangle's vertices.
    Vertex triangle[3];
    triangle[0] = { -0.5f, -0.288f, 0.0f, 1.0f, 0.0f, 0.0f };
    triangle[1] = { .5f, -.288f, 0.0f, 1.0f, 1.0f, 0.0f };
    triangle[2] = { 0.0f, .577f, 0.0f, 1.0f, 0.5f, 1.0f };

    // Create the buffer that will hold the data and be passed to the shaders.
    _createBuffer(appManager, appManager.vertexBuffer, reinterpret_cast<uint8_t*>(triangle), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

#endif // VKGEOMETRY_H
