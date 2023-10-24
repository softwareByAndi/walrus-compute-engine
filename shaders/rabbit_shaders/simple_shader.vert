#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

/** NOTE:
 - locations between in and out operaters are not related.
 - layouts MUST match the structure of the data they're bound to
 */
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  vec4 ambientLightColor; // w = intensity
  vec4 lightColor; // w = intensity
  vec3 lightPosition;
} ubo;

/** NOTE: size is only guaranteed to support at least 128 bytes = a max of 2 4x4 matrices */
layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;


/** NOTE:
 - this function is run once for every vertex.
 - gl_Position = vec4(x, y, z, norm) : -1 = top left to 1 = bottom right
 - gl_VertexIndex contains the current index for each time the function is run
 */
void main() {
  vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
  gl_Position = ubo.projectionMatrix * (ubo.viewMatrix * positionWorld);

  fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
  fragPosWorld = positionWorld.xyz;
  fragColor = color;
}



/** Save for later:
 - gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0);
 - gl_Position = push.transform * vec4(position, 1.0);
 - fragColor = color; // pass the color straight to the fragment stader

 - this calculation below is expensive and should either be avoided or pre-computed
 mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
*/
