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

vec4 GoochShading() {

	float diffuse = (max(0, dot(normal, light.dir)));

	vec3 col1 = mix(vec3(0.0, 0.0, 0.5), vec3(0.5, 0.5, 0.0) , diffuse);

	vec3 col2 = mix(vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), diffuse);

	return vec4(col1 + col2, 1.0) ;//vec4(1.0, 0.0, 0.0, 1.0);

}

void main() {
	
	outColor = GoochShading();

} 