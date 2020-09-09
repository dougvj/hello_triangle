#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char *glErrorString(GLenum err) {
  switch (err) {
  case GL_NO_ERROR:
    return "GL_NO_ERROR: No error has been recorded";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM: An unacceptable value is specified for an "
           "enumerated argument";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION: The specified operation is not allowed in "
           "the current state";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION: The command is trying to render "
           "to or read from the framebuffer while the currently bound "
           "framebuffer is not framebuffer complete";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the "
           "command";
  }
  return "Unspecified Error";
}

// Simple check error call
int check_gl_error(const char *call) {
  int err = glGetError();
  if (err != 0) {
    int prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    fprintf(stderr, "%s\nGlError: '%s' CurrentProgram: %i\n\n", call,
            glErrorString(err), prog);
    exit(1);
  }
  return err;
}

// We can wrap the check error in a define to disable the checking
#define GL_DEBUG(m)                                                            \
  m;                                                                           \
  check_gl_error(#m);

// TODO Figure out how to get this compound statement to work so it's one r
// value statment. Current breaks on void return values. #define GL_DEBUG(m)
//({__auto_type ret = m; check_gl_error(#m); ret;})

// Compiles a shader
static GLint compile_shader(GLenum shader_type, const char *shader_file) {
  // Open our shader file
  FILE *f;
  f = fopen(shader_file, "r");
  if (!f) {
    fprintf(stderr, "Unable to open shader file %s. Aborting.\n", shader_file);
    return -1;
  }
  // Get the shader size
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fprintf(stderr, "Compiling %s. Shader size: %li bytes\n", shader_file, fsize);
  char shader_source[fsize + 1];
  fseek(f, 0, SEEK_SET);
  fread(shader_source, fsize, 1, f);
  shader_source[fsize] = '\0';
  fclose(f);
  // Compile the shader
  GLuint shader = GL_DEBUG(glCreateShader(shader_type));
  const char *ss = shader_source;
  glShaderSource(shader, 1, &ss, NULL);
  glCompileShader(shader);
  // Check status
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    // Output error if there was a problem
    GLint info_log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
    GLchar info_log[info_log_length];
    glGetShaderInfoLog(shader, info_log_length, NULL, info_log);
    fprintf(stderr, "Compile failure in shader:\n%s\n", info_log);
    return -1;
  }
  return shader;
}

// Verticies for our output triangle
static const GLfloat triangle[][2] = {
    {-1.0f,  0.0f},
    { 1.0f, -1.0f},
    { 1.0f,  1.0f},
};

// Color for our output triangle
static const GLfloat colors[][4] = {
    {1.0, 0.0, 0.0, 1.0},
    {0.0, 1.0, 0.0, 1.0},
    {0.0, 0.0, 1.0, 1.0}
};

int main(int argc, char **argv) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "Unable to initialize SDL\n");
    fprintf(stderr, "%s\n", SDL_GetError());
    return 1;
  }
  // Initialize OpenGL context
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  // Enabled multisampling (a type of antialiasing0
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
  // Create the SDL window
  SDL_Window *window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 1024, 768,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (window == NULL) {
    fprintf(stderr, "Unable to create SDL Window\n");
    fprintf(stderr, "%s\n", SDL_GetError());
    exit(1);
  }
  // Create the SDL window
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == NULL) {
    fprintf(stderr, "Unable to create OpenGL Context\n");
    fprintf(stderr, "%s\n", SDL_GetError());
    exit(1);
  }
  // Output OpenGL Version
  const unsigned char *version = GL_DEBUG(glGetString(GL_VERSION));
  if (version == NULL) {
    fprintf(stderr, "Unable to determine OpenGL version\n");
    exit(1);
  }
  fprintf(stderr, "OpenGl Version: %s\n", version);
  // Set current window
  SDL_GL_MakeCurrent(window, gl_context);
  // Initialize glew (which loads the OpenGL Function Pointers)
  GLenum glew_status = glewInit();
  if (glew_status != 0) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    exit(1);
  }

  // enable depth testing. This doesn't matter for a single triangle but
  // useful if you have a 3d scene or multiple triangles that overlap
  GL_DEBUG(glEnable(GL_DEPTH_TEST));
  GL_DEBUG(glDepthFunc(GL_LESS));
  GL_DEBUG(glDepthRange(0, 1000));
  // Enable alpha blending
  GL_DEBUG(glEnable(GL_BLEND));
  GL_DEBUG(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GL_DEBUG(glEnable(GL_MULTISAMPLE));
  // We can cull triangles that are wound away from us. Also important for 3d
  // scenes but not this so mcuh. Note that since we have it enabled here
  // the order we provide the verticies in the triangle array matters
  GL_DEBUG(glEnable(GL_CULL_FACE));

  // Compile our shaders
  GLint vertex_shader;
  GLint fragment_shader;
  vertex_shader = compile_shader(GL_VERTEX_SHADER, "vertex.glsl");
  fragment_shader = compile_shader(GL_FRAGMENT_SHADER, "fragment.glsl");
  // If we have a problem quit
  if (vertex_shader < 0 || fragment_shader < 0)
    return -1;

  // Link the shaders together into a single 'shader program'
  GLuint program;
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  // error checking
  GLint status;
  // Get link log if there is a problem
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint info_log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

    GLchar info_log[info_log_length];
    glGetProgramInfoLog(program, info_log_length, NULL, info_log);
    fprintf(stderr, "Shader linker failure: %s\n", info_log);
    return -1;
  }
  // Once the shaders are linked into a programthey don't need to be kept around
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  // Handles for various buffers
  // vao is a buffer of buffers, (so will point our verticies and colors)
  // vbo_verticies is for the triangle points and
  // vbo_colors is for the triangle colors
  GLuint vao, vbo_verticies, vbo_colors;

  // Generate a single Vertex Array and bind it  (meaning subsequnece calls will
  // use it)
  GL_DEBUG(glGenVertexArrays(1, &vao));
  GL_DEBUG(glBindVertexArray(vao));
  // Generate the buffer object for the verticies
  GL_DEBUG(glGenBuffers(1, &vbo_verticies));
  // bind it for the next few calls
  GL_DEBUG(glBindBuffer(GL_ARRAY_BUFFER, vbo_verticies));
  // Upload our triangle data to the vbo
  GL_DEBUG(glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle,
                        GL_STATIC_DRAW));
  // We get the location of the 'in_position' named in the vertex shader
  GLint in_position_loc = GL_DEBUG(glGetAttribLocation(program, "in_position"));
  // Set the location in the vao to this buffer and tell it how to access the
  // data. We have 2 points per vertex hence 2, and sizeof(float) * 2 and the
  // GL_FLOAT
  GL_DEBUG(glVertexAttribPointer(in_position_loc, 2, GL_FLOAT, GL_FALSE,
                                 sizeof(float) * 2, 0));
  // Enable this buffer
  GL_DEBUG(glEnableVertexAttribArray(in_position_loc));
  // Now geneate the vbo for colors
  GL_DEBUG(glGenBuffers(1, &vbo_colors));
  // Bind it for the next few calls
  GL_DEBUG(glBindBuffer(GL_ARRAY_BUFFER, vbo_colors));
  // Upload the color data in the same way we did triangles
  GL_DEBUG(
      glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));
  // We get the location of the 'in_color' named in the vertex shader
  GLint in_color_loc = GL_DEBUG(glGetAttribLocation(program, "in_color"));
  // This time we have RGBA values so set up 4 floats per vertex
  GL_DEBUG(glVertexAttribPointer(in_color_loc, 4, GL_FLOAT, GL_FALSE,
                                 sizeof(float) * 4, 0));
  // Enable the vbo
  GL_DEBUG(glEnableVertexAttribArray(in_color_loc));
  // Now we set to use the shader program we previously compiled
  glUseProgram(program);

  for (;;) {
    // First check to see if we should quit (from SDL)
    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT)
      break;
    // Set our black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Now we draw the triangles. There are 3 points to draw
    GL_DEBUG(glDrawArrays(GL_TRIANGLES, 0, 3));
    // Swap the output
    SDL_GL_SwapWindow(window);
  }
  return 0;
}
