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


GroveLoadCellAmp::GroveLoadCellAmp(int pintx, int pinrx)
{
    this->io_pd_sck = (IO_T *)malloc(sizeof(IO_T));
    this->io_dout = (IO_T *)malloc(sizeof(IO_T));

    suli_pin_init(io_pd_sck, pinrx, SULI_OUTPUT);
    suli_pin_init(io_dout, pintx, SULI_INPUT);
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
	byte data[3] = { 0 };
	byte filler = 0x00;

	// pulse the clock pin 24 times to read the data
	data[2] = suli_shift_in(io_dout, io_pd_sck, MSBFIRST);
	data[1] = suli_shift_in(io_dout, io_pd_sck, MSBFIRST);
	data[0] = suli_shift_in(io_dout, io_pd_sck, MSBFIRST);

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN; i++) {
        suli_pin_write(io_pd_sck, SULI_HIGH);
        suli_pin_write(io_pd_sck, LOW);
	}

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( static_cast<unsigned long>(filler) << 24
			| static_cast<unsigned long>(data[2]) << 16
			| static_cast<unsigned long>(data[1]) << 8
			| static_cast<unsigned long>(data[0]) );

	return static_cast<long>(value);

}

float GroveLoadCellAmp::read_average(byte times)
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
    OFFSET = offset;
}

bool GroveLoadCellAmp::read_weight(float *weight)
{
    (*weight) = read_average() - OFFSET;
}

