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
void yield(void){};
#endif

GroveLoadCellAmp::GroveLoadCellAmp(int pintx, int pinrx)
{
    this->io_pd_sck = (IO_T *)malloc(sizeof(IO_T));
    this->io_dout = (IO_T *)malloc(sizeof(IO_T));

    suli_pin_init(io_pd_sck, pinrx, SULI_OUTPUT);
    suli_pin_init(io_dout, pintx, SULI_INPUT);

    set_gain();
    powered_on = true;
}

void GroveLoadCellAmp::set_gain(byte gain)  
{
    switch (gain)
    {
    case 128:
        GAIN = 1;
        break;
    case 64:
        GAIN = 3;
        break;
    case 32:
        GAIN = 2;
        break;
    }

    write_power(1);
    _read();
}

bool GroveLoadCellAmp::is_ready()
{
    return suli_pin_read(io_dout) == SULI_LOW;
}

long GroveLoadCellAmp::_read()
{
    if (!powered_on) {
        return 0;
    }

    while (!is_ready())
    {
        yield();
    }

    unsigned long value = 0;
    int dout;

    // pulse the clock pin 24 times to read the data
    for (byte i = 0; i < 24 + GAIN; i++)
    {
        delayMicroseconds(1);
        suli_pin_write(io_pd_sck, SULI_HIGH);
        delayMicroseconds(1);
        if (i < 24)
        {
            dout = suli_pin_read(io_dout);
            value <<= 1;
            if (dout) {
                value += 1;
            }
        }
        suli_pin_write(io_pd_sck, SULI_LOW);
    }

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if (value & 0x800000)
    {
        value |= 0xff000000;
    }

    return static_cast<long>(value);
}

long GroveLoadCellAmp::get_average(byte times)
{
    if (!powered_on) {
        return 0;
    }
    long sum = 0;
    for (byte i = 0; i < times; i++)
    {
        sum += _read();
        yield();
    }
    return sum / times;
}

bool GroveLoadCellAmp::write_offset(int offset)
{
    OFFSET = offset;
    return true;
}

bool GroveLoadCellAmp::read_offset(int *offset)
{
    *offset = OFFSET;
    return true;
}

bool GroveLoadCellAmp::write_scale(float scale)
{
    SCALE = scale;
    return true;
}

bool GroveLoadCellAmp::write_tare()
{
    tare();
    return true;
}

bool GroveLoadCellAmp::write_power(int onoff)
{
    suli_pin_write(io_pd_sck, SULI_LOW);
    if (!onoff)
    {
        suli_pin_write(io_pd_sck, SULI_HIGH);
    }
    powered_on = static_cast<bool>(onoff);
    return true;
}

bool GroveLoadCellAmp::read_scale(float *scale)
{
    *scale = SCALE;
    return true;
}

bool GroveLoadCellAmp::read_weight(float *weight)
{
    if (!powered_on) {
        error_desc = "Cannot read weight if the scale is powered off";
        return false;
    }
    (*weight) = (get_average() - OFFSET) / SCALE;
    return true;
}

void GroveLoadCellAmp::tare(byte times)
{
    OFFSET = get_average(times);
}
