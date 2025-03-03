#include "cardemalm_fe.h"

static void cleanup_shader(__attribute__((unused)) GLsizei unused_00, const GLuint *shader)
{
    glDeleteShader(*shader);
}

static void cleanup_shader_program(__attribute__((unused)) GLsizei unused_00, const GLuint *shader_program)
{
    glDeleteProgram(*shader_program);
}

static void check_status(
    const GLuint object,
    const char *object_description,
    void (*get_status)(GLuint, GLenum, GLint*),
    const GLenum status_type,
    const char *status_description,
    void (*get_info_log)(GLuint, GLsizei, GLsizei*, GLchar*)
)
{
    GLint status = GL_FALSE;
    get_status(object, status_type, &status);
    if (status == GL_TRUE)
    {
        return;
    }
    (void)fprintf(
        stderr,
        "Shader object status check failed: object: %s, status: %s\n",
        object_description,
        status_description
    );
    static const size_t info_log_max_length = 4096;
    char *info_log = calloc(info_log_max_length, 1);
    if (info_log == NULL)
    {
        (void)fprintf(stderr, "calloc failed: shader object info log: object: %s\n", object_description);
        _cardfe_end_proc(EXIT_FAILURE);
    }
    get_info_log(object, info_log_max_length, NULL, info_log);
    (void)fprintf(stderr, "Info log: \n%s\n", info_log);
    free(info_log);
    info_log = NULL;
    _cardfe_end_proc(EXIT_FAILURE);
}

GLuint _cardfe_shader_create(struct _cardfe_gl_util_gl_object_buffer_s *buffer, const GLenum type, const GLchar *source)
{
    if (source == NULL)
    {
        (void)fprintf(stderr, "Shader source is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (strlen(source) == 0)
    {
        (void)fprintf(stderr, "Shader source length is 0\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    const GLuint shader = glCreateShader(type);
    const struct _cardfe_gl_util_gl_object_s gl_object_shader = {shader, cleanup_shader};
    _cardfe_gl_util_gl_object_buffer_push(buffer, &gl_object_shader, "shader");
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    check_status(
        shader,
        "shader",
        glGetShaderiv,
        GL_COMPILE_STATUS,
        "compile",
        glGetShaderInfoLog
    );
    return shader;
}

GLuint card_shader_create(const GLenum type, const GLchar *source)
{
    return _cardfe_shader_create(&_cardfe_active_card.gl_objects, type, source);
}

GLuint _cardfe_shader_create_from_resource(
    struct _cardfe_io_util_resource_s *resource,
    const GLenum type
)
{
    char *source = _cardfe_io_util_read_resource(resource, true, NULL);
    const GLuint shader = _cardfe_shader_create(resource->gl_objects, type, source);
    free(source);
    source = NULL;
    return shader;
}

GLuint card_shader_create_from_resource(char *name, const GLenum type)
{
    struct _cardfe_io_util_resource_s resource = {&_cardfe_active_card.gl_objects, name};
    return _cardfe_shader_create_from_resource(&resource, type);
}

GLuint _cardfe_shader_program_create(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    const uint32_t shaders_length,
    const GLuint *shaders
)
{
    if (shaders == NULL)
    {
        (void)fprintf(stderr, "Shader program shader array is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    const GLuint shader_program = glCreateProgram();
    const struct _cardfe_gl_util_gl_object_s gl_object_shader_program = {shader_program, cleanup_shader_program};
    _cardfe_gl_util_gl_object_buffer_push(buffer, &gl_object_shader_program, "shader program");
    uint32_t i = 0;
    GLuint shader = 0;
    if (shaders_length != 0)
    {
        for (; i < shaders_length; i++)
        {
            shader = shaders[i];
            if (shader == 0)
            {
                (void)fprintf(stderr, "Shader to attach to shader program is 0\n");
                _cardfe_end_proc(EXIT_FAILURE);
            }
            glAttachShader(shader_program, shader);
        }
    } else
    {
        while ((shader = shaders[i]) != 0)
        {
            glAttachShader(shader_program, shader);
            i++;
        }
    }
    glLinkProgram(shader_program);
    check_status(
        shader_program,
        "shader program",
        glGetProgramiv,
        GL_LINK_STATUS,
        "link",
        glGetProgramInfoLog
    );
    glValidateProgram(shader_program);
    check_status(
        shader_program,
        "shader program",
        glGetProgramiv,
        GL_VALIDATE_STATUS,
        "validate",
        glGetProgramInfoLog
    );
    return shader_program;
}

GLuint card_shader_program_create(const uint32_t shaders_length, const GLuint *shaders)
{
    return _cardfe_shader_program_create(&_cardfe_active_card.gl_objects, shaders_length, shaders);
}

GLuint _cardfe_shader_program_nuclear_create(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    const GLuint shader_vertex,
    const GLuint shader_fragment
)
{
    const GLuint shaders[] = {shader_vertex, shader_fragment};
    return _cardfe_shader_program_create(buffer, 2, shaders);
}

GLuint card_shader_program_nuclear_create(const GLuint shader_vertex, const GLuint shader_fragment)
{
    return _cardfe_shader_program_nuclear_create(&_cardfe_active_card.gl_objects, shader_vertex, shader_fragment);
}

GLuint _cardfe_shader_program_nuclear_create_from_resources(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    char *shader_vertex_name,
    char *shader_fragment_name
)
{
    struct _cardfe_io_util_resource_s resource = {buffer, shader_vertex_name};
    const GLuint shader_vertex = _cardfe_shader_create_from_resource(&resource, GL_VERTEX_SHADER);
    resource.name = shader_fragment_name;
    const GLuint shader_fragment = _cardfe_shader_create_from_resource(&resource, GL_FRAGMENT_SHADER);
    return _cardfe_shader_program_nuclear_create(buffer, shader_vertex, shader_fragment);
}

GLuint card_shader_program_nuclear_create_from_resources(
    char *shader_vertex_name,
    char *shader_fragment_name
)
{
    return _cardfe_shader_program_nuclear_create_from_resources(
        &_cardfe_active_card.gl_objects,
        shader_vertex_name,
        shader_fragment_name
    );
}
