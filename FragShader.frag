#version 320 es

//// Shader Resources ////
layout(set = 0, binding = 0) uniform mediump sampler2D triangleTexture;

//// Vertex Inputs ////
layout(location = 0) in mediump vec2 UV;
layout(location = 1) in highp float SHADE;

//// Fragment Outputs ////
layout(location = 0) out mediump vec4 fragColor;

void main()
{
	// Sample the checker board texture and write to the frame buffer attachment.
        fragColor =vec4(1.0,0.3,0.3,1.0)*SHADE;// texture(triangleTexture, UV) * SHADE; //
}
