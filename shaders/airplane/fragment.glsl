#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

uniform float ambientStrength;
uniform vec3 ambientColour;

void main()
{
  vec4 result = ambientStrength * vec4(ambientColour, 1.0) * vec4(texture(texture_diffuse, TexCoords));
  color = result;
  
  //color = vec4(texture(texture_diffuse, TexCoords));
}