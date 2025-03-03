#include "cardemalm_fe.h"

#include <dlfcn.h>

struct _cardfe_card_s _cardfe_active_card = {NULL, {0, NULL}};

static void check_fields(void)
{
    if (_cardfe_active_card.value->name == NULL)
    {
        (void)fprintf(stderr, "Name of active card is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (_cardfe_active_card.value->target_delta_time <= 0.0)
    {
        (void)fprintf(stderr, "Target delta time of active card is <= 0.0\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (_cardfe_active_card.value->screen_width == 0 || _cardfe_active_card.value->screen_height == 0)
    {
        (void)fprintf(
            stderr,
            "Screen dimension of active card is 0: screen width: %u, screen height: %u\n",
            _cardfe_active_card.value->screen_width,
            _cardfe_active_card.value->screen_height
        );
        _cardfe_end_proc(EXIT_FAILURE);
    }
}

void _cardfe_card_active_load(const char *name)
{
    if (name == NULL)
    {
        (void)fprintf(stderr, "Name of card to load is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    const size_t name_length = strlen(name);
    if (name_length == 0)
    {
        (void)fprintf(stderr, "Name length of card to load is 0\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    // unload active card before loading new one
    _cardfe_card_active_unload();
    // build active card shared object filename
    static const char filename_prefix[] = "./cards/libcard_";
    static const char filename_suffix[] = ".so";
    char *filename = calloc(strlen(filename_prefix) + name_length + strlen(filename_suffix) + 1, 1);
    if (filename == NULL)
    {
        (void)fprintf(stderr, "calloc failed: filename of card to load\n");
        _cardfe_print_errno();
        _cardfe_end_proc(EXIT_FAILURE);
    }
    (void)strcpy(filename, filename_prefix);
    (void)strcat(filename, name);
    (void)strcat(filename, filename_suffix);
    // link active card shared object
    _cardfe_active_card.dl_handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    free(filename);
    filename = NULL;
    if (_cardfe_active_card.dl_handle == NULL)
    {
        (void)fprintf(stderr, "Linking active card failed: %s\n", dlerror());
        _cardfe_print_errno();
        _cardfe_end_proc(EXIT_FAILURE);
    }
    // concat resource directory
    static const char resource_directory_prefix[] = "./cards/";
    static const char resource_directory_suffix[] = "/res/";
    _cardfe_active_card.gl_objects.resource_directory = calloc(
        strlen(resource_directory_prefix) + name_length + strlen(resource_directory_suffix) + 1,
        1
    );
    if (_cardfe_active_card.gl_objects.resource_directory == NULL)
    {
        (void)fprintf(stderr, "calloc failed: active card resource directory\n");
        _cardfe_print_errno();
        _cardfe_end_proc(EXIT_FAILURE);
    }
    (void)strcpy(_cardfe_active_card.gl_objects.resource_directory, resource_directory_prefix);
    (void)strcat(_cardfe_active_card.gl_objects.resource_directory, name);
    (void)strcat(_cardfe_active_card.gl_objects.resource_directory, resource_directory_suffix);
    // load card symbol
    _cardfe_active_card.value = dlsym(_cardfe_active_card.dl_handle, "_card");
    if (_cardfe_active_card.value == NULL)
    {
        (void)fprintf(stderr, "Loading active card symbol failed: %s\n", dlerror());
        _cardfe_print_errno();
        _cardfe_end_proc(EXIT_FAILURE);
    }
    check_fields();
    _cardfe_active_card.value->init();
}

void _cardfe_card_active_unload(void)
{
    _cardfe_active_card.value = NULL;
    if (_cardfe_active_card.gl_objects.resource_directory != NULL)
    {
        free(_cardfe_active_card.gl_objects.resource_directory);
        _cardfe_active_card.gl_objects.resource_directory = NULL;
    }
    _cardfe_gl_util_gl_object_buffer_cleanup(&_cardfe_active_card.gl_objects);
    if (_cardfe_active_card.dl_handle != NULL && dlclose(_cardfe_active_card.dl_handle) != 0)
    {
        (void)fprintf(stderr, "Unlinking active card failed: %s\n", dlerror());
        _cardfe_print_errno();
        _cardfe_end_proc(EXIT_FAILURE);
    }
    _cardfe_active_card.dl_handle = NULL;
}
