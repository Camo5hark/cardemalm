/*
 * Cardemalm frontend interface
 */

#ifndef _CARDEMALM_FRONTEND_H_
#define _CARDEMALM_FRONTEND_H_

#include <cardemalm.h>

struct _cardfe_gl_util_gl_object_s
{
    GLuint value;
    void (*cleanup)(GLsizei, const GLuint*);
};

struct _cardfe_gl_util_gl_object_buffer_s
{
    uint32_t length;
    struct _cardfe_gl_util_gl_object_s *values;
    char *resource_directory;
};

struct _cardfe_io_util_resource_s
{
    struct _cardfe_gl_util_gl_object_buffer_s *gl_objects;
    char *name;
};

struct _cardfe_card_s
{
    // if this is NULL, the card is NULL
    void *dl_handle;
    struct _cardfe_gl_util_gl_object_buffer_s gl_objects;
    struct card_s *value;
};

extern struct _cardfe_gl_util_gl_object_buffer_s _cardfe_card_essentials;

extern struct _cardfe_card_s _cardfe_active_card;

__attribute__((noreturn)) void _cardfe_end_proc(int exit_status);
// print errno and errno description
void _cardfe_print_errno(void);

// initialize window
void _cardfe_window_init(void);
void _cardfe_window_cleanup(void);

// initialize OpenGL capabilities (load symbols)
void _cardfe_gl_util_init(void);
void _cardfe_gl_util_gl_object_buffer_push(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    const struct _cardfe_gl_util_gl_object_s *object,
    const char *object_description
);
void _cardfe_gl_util_gl_object_buffer_cleanup(struct _cardfe_gl_util_gl_object_buffer_s *buffer);

__attribute__((malloc(free))) char *_cardfe_io_util_read_resource(
    struct _cardfe_io_util_resource_s *resource,
    bool null_terminate,
    size_t *size
);

GLuint _cardfe_shader_create(struct _cardfe_gl_util_gl_object_buffer_s *buffer, GLenum type, const GLchar *source);
GLuint _cardfe_shader_create_from_resource(
    struct _cardfe_io_util_resource_s *resource,
    GLenum type
);
GLuint _cardfe_shader_program_create(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    uint32_t shaders_length,
    const GLuint *shaders
);
GLuint _cardfe_shader_program_nuclear_create(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    GLuint shader_vertex,
    GLuint shader_fragment
);
GLuint _cardfe_shader_program_nuclear_create_from_resources(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    char *shader_vertex_name,
    char *shader_fragment_name
);

GLuint _cardfe_texture_create(struct _cardfe_gl_util_gl_object_buffer_s *buffer, GLint filter);

void _cardfe_mesh_create(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    struct card_mesh_s *mesh,
    uint32_t position_length,
    uint32_t n_vertices,
    const void *vertices,
    uint32_t n_elements,
    const uint32_t *elements,
    uint32_t flags
);

void _cardfe_card_essential_init(void);

void _cardfe_card_active_load(const char* name);
void _cardfe_card_active_unload(void);

#endif
