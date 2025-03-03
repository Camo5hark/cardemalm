#include "cardemalm_fe.h"

uint32_t card_window_w = 0, card_window_h = 0;
GLFWwindow *card_window = NULL;

void _cardfe_window_init(void)
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (monitor == NULL)
    {
        (void)fprintf(stderr, "GLFW primary monitor is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    const GLFWvidmode *video_mode = glfwGetVideoMode(monitor);
    if (video_mode == NULL)
    {
        (void)fprintf(stderr, "GLFW primary monitor video mode is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    card_window_w = video_mode->width;
    card_window_h = video_mode->height;
    if (card_window_w == 0 || card_window_h == 0)
    {
        (void)fprintf(stderr, "Monitor dimension is 0: width: %u, height: %u\n", card_window_w, card_window_h);
        _cardfe_end_proc(EXIT_FAILURE);
    }
    glfwDefaultWindowHints();
    // make window invisible during creation
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // window size never needs to change
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // configure OpenGL ES support
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    card_window = glfwCreateWindow(card_window_w, card_window_h, "cardemalm", monitor, NULL);
    if (card_window == NULL)
    {
        (void)fprintf(stderr, "Window creation failed\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(card_window);
    // disable v-sync
    glfwSwapInterval(0);
    // make window visible now that it has been created
    glfwShowWindow(card_window);
    // cursor not needed
    glfwSetInputMode(card_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void _cardfe_window_cleanup(void)
{
    if (card_window == NULL)
    {
        return;
    }
    glfwDestroyWindow(card_window);
    card_window = NULL;
}
