#version 450

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout(location = 0) in VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 texCoord;
    vec3 flat_normal;
    float time;
} gs_in[];

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 texCoord;
layout(location = 3) flat out vec3 flat_normal;

void main(void)
{	

    gl_Position = gl_in[0].gl_Position;
    pos = gs_in[0].pos;
    normal = gs_in[0].normal;
    texCoord = gs_in[0].texCoord;
    flat_normal = gs_in[0].flat_normal;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    pos = gs_in[1].pos;
    normal = gs_in[1].normal;
    texCoord = gs_in[1].texCoord;
    flat_normal = gs_in[1].flat_normal;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    pos = gs_in[2].pos;
    normal = gs_in[2].normal;
    texCoord = gs_in[2].texCoord;
    flat_normal = gs_in[2].flat_normal;
    EmitVertex();

    EndPrimitive();

}  