/*
 * grove_ultrasonic_ranger.h
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


#ifndef __GROVE_ENHANCED_ULTRA_RANGER_H__
#define __GROVE_ENHANCED_ULTRA_RANGER_H__

#include "suli2.h"

#define MAX_DISTANCE 150.0

#define LINK_D0 14
#define LINK_D1 12
#define LINK_D2 13

#define NODE_D0 3
#define NODE_D1 5

//GROVE_NAME        "Grove - Enhanced Ultrasonic Ranger"
//SKU               101020010_ENH
//IF_TYPE           GPIO
//IMAGE_URL         https://raw.githubusercontent.com/Seeed-Studio/Grove_Drivers_for_Wio/static/images/grove-Ultrasonic-Ranger.jpg
//DESCRIPTION       "This Grove - Ultrasonic sensor is a non-contact distance measurement module which works at 42KHz, suitable for projects that require middle distance measurement."
//WIKI_URL          http://wiki.seeedstudio.com/Grove-Ultrasonic_Ranger/
//ADDED_AT          "2019-08-18"
//AUTHOR            "AMTech"

class GroveEnhancedUltraRanger
{
public:
    GroveEnhancedUltraRanger(int pin);

    bool on_power_on();

    bool on_power_off();

    /**
     * Get the range / distance between sensor and object.
     *
     * @param range_cm - unit: cm
     *
     * @return bool
     */
    bool read_range_in_cm(float *range_cm);

    void compute_distance();

    DEFINE_EVENT(distance_change_cm, SULI_EDT_STRING);


private:
    IO_T *io;
    TIMER_T *timer;
    float previous_read = -1;

    int portNo;

    uint32_t _get_pulse_width();
};

static void timer_handler_distance_change(void *para) ICACHE_RAM_ATTR;
#endif
