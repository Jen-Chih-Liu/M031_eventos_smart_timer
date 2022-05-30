/* include ------------------------------------------------------------------ */
#include "eventos.h"
#include "event_def.h"
#include <stdio.h>
#include "string.h"
#include "NuMicro.h"
/* data structure ----------------------------------------------------------- */
typedef struct eos_reactor_gpio_int_tag {
    eos_reactor_t super;
    void *          data;
    eos_u16_t       size;
} eos_reactor_gpio_int_t;

eos_reactor_gpio_int_t actor_gpio_int;

/* static event handler ----------------------------------------------------- */
static void gpio_int_e_handler(eos_reactor_gpio_int_t * const me, eos_event_t const * const e);

/* api ---------------------------------------------------- */
void eos_reactor_gpio_init(void)
{
    eos_reactor_init(&actor_gpio_int.super, 3, EOS_NULL);
    eos_reactor_start(&actor_gpio_int.super, EOS_HANDLER_CAST(gpio_int_e_handler));


#if (EOS_USE_PUB_SUB != 0)
    eos_event_sub((eos_actor_t *)(&actor_gpio_int), Event_gpio_int);
#endif

}

/* static state function ---------------------------------------------------- */
static void gpio_int_e_handler(eos_reactor_gpio_int_t * const me, eos_event_t const * const e)
{
    if (e->topic == Event_gpio_int) {
        printf("%s\n\r",(char *)e->data);
			  printf("%d\n\r",e->size);
			  //printf("gpio_int_handler a\n\r");
    }
}

void GPABGH_IRQHandler(void)
{
    volatile uint32_t temp;

    /* To check if PB.2 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PB, BIT0))
    {
        GPIO_CLR_INT_FLAG(PB, BIT0);
        const char *news = "S has been married again with one Bangzi!";
         eos_event_pub(Event_gpio_int, (void *)news, strlen(news));
    }
    
}
