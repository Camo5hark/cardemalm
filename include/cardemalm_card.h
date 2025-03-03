#ifndef CARDEMALM_CORE_API_H
#define CARDEMALM_CORE_API_H

#include <cardemalm.h>

extern struct card_s _card;

void _card_init(void);
void _card_update(void);
void _card_render(void);
void _card_cleanup(void);

#endif
