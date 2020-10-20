#version 410
layout(location = 0) in  vec3 vertex_position;
layout(location = 1) in  vec3 vertex_colour;
out vec3 colour;
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 project_mat;
void main () {
  gl_Position = project_mat * view_mat * model_mat * vec4(vertex_position, 1.0) ;
  colour = vertex_colour;
}
