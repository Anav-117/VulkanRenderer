#version 450

layout(location = 0) in vec3 pos_attrib;
layout(location = 1) in vec3 normal_attrib;
layout(location = 2) in vec2 texCoord_attrib;

layout(set = 0, binding = 0) uniform Transform {
	mat4 model;
	mat4 view;
	mat4 projection;
	float time;
} transform;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 texCoord;
layout(location = 3) flat out vec3 flat_normal;
layout(location = 4) out float time;

void main() {

	vec4 p = transform.projection * transform.view * transform.model * vec4(pos_attrib, 1.0);
	gl_Position = p;
	pos = p.xyz;

	normal = (transform.model * vec4(normal_attrib, 1.0)).xyz;
	flat_normal = (transform.model * vec4(normal_attrib, 1.0)).xyz;

	time = transform.time;

	texCoord = texCoord_attrib;

}