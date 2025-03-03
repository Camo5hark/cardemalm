#include "cardemalm_fe.h"

GLuint _cardfe_texture_create(struct _cardfe_gl_util_gl_object_buffer_s *buffer, const GLint filter)
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    const struct _cardfe_gl_util_gl_object_s gl_object_texture = {texture, glDeleteTextures};
    _cardfe_gl_util_gl_object_buffer_push(buffer, &gl_object_texture, "texture");
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

GLuint card_texture_create(const GLint filter)
{
    return _cardfe_texture_create(&_cardfe_active_card.gl_objects, filter);
}
