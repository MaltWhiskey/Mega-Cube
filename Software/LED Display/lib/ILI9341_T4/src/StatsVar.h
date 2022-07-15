/******************************************************************************
*  ILI9341_T4 library for driving an ILI9341 screen via SPI with a Teensy 4/4.1
*  Implements vsync and differential updates from a memory framebuffer.
*
*  Copyright (c) 2020 Arvind Singh.  All right reserved.
*
* This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************/
#ifndef _ILI9341_T4_STATSVAR_H_
#define _ILI9341_T4_STATSVAR_H_

// only C++, no plain C
#ifdef __cplusplus


#include <stdint.h>
#include <Arduino.h>

#define ILI9341_T4_ALWAYS_INLINE __attribute__((always_inline))

namespace ILI9341_T4
{


/**
 * Class that stores some statistics about a sequence of int32 values.
 * It keeps track of:
 * 
 * - the min value of the sequence
 * - the max value of the sequence
 * - the average value of the sequence. 
 * - the standard deviation around the average.  
 * 
 **/
 class StatsVar
    {
    public:
  
        /** ctor. */
        StatsVar()
            {
            reset(); 
            }


        /**
         * Reset all statistics to their default values. 
         **/
        void reset()
            {
            _count = 0; 
            _min = INT32_MAX;
            _max = INT32_MIN;
            _sum = 0;
            _sumsqr = 0;
            }


        /**
         * Add a new value to the sequence.
         **/
        void push(int32_t val)  ILI9341_T4_ALWAYS_INLINE
            {
            _count++;
            _sum += val;
            _sumsqr += (val * val);
            if (val < _min) _min = val; 
            if (val > _max) _max = val;
            }


        /**
         * Output the statistics into a stream. 
         **/
        void print(const char* unit, const char * endl, Stream* outputStream, bool with_precision = false) const
            {
            if (outputStream)
                {
                if (with_precision)
                    outputStream->printf("avg=%.2f%s [min=%d%s , max=%d%s] std=%.2f%s%s", avg(), unit, min(), unit, max(), unit, std(), unit, endl);
                else
                    outputStream->printf("avg=%.0f%s [min=%d%s , max=%d%s] std=%.0f%s%s", avg(), unit, min(), unit, max(), unit, std(), unit, endl);
                }
            }


        /**
         * Return the current number of record pushed (since the last reset);
         **/
        uint32_t count() const { return _count; }


        /**
         * Return the minimum value recorded (since the last reset). 
         **/
        int32_t min() const { return _min; }


        /**
         * Return the maximum value recorded (since the last reset).
         **/
        int32_t max() const { return _max; }


        /**
         * Return the average value of all records (since the last reset).
         **/
        float avg() const { return ((_count == 0) ? 0 : (((float)_sum) / _count)); }


        /**
         * Return the std around its average off all records (since the last reset).
         **/
        float std() const 
            {
            if (_count == 0) return 0.0f;
            const float a = _sum;
            const float b = ((((uint64_t)_sumsqr) >> 32) * 4294967296.0f) + (((uint64_t)_sumsqr) & 0xFFFFFFFF); // BUG: converting directly from uint64 to double fails so we do this dirty workaround...
            const float c = sqrt((b - ((a*a)/ _count))/_count);
            return c;
            }


        private:

            uint32_t _count; 
            int32_t _min;
            int32_t _max;
            int64_t _sum;
            int64_t _sumsqr;
    };




}

#endif

#endif

/** end of file */

