

#include "m0_delay.h"

/*lint --e{438} "Variable not used" */
void m0_delay_ms(uint32_t volatile number_of_ms)
{
    while(number_of_ms != 0)
    {
        number_of_ms--;
        m0_delay_us(999);
//			WDT_ClearWDT();
    }
}
