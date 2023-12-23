#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(set = 0, binding = 1) uniform sampler2D albedo;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform Light {
	vec3 dir;
	int shaderType;
} light;

vec3 viewPos = vec3(0.0f, 0.0f, 2.5f);

vec4 PhongShading() {

	float diffuse = max(0, dot(normal, light.dir));

	vec3 viewDir = normalize(viewPos - pos);
	vec3 reflected = reflect(-1.0f * light.dir, normal);

	float specdot = pow(max(0, dot(normalize(reflected), viewDir)), 50);

	return (specdot+diffuse) * texture(albedo, texCoord);

}

vec4 CelShading() {

	float diffuse = max(0, dot(normal, light.dir));

	if (diffuse > 0.2) {
		diffuse = 0.5;
	}
	else {
		diffuse = 0.05;
	}

	vec3 viewDir = normalize(viewPos - pos);
	vec3 reflected = reflect(-1.0f * light.dir, normal);

	float specdot = pow(max(0, dot(normalize(reflected), viewDir)), 50);

	if (specdot > 0.5) {
		specdot = 10.0;
	}
	else {
		specdot = 0.0;
	}

	return (specdot+diffuse) * texture(albedo, texCoord);	

}

vec4 GoochShading() {

	float diffuse = max(0, dot(normal, light.dir));

	return vec4(mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 0.0), diffuse), 1.0) ;//vec4(1.0, 0.0, 0.0, 1.0);

}

void main() {
	
	//if (light.shaderType == 0) {
		outColor = PhongShading();	
	//}
	//else if (light.shaderType == 1) {
	//	outColor = CelShading();
	//}
	//else if (light.shaderType == 2) {
	//	outColor = GoochShading();
	//}
	//else {
	//	outColor =  vec4(1.0, 0.0, 1.0, 1.0);
	//}

} 