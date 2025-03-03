/*
 * Executable entry point and utilities
 */

#include "cardemalm_fe.h"

#include <sys/ioctl.h>

double card_delta_time_measured = 0.0, card_fps = 0.0;



// --- STATICS ---



// INIT PROCESSES

static void on_glfw_error(const int error, const char *description)
{
    (void)fprintf(stderr, "GLFW error: %d: %s\n", error, description);
    _cardfe_end_proc(EXIT_FAILURE);
}

static void init_glfw(void)
{
    glfwSetErrorCallback(on_glfw_error);
    if (!glfwInit())
    {
        (void)fprintf(stderr, "GLFW initialization failed\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
}

// main init process
static void init(void)
{
    init_glfw();
    _cardfe_window_init();
    _cardfe_gl_util_init();
    _cardfe_card_essential_init();
}

static void proc_cmd(void)
{
    // get available bytes in stdin
    size_t stdin_available = 0;
    if (ioctl(STDIN_FILENO, FIONREAD, &stdin_available) == -1)
    {
        (void)fprintf(stderr, "Failed to get available byte count from STDIN\n");
        _cardfe_print_errno();
        _cardfe_end_proc(EXIT_FAILURE);
    }
    // do nothing if no bytes are available
    if (stdin_available == 0)
    {
        return;
    }
    // otherwise, process command char
    const int cmd = fgetc(stdin);
    if (cmd == EOF)
    {
        return;
    }
    switch ((char)cmd) {
    default:
        break;
    case 'q':
    case 's':
        _cardfe_end_proc(EXIT_SUCCESS);
    }
}

static void update(void)
{
    proc_cmd();
    if (glfwGetKey(card_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        _cardfe_end_proc(EXIT_SUCCESS);
    }
}

static void render(void)
{
}

// main process/loop
__attribute__((noreturn)) static void proc(void)
{
    double last_frame_time = glfwGetTime();
    while (true)
    {
        if (_cardfe_active_card.dl_handle == NULL)
        {
            _cardfe_card_active_load("default");
        }
        const double time = glfwGetTime();
        card_delta_time_measured = time - last_frame_time;
        card_fps = 1.0 / card_delta_time_measured;
        if (card_delta_time_measured < _cardfe_active_card.value->target_delta_time)
        {
            continue;
        }
        last_frame_time = time;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        update();
        render();
        glfwSwapBuffers(card_window);
        glfwPollEvents();
    }
}

// cleanup/terminate process
static void cleanup(void)
{
    _cardfe_card_active_unload();
    _cardfe_gl_util_gl_object_buffer_cleanup(&_cardfe_card_essentials);
    _cardfe_window_cleanup();
    glfwTerminate();
}



// --- GLOBALS ---



__attribute__((noreturn)) void _cardfe_end_proc(const int exit_status)
{
    cleanup();
    exit(exit_status);
}

void _cardfe_print_errno(void)
{
    const int _errno = errno;
    (void)fprintf(stderr, "errno: %d: %s\n", _errno, strerror(_errno));
}

// executable entry point - no args needed
void main(void)
{
    init();
    proc();
}
