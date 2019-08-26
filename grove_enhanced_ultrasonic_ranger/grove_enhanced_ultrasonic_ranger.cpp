/*
 * grove_ultrasonic_ranger.cpp
 *
 * Copyright (c) 2012 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Jack Shao (jacky.shaoxg@gmail.com)
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "suli2.h"
#include "grove_enhanced_ultrasonic_ranger.h"



GroveEnhancedUltraRanger::GroveEnhancedUltraRanger(int pin)
{
    this->io = (IO_T *)malloc(sizeof(IO_T));
    this->timer = (TIMER_T *)malloc(sizeof(TIMER_T));

    suli_pin_init(io, pin, SULI_OUTPUT);
    on_power_on();

    switch(pin) {
        case LINK_D0:
        case NODE_D0:
            this->portNo = 0;
            break;
        case LINK_D1:
        case NODE_D1:
            this->portNo = 1;
            break;
        case LINK_D2:
            this->portNo = 2;
            break;
    }
}

bool GroveEnhancedUltraRanger::on_power_on() {
    suli_timer_install(timer, 100000, timer_handler_distance_change, this, true);
    return true;
}

bool GroveEnhancedUltraRanger::on_power_off() {
    suli_timer_remove(timer);
    return true;
}

bool GroveEnhancedUltraRanger::read_range_in_cm(float *range_cm)
{
    uint32_t d = _get_pulse_width();
    *range_cm = d / 29.4 / 2;
    return true;
}

// bool GroveEnhancedUltraRanger::read_range_in_inch(float *range_inch)
// {
//     uint32_t d = _get_pulse_width();
//     *range_inch = d / 74.7 / 2;
//     return true;
// }

uint32_t GroveEnhancedUltraRanger::_get_pulse_width()
{
    suli_pin_dir(io, SULI_OUTPUT);
    suli_pin_write(io, SULI_LOW);
    suli_delay_us(2);
    suli_pin_write(io, SULI_HIGH);
    suli_delay_us(5);
    suli_pin_write(io, SULI_LOW);
    suli_pin_dir(io, SULI_INPUT);
    return suli_pin_pulse_in(io, SULI_HIGH, 20000);
}

void GroveEnhancedUltraRanger::compute_distance() {
    char buffer[50];
    uint32_t d = _get_pulse_width();
    float range_cm = d / 29.4 / 2;
    range_cm = min(range_cm, MAX_DISTANCE);
    if (previous_read == -1 || range_cm != previous_read) {
        previous_read = range_cm;
        sprintf(buffer, "{\"port\": \"D%d\", \"val\": \"%f\"", this->portNo, range_cm);
        POST_EVENT_IN_INSTANCE(this, distance_change_cm, &buffer);
    }
}

static void timer_handler_distance_change(void *para) {
    GroveEnhancedUltraRanger *g = (GroveEnhancedUltraRanger *)para;
    g->compute_distance();
}
