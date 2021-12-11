#version 430

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 tex_coord;
layout (binding = 0) uniform sampler2D s;

in vec2 tc;
out vec4 color;

void main(void)
{
	color = texture(s,tc);
}