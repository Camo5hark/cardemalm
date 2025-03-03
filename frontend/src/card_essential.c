#include "cardemalm_fe.h"

struct _cardfe_gl_util_gl_object_buffer_s _cardfe_card_essentials = {0, NULL, "./res/"};
GLuint card_essential_shader_program_standard = 0;

void _cardfe_card_essential_init(void)
{
    card_essential_shader_program_standard = _cardfe_shader_program_nuclear_create_from_resources(
        &_cardfe_card_essentials,
        "shaders/standard.vsh",
        "shaders/standard.fsh"
    );

    struct card_mesh_vertex_2pt_s vertices[] = {
        {{-0.5, 0.5}, {0.0, 0.0}},
        {{0.5, 0.5}, {1.0, 0.0}},
        {{0.5, -0.5}, {1.0, 1.0}},
        {{-0.5, -0.5}, {0.0, 1.0}}
    };
    uint32_t elements[] = {
        0, 1, 2,
        2, 3, 0
    };
    struct card_mesh_s quad = {0, 0};
    _cardfe_mesh_create(
        &_cardfe_card_essentials,
        &quad,
        2,
        4,
        vertices,
        6,
        elements,
        CARD_MESH_FLAG_TEXCOORDS_PRESENT | CARD_MESH_FLAG_NORMALS_PRESENT
    );
    printf("%u %u\n", quad.vao, quad.n_elements);
}
