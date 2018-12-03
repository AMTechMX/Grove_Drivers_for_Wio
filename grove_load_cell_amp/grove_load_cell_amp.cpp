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

#include "suli2.h"
#include "grove_load_cell_amp.h"

#if ARDUINO_VERSION <= 106
    void yield(void) {};
#endif


GroveLoadCellAmp::GroveLoadCellAmp(int pintx, int pinrx)
{
    this->io_pd_sck = (IO_T *)malloc(sizeof(IO_T));
    this->io_dout = (IO_T *)malloc(sizeof(IO_T));

    suli_pin_init(io_pd_sck, pinrx, SULI_OUTPUT);
    suli_pin_init(io_dout, pintx, SULI_INPUT);

    set_gain();
    tare();
}

void GroveLoadCellAmp::set_gain(byte gain)
{
    if (gain < 64) {
        GAIN = 2;
    } else if (gain < 128) {
        GAIN = 3;
    } else {
        GAIN = 1;
    }

    suli_pin_write(io_pd_sck, SULI_LOW);
    _read();
}

bool GroveLoadCellAmp::is_ready()
{
    return suli_pin_read(io_dout) == SULI_LOW;
}

long GroveLoadCellAmp::_read()
{
    while (!is_ready())
    {
        yield();
    }

    unsigned long value = 0;

	// pulse the clock pin 24 times to read the data
    for (byte i = 0; i < 24 + GAIN; i++) {
        suli_pin_write(io_pd_sck, SULI_HIGH);
        if (i < 24) {
            value = value << 1 | suli_pin_read(io_dout);
        }
        suli_pin_write(io_pd_sck, SULI_LOW);
    }

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (value & 0x800000) {
		value |= static_cast<unsigned long>(0xff) << 24;
	}

	return static_cast<long>(value);
}

long GroveLoadCellAmp::read_average(byte times)
{
    long sum = 0;
    for (byte i = 0; i < times; i ++) 
    {
        sum += _read();
        yield();
    }
    return sum / times;
}

bool GroveLoadCellAmp::write_offset(float offset)
{
    OFFSET = offset * SCALE;
}

bool GroveLoadCellAmp::read_offset(float *offset)
{
    *offset = OFFSET / SCALE;
}

bool GroveLoadCellAmp::write_scale(float scale)
{
    SCALE = scale;
}

bool GroveLoadCellAmp::read_scale(float *scale)
{
    *scale = SCALE;
}

bool GroveLoadCellAmp::read_weight(float *weight)
{
    (*weight) = (read_average() - OFFSET) / SCALE;
}

void GroveLoadCellAmp::tare(byte times)
{
    OFFSET = read_average(times);
}

