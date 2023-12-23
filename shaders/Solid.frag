#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) flat in vec3 flat_normal;
layout(location = 4) in float time;

layout(set = 0, binding = 1) uniform sampler2D albedo;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform Light {
	vec3 dir;
	vec3 viewPos;
} light;

void main() {
	
	outColor = vec4(0.0, 0.0, 0.0, 1.0);

} 