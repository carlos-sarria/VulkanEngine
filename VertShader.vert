#version 320 es

//// Vertex Shader inputs
layout(location = 0) in highp vec3 vertex;
layout(location = 1) in highp vec3 normal;
layout(location = 2) in highp vec2 uv;

//// Shader Resources ////
layout(std140, set = 1, binding = 0) uniform UniformBufferObject
{
	mat4 modelViewProjectionMatrix;
        vec3 lightDirection;
};

//// Per Vertex Outputs ////
layout(location = 0) out mediump vec2 UV_OUT;
layout(location = 1) out highp float SHADE_OUT;

void main()
{
    vec3 light;
    vec4 pos = vec4(vertex, 1.0);

	// Calculate the ndc position for the current vertex using the model view projection matrix.
        gl_Position = modelViewProjectionMatrix * pos;
        light = normalize(lightDirection-vertex);
        SHADE_OUT = dot(normal,light)*0.5+0.5;
	UV_OUT = uv;
}
