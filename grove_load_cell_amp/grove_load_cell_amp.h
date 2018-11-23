/*
 * grove_example.h
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


#ifndef __GROVE_EXAMPLE_H__
#define __GROVE_EXAMPLE_H__

#include "suli2.h"

//GROVE_NAME        "Grove_Load_Cell_Amp"
//SKU               SEN-13879
//IF_TYPE           UART
//IMAGE_URL         https://cdn.sparkfun.com//assets/parts/1/1/5/1/0/13879-01.jpg
//DESCRIPTION       "Load cell amplifier."
//WIKI_URL          https://github.com/Seeed-Studio/Grove_Drivers_for_Wio/wiki/Grove_Example
//ADDED_AT          "2018-11-22"
//AUTHOR            "Abel M."

#define suli_shift_in(io_data, io_clock, bitOrder) shiftIn((*io_data), (*io_clock), bitOrder)

class GroveLoadCellAmp
{
public:
    GroveLoadCellAmp(int pintx, int pinrx);

    /**
     * Reads current measured weight
     *
     * @param weight - the read weight
     *
     * @return bool
     */
    bool read_weight(float *weight);

    /**
     * Sets the offset for next weight readings
     * 
     * @param offset - the offset to set_gain
     * 
     * @return bool
     */
    bool write_offset(float offset);

private:
    IO_T *io_pd_sck;
    IO_T *io_dout;
    byte GAIN;
    float OFFSET;

    void set_gain(byte gain = 128);
    long _read();
    bool is_ready();
    float read_average(byte times = 10);
};

#endif
