/* Includes ------------------------------------------------------------------*/
#include "driver_initialization.h"
#include "test.h"

int main(void)
{
    soc_configuration();
    platform_configuration();

//    output_clock(SYSCLOCK);

    print_test_header();

    spi_test();

    while(1)
    {
    };
}
