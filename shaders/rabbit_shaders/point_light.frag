#version 450

layout(location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  vec4 ambientLightColor; // w = intensity
  vec4 lightColor; // w = intensity
  vec3 lightPosition;
} ubo;

void main() {
  // FIXME: remove this after point light render system is made
  // squared distance from center
  float sqDstFromCenter = sqrt(dot(fragOffset, fragOffset));
  if (sqDstFromCenter >= 1.0) {
    discard;
  }

  outColor = vec4(ubo.lightColor.xyz, 1.0);
}
