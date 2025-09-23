/* Library for SPI flash 25* devices.
 * Copyright (c) 2014 Multi-Tech Systems
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SPIFLASH25_H
#define SPIFLASH25_H

#include <mbed.h>

#define BLOCK_0     0x0   //Spacing  8192 byte
#define BLOCK_1     0x10000//Spacing  8192 byte
#define BLOCK_2     0x20000//Spacing  8192 byte
#define BLOCK_3     0x30000//Spacing  8192 byte
#define BLOCK_4     0x40000//Spacing  8192 byte
#define BLOCK_5     0x50000//Spacing  8192 byte
#define BLOCK_6     0x60000//Spacing  8192 byte
#define BLOCK_7     0x70000//Spacing  8192 byte
#define BLOCK_8     0x80000//Spacing  8192 byte
#define BLOCK_9     0x90000//Spacing  8192 byte
#define BLOCK_10    0xA0000//Spacing  8192 byte
#define BLOCK_11    0xB0000//Spacing  8192 byte
#define BLOCK_12    0xC0000//Spacing  8192 byte
#define BLOCK_13    0xD0000//Spacing  8192 byte
#define BLOCK_14    0xE0000//Spacing  8192 byte
#define BLOCK_15    0xF0000//Spacing  8192 byte



class SpiFlash25 {
    public:
        SpiFlash25(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName W = NC, PinName HOLD = NC, int page_size = 256);

        /* Set the page size (default 256) */
        void set_page_size(int size);

        /* Set up the internal SPI object */
        void format(int bits, int mode);
        void frequency(int hz);

        /* Reads and writes can be across page boundaries */
        bool read(int addr, int len, char* data);
        bool write(int addr, int len, const char* data);

        /* Read ID and status registers */
        char* read_id();
        char read_status();

        /* Erase methods */
        void clear_sector(int addr);
        void clear_mem();

        void deep_power_down();
        void wakeup();
        int read_memsize();

    private:
        enum {
            WRITE_ENABLE                = 0x06,
            WRITE_DISABLE               = 0x04,
            READ_IDENTIFICATION         = 0x9F,
            READ_STATUS                 = 0x05,
            WRITE_STATUS                = 0x01,
            READ_DATA                   = 0x03,
            READ_DATA_FAST              = 0x0B,
            PAGE_PROGRAM                = 0x02,
            SECTOR_ERASE                = 0xD8,
            BULK_ERASE                  = 0xC7,
            DEEP_POWER_DOWN             = 0xB9,
            DEEP_POWER_DOWN_RELEASE     = 0xAB,
        };

        enum {
            STATUS_SRWD                 = 0x80,     // 0b 1000 0000
            STATUS_BP2                  = 0x10,     // 0b 0001 0000
            STATUS_BP1                  = 0x08,     // 0b 0000 1000
            STATUS_BP0                  = 0x04,     // 0b 0000 0100
            STATUS_WEL                  = 0x02,     // 0b 0000 0010
            STATUS_WIP                  = 0x01,     // 0b 0000 0001
        };

        enum {
            ID_MANUFACTURER             = 0,
            ID_MEM_TYPE                 = 1,
            ID_MEM_SIZE                 = 2,
        };

        bool write_page(int addr, int len, const char* data);
        void enable_write();
        void wait_for_write();

        static inline char high_byte(int addr) {
            return ((addr & 0xff0000) >> 16);
        }
        static inline char mid_byte(int addr) {
            return ((addr & 0xff00) >> 8);
        }
        static inline char low_byte(int addr) {
            return (addr & 0xff);
        }

        SPI _spi;
        DigitalOut _cs;
        DigitalOut* _w;
        DigitalOut* _hold;
        int _mem_size;
        int _page_size;
        char _id[3];
};
#endif
