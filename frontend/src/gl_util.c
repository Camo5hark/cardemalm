#include "cardemalm_fe.h"

static void configure_gl(void)
{
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glEnable(GL_TEXTURE_2D);
}

void _cardfe_gl_util_init(void)
{
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        (void)fprintf(stderr, "GLAD load GLES2 loader failed\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    configure_gl();
}

void _cardfe_gl_util_gl_object_buffer_push(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    const struct _cardfe_gl_util_gl_object_s *object,
    const char *object_description
)
{
    if (buffer == NULL)
    {
        (void)fprintf(stderr, "GL object buffer to push to is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (object == NULL)
    {
        (void)fprintf(stderr, "GL object to push is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (object->value == 0)
    {
        (void)fprintf(stderr, "GL object to push is 0: %s\n", object_description);
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (object->cleanup == NULL)
    {
        (void)fprintf(stderr, "Cleanup function of object to push is NULL: %s\n", object_description);
    }
    if (buffer->length == 0 && buffer->values != NULL)
    {
        (void)fprintf(stderr, "GL object buffer to push to contains 0 objects but is not NULL\n");
        object->cleanup(1, &object->value);
        _cardfe_end_proc(EXIT_FAILURE);
    }
    buffer->length++;
    buffer->values = reallocarray(buffer->values, buffer->length, sizeof(struct _cardfe_gl_util_gl_object_s));
    if (buffer->values == NULL)
    {
        (void)fprintf(stderr, "reallocarray failed: GL object buffer\n");
        _cardfe_print_errno();
        object->cleanup(1, &object->value);
        _cardfe_end_proc(EXIT_FAILURE);
    }
    (void)memcpy(buffer->values + buffer->length - 1, object, sizeof(struct _cardfe_gl_util_gl_object_s));
}

void _cardfe_gl_util_gl_object_buffer_cleanup(struct _cardfe_gl_util_gl_object_buffer_s *buffer)
{
    if (buffer == NULL)
    {
        (void)fprintf(stderr, "GL object buffer to cleanup is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (buffer->length == 0)
    {
        if (buffer->values == NULL)
        {
            return;
        }
        (void)fprintf(stderr, "GL object buffer to cleanup contains 0 objects but is not NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    buffer->length = 0;
    free(buffer->values);
    buffer->values = NULL;
}
