#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "adc.h"
#include "cyBot_Scan.h"

void adc_init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x02; //Enable GPIO Port B //Right
    SYSCTL_RCGCADC_R |= 0x01; // Enable ADC0

    volatile int delay = SYSCTL_RCGCGPIO_R;
    GPIO_PORTB_DIR_R &= ~0x10; //Disable digital
    GPIO_PORTB_AFSEL_R |= 0x10; // Enable alternate function
    GPIO_PORTB_DEN_R &= ~0x10; //Disable digital
    GPIO_PORTB_AMSEL_R |= 0x10; //Enable analog

    //Enabling ss0
    ADC0_ACTSS_R &= ~0x08; //clearing the bit
    ADC0_EMUX_R &= ~0xF000;
    ADC0_SSMUX3_R = 10;//PB4
    ADC0_SSCTL3_R = 0x06;
    ADC0_SAC_R = 0x04;
    ADC0_ACTSS_R |= 0x08; //setting the bit


}

uint16_t adc_read(void)
{
    ADC0_PSSI_R = 0x08;
    while ((ADC0_RIS_R & 0x08) == 0);
    uint16_t result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = 0x08;
    return result;
}
