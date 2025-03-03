#include <cardemalm_card.h>

static void init(void)
{
    puts("Hello from default card");
}

static void update(void)
{
}

static void render(void)
{
}

static void cleanup(void)
{
}

struct card_s _card = {
    "default",
    1.0 / 30.0,
    100,
    100,
    &init,
    &update,
    &render,
    &cleanup
};
