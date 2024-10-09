#version 320 es

//// Vertex Shader inputs
layout(location = 0) in highp vec4 vertex;
layout(location = 1) in highp vec3 normal;
layout(location = 2) in mediump vec2 uv;

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

	// Calculate the ndc position for the current vertex using the model view projection matrix.
	gl_Position = modelViewProjectionMatrix * vertex;
        light = normalize(vec4(lightDirection,0.0)-vertex).xyz;
        SHADE_OUT = dot(normal,light)*0.5+0.5;
	UV_OUT = uv;
}
