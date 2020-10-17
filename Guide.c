#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>

#define GL_LOG_FILE "gl.log"
#define PI 3.1416f
#define CIRC_RES 30

bool gl_log (const char* message, ...);
bool restart_gl_log ();
bool gl_log_err (const char* message, ...);
void glfw_error_callback (int error, const char* description);
void log_gl_params ();
void _print_shader_info_log (GLuint shader_index);
void _print_programme_info_log (GLuint programme);
void print_all (GLuint programme);
const char* GL_type_to_string (GLenum type);  
bool is_valid (GLuint programme);

// reported window size may be good to know for a few things
int g_win_width = 640;
int g_win_height = 480;
// keep track of framebuffer size for things like the viewport and the mouse cursor
int g_fb_width = 640;
int g_fb_height = 480;


void glfw_window_size_callback (GLFWwindow* window, int width, int height) {
  g_win_width = width;
  g_win_height = height;
  gl_log("window_size changed to %dx%d\n" , width,height);
		       
}
void glfw_framebuffer_resize_callback (GLFWwindow* window, int width, int height) {
  g_fb_width = width;
  g_fb_height = height;
  gl_log("framebuffer_size changed to %dx%d\n", width , height);
  /* later update any perspective matrices used here */
}

double previous_seconds;
int frame_count;
/* we will use this function to update the window title with a frame rate */
void _update_fps_counter (GLFWwindow* window);

int main () {
  if (!restart_gl_log ()) { /* quit? */ }
  // start GL context and O/S window using the GLFW helper library
  gl_log ("starting GLFW\n%s\n", glfwGetVersionString ());
  // register the error call-back function that we wrote, above
  glfwSetErrorCallback (glfw_error_callback);
  if (!glfwInit ()) {
    fprintf (stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }
  // uncomment these lines if on Apple OS X
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // gl anti-aliasing filter samples 16 is for screen shot , 4 is normally  good. 
    glfwWindowHint (GLFW_SAMPLES, 16);


  GLFWmonitor* mon = glfwGetPrimaryMonitor ();
  const GLFWvidmode* vmode = glfwGetVideoMode (mon);
  //GLFWwindow* window = glfwCreateWindow (vmode->width, vmode->height, "Extended GLInit", mon, NULL);

  GLFWwindow* window = glfwCreateWindow (640, 480, "Hello Triangle", NULL, NULL);
  if (!window) {
    fprintf (stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwSetWindowSizeCallback (window, glfw_window_size_callback);
  glfwSetFramebufferSizeCallback (window, glfw_framebuffer_resize_callback);
  
  
  glfwMakeContextCurrent (window);
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit ();

  log_gl_params();
  // get version info
  const GLubyte* renderer = glGetString (GL_RENDERER);
  const GLubyte* version = glGetString (GL_VERSION);
  printf ("Renderer: %s\n", renderer);
  printf ("OpenGL version supported %s\n", version);
  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable (GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
  /* OTHER STUFF GOES HERE NEXT */
  GLfloat points[]={
		    -0.5f  ,0.5f  , 0.0f,
		    -0.5f ,-0.5f , 0.0f,
		    0.5f  ,-0.5f , 0.0f,
		    0.5f  ,-0.5f , 0.0f,
		    0.5f  ,0.5f  , 0.0f,
		    -0.5f , 0.5f  ,0.0f
		    
  };

  GLfloat points_circ[CIRC_RES*3];
  for(int i=0;i<CIRC_RES;i++){
    points_circ[i*3] = 0.5*cos((2.0 * PI * i )/ CIRC_RES);
    points_circ[i*3+1] = 0.5*sin((2.0 * PI * i )/ CIRC_RES);
    points_circ[i*3+2] = 0.0f;
  }

  

  
  /* GLuint vbo = 0; */
  /* glGenBuffers (1,&vbo); */
  /* glBindBuffer(GL_ARRAY_BUFFER,vbo); */
  /* glBufferData(GL_ARRAY_BUFFER, sizeof(points),points,GL_STATIC_DRAW); */

  GLuint vbo_circ = 0;
  glGenBuffers (1,&vbo_circ);
  glBindBuffer(GL_ARRAY_BUFFER,vbo_circ);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points_circ),points_circ,GL_STATIC_DRAW);
  
  /* GLuint vao = 0; */
  /* glGenVertexArrays (1, &vao); */
  /* glBindVertexArray (vao); */
  /* glEnableVertexAttribArray (0); */
  /* glBindBuffer (GL_ARRAY_BUFFER, vbo); */
  /* glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL); */

  GLuint vao_circ = 0;
  glGenVertexArrays (1, &vao_circ);
  glBindVertexArray (vao_circ);
  glEnableVertexAttribArray (0);
  glBindBuffer (GL_ARRAY_BUFFER, vbo_circ);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  const char* vertex_shader =
    "#version 410\n"
    "in vec3 vp;"
    "void main () {"
    " gl_Position = vec4 (vp, 1.0);"
    "}";

  const char* fragment_shader =
    "#version 410\n"
    "out vec4 frag_colour;"
    "void main () {"
    " frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
    "}";

  GLuint vs = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vs, 1, &vertex_shader, NULL);
  glCompileShader (vs);
  
  // check for compile errors
  int params = -1;
  glGetShaderiv (vs, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf (stderr, "ERROR: GL shader index %i did not compile\n", vs);
    _print_shader_info_log (vs);
    return false; // or exit or something
  }
  
  GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fs, 1, &fragment_shader, NULL);
  glCompileShader (fs);
  params = -1;
  glGetShaderiv (fs, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf (stderr, "ERROR: GL shader index %i did not compile\n", fs);
    _print_shader_info_log (fs);
    return false; // or exit or something
  }

  GLuint shader_programme = glCreateProgram ();
  glAttachShader (shader_programme, fs);
  glAttachShader (shader_programme, vs);
  glLinkProgram (shader_programme);
  // check if link was successful
  params = -1;
  glGetProgramiv (shader_programme, GL_LINK_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf (stderr, "ERROR: could not link shader programme GL index %u\n",shader_programme);
    _print_programme_info_log (shader_programme);
    return false;
  }
  //validate shaders
  is_valid(shader_programme);  


 
  glClearColor(0.6f,0.6f,0.8f,1.0f);
  while (!glfwWindowShouldClose (window)) {
    _update_fps_counter(window);
    // wipe the drawing surface clear
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport (0, 0, g_fb_width, g_fb_height);
    glUseProgram (shader_programme);
    glBindVertexArray (vao_circ);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays (GL_LINE_STRIP, 0,CIRC_RES);
    // update other events like input handling
    glfwPollEvents ();
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers (window);

    if (GLFW_PRESS == glfwGetKey (window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose (window, 1);
    }
  }
  

  // close GL context and any other GLFW resources
  
  glfwTerminate();
  return 0;
}

bool restart_gl_log () {
  FILE* file = fopen (GL_LOG_FILE, "w");
  if (!file) {
    fprintf (
	     stderr,
	     "ERROR: could not open GL_LOG_FILE log file %s for writing\n",
	     GL_LOG_FILE
	     );
    return false;
  }
  time_t now = time (NULL);
  char* date = ctime (&now);
  fprintf (file, "GL_LOG_FILE log. local time %s\n", date);
  fclose (file);
  return true;
}


bool gl_log (const char* message, ...) {
  va_list argptr;
  FILE* file = fopen (GL_LOG_FILE, "a");if (!file) {
    fprintf (
	     stderr,
	     "ERROR: could not open GL_LOG_FILE %s file for appending\n",
	     GL_LOG_FILE
	     );
    return false;
  }
  va_start (argptr, message);
  vfprintf (file, message, argptr);
  va_end (argptr);
  fclose (file);
  return true;
}


bool gl_log_err (const char* message, ...) {
  va_list argptr;
  FILE* file = fopen (GL_LOG_FILE, "a");
  if (!file) {
    fprintf (
	     stderr,
	     "ERROR: could not open GL_LOG_FILE %s file for appending\n",
	     GL_LOG_FILE
	     );
    return false;
  }
  va_start (argptr, message);
  vfprintf (file, message, argptr);
  va_end (argptr);
  va_start (argptr, message);
  vfprintf (stderr, message, argptr);
  va_end (argptr);
  fclose (file);
  return true;
}
void glfw_error_callback (int error, const char* description) {
  gl_log_err ("GLFW ERROR: code %i msg: %s\n", error, description);
}

void log_gl_params () {
  GLenum params[] = {
		     GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		     GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		     GL_MAX_DRAW_BUFFERS,
		     GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		     GL_MAX_TEXTURE_IMAGE_UNITS,
		     GL_MAX_TEXTURE_SIZE,
		     GL_MAX_VARYING_FLOATS,
		     GL_MAX_VERTEX_ATTRIBS,
		     GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		     GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		     GL_MAX_VIEWPORT_DIMS,
		     GL_STEREO,
  };
  const char* names[] = {
			 "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
			 "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
			 "GL_MAX_DRAW_BUFFERS",
			 "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
			 "GL_MAX_TEXTURE_IMAGE_UNITS",
			 "GL_MAX_TEXTURE_SIZE",
			 "GL_MAX_VARYING_FLOATS",
			 "GL_MAX_VERTEX_ATTRIBS",
			 "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
			 "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
			 "GL_MAX_VIEWPORT_DIMS",
			 "GL_STEREO",
  };
  gl_log ("GL Context Params:\n");
  // integers - only works if the order is 0-10 integer return types
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv (params[i], &v);
    gl_log ("%s %i\n", names[i], v);
  }
  // others
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv (params[10], v);
  gl_log ("%s %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv (params[11], &s);
  gl_log ("%s %u\n", names[11], (unsigned int)s);
  gl_log ("-----------------------------\n");
}


/* we will use this function to update the window title with a frame rate */
void _update_fps_counter (GLFWwindow* window) {
  double current_seconds;
  double elapsed_seconds;
  current_seconds = glfwGetTime ();
  elapsed_seconds = current_seconds - previous_seconds;
  /* limit text updates to 4 per second */
  if (elapsed_seconds > 0.25) {
    previous_seconds = current_seconds;
    char tmp[128];
    double fps = (double)frame_count / elapsed_seconds;
    sprintf (tmp, "opengl @ fps: %.2f", fps);glfwSetWindowTitle (window, tmp);
    frame_count = 0;
  }
  frame_count++;
}
void _print_shader_info_log (GLuint shader_index) {
  int max_length = 2048;
  int actual_length = 0;
  char log[2048];
  glGetShaderInfoLog (shader_index, max_length, &actual_length, log);
  printf ("shader info log for GL index %u:\n%s\n", shader_index, log);
}
void _print_programme_info_log (GLuint programme) {
  int max_length = 2048;
  int actual_length = 0;
  char log[2048];
  glGetProgramInfoLog (programme, max_length, &actual_length, log);
  printf ("program info log for GL index %u:\n%s", programme, log);
}
void print_all (GLuint programme) {
  printf ("--------------------\nshader programme %i info:\n", programme);
  int params = -1;
  glGetProgramiv (programme, GL_LINK_STATUS, &params);
  printf ("GL_LINK_STATUS = %i\n", params);
  glGetProgramiv (programme, GL_ATTACHED_SHADERS, &params);
  printf ("GL_ATTACHED_SHADERS = %i\n", params);
  glGetProgramiv (programme, GL_ACTIVE_ATTRIBUTES, &params);
  printf ("GL_ACTIVE_ATTRIBUTES = %i\n", params);
  for (GLuint i = 0; i < (GLuint)params; i++) {
    char name[64];
    int max_length = 64;
    int actual_length = 0;
    int size = 0;
    GLenum type;
    glGetActiveAttrib (programme, i, max_length, &actual_length, &size, &type,name);
    if (size > 1) {
      for (int j = 0; j < size; j++) {
	char long_name[64];
	sprintf (long_name, "%s[%i]", name, j);
	int location = glGetAttribLocation (programme, long_name);
	printf (" %i) type:%s name:%s location:%i\n", i, GL_type_to_string (type),long_name, location);
      }
    } else {
      int location = glGetAttribLocation (programme, name);
      printf (" %i) type:%s name:%s location:%i\n", i, GL_type_to_string (type),name, location);
    }
  }
  glGetProgramiv (programme, GL_ACTIVE_UNIFORMS, &params);
  printf ("GL_ACTIVE_UNIFORMS = %i\n", params);
  for (GLuint i = 0; i < (GLuint)params; i++) {
    char name[64];
    int max_length = 64;
    int actual_length = 0;
    int size = 0;
    GLenum type;
    glGetActiveUniform (programme, i, max_length, &actual_length, &size, &type,name);
    if (size > 1) {
      for (int j = 0; j < size; j++) {
	char long_name[64];
	sprintf (long_name, "%s[%i]", name, j);
	int location = glGetUniformLocation (programme, long_name);
	printf (" %i) type:%s name:%s location:%i\n", i, GL_type_to_string (type),long_name, location);
      }
    } else {
      int location = glGetUniformLocation (programme, name);
      printf (" %i) type:%s name:%s location:%i\n", i, GL_type_to_string (type),name, location);
    }
  }
  _print_programme_info_log (programme);
}
const char* GL_type_to_string (GLenum type) {
  switch (type) {case GL_BOOL: return "bool";
  case GL_INT: return "int";
  case GL_FLOAT: return "float";
  case GL_FLOAT_VEC2: return "vec2";
  case GL_FLOAT_VEC3: return "vec3";
  case GL_FLOAT_VEC4: return "vec4";
  case GL_FLOAT_MAT2: return "mat2";
  case GL_FLOAT_MAT3: return "mat3";
  case GL_FLOAT_MAT4: return "mat4";
  case GL_SAMPLER_2D: return "sampler2D";
  case GL_SAMPLER_3D: return "sampler3D";
  case GL_SAMPLER_CUBE: return "samplerCube";
  case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
  default: break;
  }
  return "other";
}

bool is_valid (GLuint programme) {
    glValidateProgram (programme);
    int params = -1;
    glGetProgramiv (programme, GL_VALIDATE_STATUS, &params);
    printf ("program %i GL_VALIDATE_STATUS = %i\n", programme, params);
    if (GL_TRUE != params) {
      _print_programme_info_log (programme);
      return false;
    }
    return true;
}
