/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "stepper.pio.h"

#define DELAY_ADJUST 0.1329
#define CALC_STEP_DELAY(ticks) ((ticks*(1.0f-DELAY_ADJUST)) )
#define CALC_STEP_DIR(steps,dir) (((steps-1) << 1)|(dir & 0x01))

// Static test data for ramp up, one constant speed turn, ramp down
// and reverse direction back. First 32bit word has step count and 
// 1/0 for step direction, second word has the delay in PIO clock
// ticks. Timing is consistantly about 15% off but porportionally 
// consistant.
#define STEP_PROFILE_WORDS (12 * 2) 
int32_t step_profile[] = {
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(48000),   // 999.3us, 1 step
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(24000),   // 499.72us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(12000),   // 249.96us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(6000),    // 125.06us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(3000),    // 62.6us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(1500),    // 31.38us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(750),     // 15.78us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(375),     // 8.0us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(187),     // 4.06us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(93),      // 2.1us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(48),      // 1.18us
	CALC_STEP_DIR(3200,0), CALC_STEP_DELAY(48),   // 1.14us
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(48),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(93),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(187),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(375),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(750),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(1500),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(3000),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(6000),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(12000),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(24000),
	CALC_STEP_DIR(1,0), CALC_STEP_DELAY(48000),
	CALC_STEP_DIR(0,1), CALC_STEP_DELAY(0),      // This will change direction without a step
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,1), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,1), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,1), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(48000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(24000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(12000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(60000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(3000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(1500),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(750),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(375),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(187),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(93),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(48),
	CALC_STEP_DIR(3200,1), CALC_STEP_DELAY(48),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(48),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(93),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(187),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(375),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(750),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(1500),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(3000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(6000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(12000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(24000),
	CALC_STEP_DIR(1,1), CALC_STEP_DELAY(48000),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0),
	CALC_STEP_DIR(0,0), CALC_STEP_DELAY(0)
};

#define STEPPER_STEP_PIN 10u
#define TRIGGER_PIN 12u

// Selected DMA channel
int dma_chan;

void dma_handler() {
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << dma_chan;
    // Give the channel a new wave table entry to read from, and re-trigger it
    dma_channel_set_read_addr(dma_chan, &step_profile[0], true);	
}

int main() {
    // Choose which PIO instance to use (there are two instances)
    PIO pio = pio0;

    gpio_init(TRIGGER_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_put(TRIGGER_PIN, 0);

    // Set up a PIO state machine to serialise our bits
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &stepper_program);
    stepper_program_init(pio, sm, offset, STEPPER_STEP_PIN);

#if false
    // Configure a channel to write the same word (32 bits) repeatedly to pio
    // SM0's TX FIFO, paced by the data request signal from that peripheral.
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);

    dma_channel_configure(
        dma_chan,
        &c,
        &pio0_hw->txf[0], // Write address (only need to set this once)
        NULL,             // Don't provide a read address yet
        STEP_PROFILE_WORDS,  // Write this many DMA_SIZE_32 words, halt, trigger interrupt
        false             // Don't start yet
    );

    // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq0_enabled(dma_chan, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // Manually call the handler once, to trigger the first transfer
    dma_handler();
#else
    //pio_sm_clear_fifos(pio, sm);
    //pio_sm_restart(pio, sm);
    for(int i=0; i < STEP_PROFILE_WORDS; i++ ) {
        gpio_put(TRIGGER_PIN, 1);
        pio_sm_put_blocking(pio, sm, step_profile[i] );
        gpio_put(TRIGGER_PIN, 0);
    }
#endif


    // Everything else from this point is interrupt-driven. The processor has
    // time to sit and think about its early retirement -- maybe open a bakery?
    while (true)
        tight_loop_contents();
}
