#include "cardemalm_fe.h"

static void init_buffer_object(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    const GLenum target,
    const size_t data_size,
    const void *data
)
{
    GLuint buffer_object = 0;
    glGenBuffers(1, &buffer_object);
    const struct _cardfe_gl_util_gl_object_s gl_object_buffer_object = {buffer_object, glDeleteBuffers};
    _cardfe_gl_util_gl_object_buffer_push(buffer, &gl_object_buffer_object, "mesh buffer object");
    glBindBuffer(target, buffer_object);
    glBufferData(target, data_size, data, GL_STATIC_DRAW);
}

static void init_vertex_attribute(off_t *offset, const uint32_t index, const uint32_t length, const size_t vertex_size)
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, length, GL_FLOAT, GL_FALSE, vertex_size, offset);
    *offset += length;
}

void _cardfe_mesh_create(
    struct _cardfe_gl_util_gl_object_buffer_s *buffer,
    struct card_mesh_s *mesh,
    const uint32_t position_length,
    const uint32_t n_vertices,
    const void *vertices,
    const uint32_t n_elements,
    const uint32_t *elements,
    const uint32_t flags
)
{
    if (position_length == 0 || position_length > 4)
    {
        (void)fprintf(stderr,
            "Mesh position length is out of bounds: position length: %u, min: 1, max: 4\n",
            position_length
        );
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (n_vertices == 0)
    {
        (void)fprintf(stderr, "Mesh vertex count is 0\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (vertices == NULL)
    {
        (void)fprintf(stderr, "Mesh vertex buffer is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (n_elements == 0)
    {
        (void)fprintf(stderr, "Mesh element count is 0\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (elements == NULL)
    {
        (void)fprintf(stderr, "Mesh element buffer is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    glGenVertexArrays(1, &mesh->vao);
    const struct _cardfe_gl_util_gl_object_s gl_object_vao = {mesh->vao, glDeleteVertexArrays};
    _cardfe_gl_util_gl_object_buffer_push(buffer, &gl_object_vao, "mesh vertex array");
    glBindVertexArray(mesh->vao);
    const uint32_t texcoords_present = flags & CARD_MESH_FLAG_TEXCOORDS_PRESENT;
    const uint32_t normals_present = flags & CARD_MESH_FLAG_NORMALS_PRESENT;
    static const uint32_t texcoord_length = 2;
    const size_t position_size = position_length * sizeof(float);
    static const size_t texcoord_size = texcoord_length * sizeof(float);
    const size_t vertex_size =
        position_size +
        (texcoords_present ? texcoord_size : 0) +
        (normals_present ? position_size : 0);
    init_buffer_object(buffer, GL_ARRAY_BUFFER, n_vertices * vertex_size, vertices);
    init_buffer_object(buffer, GL_ELEMENT_ARRAY_BUFFER, n_elements * sizeof(uint32_t), elements);
    off_t vertex_attribute_offset = 0;
    init_vertex_attribute(
        &vertex_attribute_offset,
        CARD_MESH_ATTRIBUTE_INDEX_POSITION,
        position_length,
        vertex_size
    );
    if (texcoords_present)
    {
        init_vertex_attribute(
            &vertex_attribute_offset,
            CARD_MESH_ATTRIBUTE_INDEX_TEXCOORD,
            texcoord_length,
            vertex_size
        );
    }
    if (normals_present)
    {
        init_vertex_attribute(
            &vertex_attribute_offset,
            CARD_MESH_ATTRIBUTE_INDEX_NORMAL,
            position_length,
            vertex_size
        );
    }
    glBindVertexArray(0);
    mesh->n_elements = n_elements;
}

void card_mesh_draw(const uint64_t mesh)
{
    glBindVertexArray((GLuint)(mesh >> 32));
    glDrawElements(GL_TRIANGLES, (GLsizei)mesh, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}
