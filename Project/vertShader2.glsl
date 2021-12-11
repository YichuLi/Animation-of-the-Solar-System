#version 430

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 tex_coord;
layout (binding = 0) uniform sampler2D s;
out vec2 tc;

void main(void)
{
	gl_Position = proj_matrix * mv_matrix * vec4(vertPos,1.0);
	tc = tex_coord;
}