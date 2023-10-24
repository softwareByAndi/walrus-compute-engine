#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  vec4 ambientLightColor; // w = intensity
  vec4 lightColor; // w = intensity
  vec3 lightPosition;
} ubo;

/** NOTE:
 - push_constant structures MUST match whatever push constant struct is used
 - size is only guaranteed to support at least 128 bytes = a max of 2 4x4 matrices
 */
layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;


void main() {

  vec3 directionToLight = ubo.lightPosition - fragPosWorld;
  float attenuation = 1 / dot(directionToLight, directionToLight); // 1 / distance squared

  vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
  vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

  outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0); // RGBa
}





/** Saved for later:
 - outColor = vec4(push.color, 1.0); // RGBa
*/
