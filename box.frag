#version 410 
in vec3 box_pos_eye;
in vec3 box_normal_eye;
in vec2 box_text_eye;
uniform mat4 view_mat;
uniform sampler2D box_texture;
// fixed point light properties
vec3 light_position_world = vec3 (2.0, 2.0, 4.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour
// surface reflectance
vec3 Ks = vec3 (1.0, 1.0,1.0); // fully reflect specular light	    
vec3 Kd = vec3 (0.5, 0.5,0.0); // orange diffuse surface reflectance 
vec3 Ka = vec3 (1.0, 1.0,1.0); // fully reflect ambient light	    
float specular_exponent =100.0; // specular 'power'		    

out vec4 fragment_colour; // final colour of surface

void main () {
  vec4 texel = texture (box_texture, box_text_eye);
  // ambient intensity
  vec3 Ia = La * vec3(texel);
  // diffuse intensity
  
  // raise light position to eye space
  vec3 light_position_eye = vec3 (view_mat * vec4 (light_position_world, 1.0));
  vec3 distance_to_light_eye = light_position_eye - box_pos_eye;
  vec3 direction_to_light_eye = normalize (distance_to_light_eye);
  float dot_prod = dot (direction_to_light_eye, box_normal_eye);
  dot_prod = max (dot_prod, 0.0);
  vec3 Id = Ld * vec3(texel)* dot_prod; // final diffuse intensity
  // specular intensity
 
  vec3 reflection_eye = reflect (-direction_to_light_eye, box_normal_eye);
  vec3 surface_to_viewer_eye = normalize (-box_pos_eye);
  float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
  dot_prod_specular = max (dot_prod_specular, 0.0);
  float specular_factor = pow (dot_prod_specular, specular_exponent);
  vec3 Is = Ls * vec3(texel) * specular_factor; // final specular intensity

  // final colour
  fragment_colour = vec4 (Ia+Id+Is, 1.0);
}
