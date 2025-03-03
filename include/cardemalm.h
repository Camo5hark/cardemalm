/*
 * Cardemalm API symbols for implementation and then use in Cardemalm cores
 */

#ifndef _CARDEMALM_CORE_H_
#define _CARDEMALM_CORE_H_

// --- INCLUDES ---

// include cstdlib
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
// include cstdlib unix extensions
#include <sys/mman.h>
#include <unistd.h>

// prevent GLFW from including anything for us such as GLAD
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>

struct card_s
{
 char *name;
 // target seconds between frames
 double target_delta_time;
 uint32_t screen_width;
 uint32_t screen_height;
 void (*init)(void);
 void (*update)(void);
 void (*render)(void);
 void (*cleanup)(void);
};

#define CARD_MESH_FLAG_TEXCOORDS_PRESENT 0b1
#define CARD_MESH_FLAG_NORMALS_PRESENT 0b10

#define CARD_MESH_ATTRIBUTE_INDEX_POSITION 0
#define CARD_MESH_ATTRIBUTE_INDEX_TEXCOORD 1
#define CARD_MESH_ATTRIBUTE_INDEX_NORMAL 2

struct card_mesh_vertex_2p_s
{
 float position[2];
};
struct card_mesh_vertex_2pt_s
{
 float position[2];
 float texcoord[2];
};
struct card_mesh_vertex_2pn_s
{
 float position[2];
 float normal[2];
};
struct card_mesh_vertex_2ptn_s
{
 float position[2];
 float texcoord[2];
 float normal[2];
};

struct card_mesh_s
{
 GLuint vao;
 uint32_t n_elements;
};

extern uint32_t card_window_w, card_window_h;
extern GLFWwindow *card_window;

extern GLuint card_essential_shader_program_standard;

/*
 * delta_time: Measured time between each frame
 * fps: Measured frames per second; reciprocal of delta_time
 */
extern double card_delta_time_measured, card_fps;

__attribute__((malloc(free))) char *card_io_util_read_file(const char *filename, bool null_terminate, size_t *size);
__attribute__((malloc(free))) char *card_io_util_read_resource(char *name, bool null_terminate, size_t *size);

GLuint card_shader_create(GLenum type, const GLchar *source);
GLuint card_shader_create_from_resource(char *name, GLenum type);
GLuint card_shader_program_create(uint32_t shaders_length, const GLuint *shaders);
/*
 * A nuclear shader program contains a single vertex shader and a single fragment shader
 */
GLuint card_shader_program_nuclear_create(GLuint shader_vertex, GLuint shader_fragment);
GLuint card_shader_program_nuclear_create_from_resources(char *shader_vertex_name, char *shader_fragment_name);

// Creates a standard texture with resizing filter
GLuint card_texture_create(GLint filter);

#endif
