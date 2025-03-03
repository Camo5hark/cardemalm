#include "cardemalm_fe.h"

// quarter gb
#define MAX_FILE_READ_SIZE 268435456

__attribute__((malloc(free))) static char *concat_resource_filename(struct _cardfe_io_util_resource_s *resource)
{
    if (resource == NULL)
    {
        (void)fprintf(stderr, "Resource to concatenate is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (resource->gl_objects == NULL)
    {
        (void)fprintf(stderr, "GL object buffer of resource to concatenate is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (resource->gl_objects->resource_directory == NULL)
    {
        (void)fprintf(stderr, "Directory of GL object buffer of resource to concatenate is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (resource->name == NULL)
    {
        (void)fprintf(stderr, "Name of resource to concatenate is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    const size_t directory_length = strlen(resource->gl_objects->resource_directory);
    if (directory_length == 0)
    {
        (void)fprintf(stderr, "Directory length of GL object buffer of resource to concatenate is 0\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    const size_t name_length = strlen(resource->name);
    if (name_length == 0)
    {
        (void)fprintf(stderr, "Name length of resource to concatenate is 0\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    char *filename = calloc(directory_length + name_length + 1, 1);
    if (filename == NULL)
    {
        (void)fprintf(stderr, "calloc failed: resource filename\n");
        _cardfe_print_errno();
        _cardfe_end_proc(EXIT_FAILURE);
    }
    (void)strcpy(filename, resource->gl_objects->resource_directory);
    (void)strcat(filename, resource->name);
    return filename;
}

__attribute__((malloc(free))) char *card_io_util_read_file(
    const char *filename,
    const bool null_terminate,
    size_t *size
)
{
    if (filename == NULL)
    {
        (void)fprintf(stderr, "Filename of file to read is NULL\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    if (strlen(filename) == 0)
    {
        (void)fprintf(stderr, "Filename length of file to read is 0\n");
        _cardfe_end_proc(EXIT_FAILURE);
    }
    FILE *file = fopen(filename, "rw");
    if (file == NULL)
    {
        (void)fprintf(stderr, "Open file to read failed: %s\n", filename);
        _cardfe_print_errno();
        _cardfe_end_proc(EXIT_FAILURE);
    }
    bool error = false;
    if (fseek(file, 0, SEEK_END) == -1)
    {
        (void)fprintf(stderr, "Seek to end of file to read failed: %s\n", filename);
        _cardfe_print_errno();
        error = true;
        goto cleanup;
    }
    const size_t _size = ftell(file);
    if (_size == -1)
    {
        (void)fprintf(stderr, "Get file to read position failed: %s\n", filename);
        _cardfe_print_errno();
        error = true;
        goto cleanup;
    }
    if (_size == 0 || _size > MAX_FILE_READ_SIZE)
    {
        (void)fprintf(
            stderr,
            "Size of file to read is out of bounds: size: %lu, bounds: (0, %u)\n",
            _size,
            MAX_FILE_READ_SIZE
        );
        error = true;
        goto cleanup;
    }
    if (fseek(file, 0, SEEK_SET) == -1)
    {
        (void)fprintf(stderr, "Seek to start of file to read failed: %s\n", filename);
        _cardfe_print_errno();
        error = true;
        goto cleanup;
    }
    const size_t _size_null_terminated = _size + (null_terminate ? 1 : 0);
    char *contents = malloc(_size_null_terminated);
    if (contents == NULL)
    {
        (void)fprintf(stderr, "malloc failed: file to read contents: %s\n", filename);
        _cardfe_print_errno();
        error = true;
        goto cleanup;
    }
    for (size_t i = 0; i < _size; i++)
    {
        contents[i] = fgetc(file);
    }
    if (null_terminate)
    {
        contents[_size] = '\0';
    }
    if (size != NULL)
    {
        *size = _size_null_terminated;
    }
cleanup:
    if (fclose(file) == EOF)
    {
        (void)fprintf(stderr, "Close file to read failed: %s\n", filename);
        _cardfe_print_errno();
        error = true;
    }
    if (error)
    {
        if (contents != NULL)
        {
            free(contents);
            contents = NULL;
        }
        _cardfe_end_proc(EXIT_FAILURE);
    }
    return contents;
}

__attribute__((malloc(free))) char *_cardfe_io_util_read_resource(
    struct _cardfe_io_util_resource_s *resource,
    const bool null_terminate,
    size_t *size
)
{
    char *filename = concat_resource_filename(resource);
    char *contents = card_io_util_read_file(filename, null_terminate, size);
    free(filename);
    filename = NULL;
    return contents;
}

__attribute__((malloc(free))) char *card_io_util_read_resource(char *name, const bool null_terminate, size_t *size)
{
    struct _cardfe_io_util_resource_s resource = {&_cardfe_active_card.gl_objects, name};
    return _cardfe_io_util_read_resource(&resource, null_terminate, size);
}
