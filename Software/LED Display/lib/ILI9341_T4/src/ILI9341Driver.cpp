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


#include "ILI9341Driver.h"
#include "font_ILI9341_T4.h"

#include <SPI.h>



namespace ILI9341_T4
{


    /**********************************************************************************************************
    * Initialization and general settings
    ***********************************************************************************************************/


    FLASHMEM ILI9341Driver::ILI9341Driver(uint8_t cs, uint8_t dc, uint8_t sclk, uint8_t mosi, uint8_t miso, uint8_t rst, uint8_t touch_cs, uint8_t touch_irq)
        {
        // general
        _width = ILI9341_T4_TFTWIDTH;
        _height = ILI9341_T4_TFTHEIGHT;
        _rotation = 0;
        _refreshmode = 0; 
        _outputStream = nullptr;

        // buffering
        _late_start_ratio = ILI9341_T4_DEFAULT_LATE_START_RATIO;
        _late_start_ratio_override = true;
        _diff_gap = ILI9341_T4_DEFAULT_DIFF_GAP;
        _vsync_spacing = ILI9341_T4_DEFAULT_VSYNC_SPACING;
        _diff1 = nullptr;
        _diff2 = nullptr;
        _fb1 = nullptr;
        _dummydiff1 = &_dd1;
        _dummydiff2 = &_dd2;
        _mirrorfb = nullptr;
        _ongoingDiff = nullptr;

        _compare_mask = 0; 

        // vsync
        _period = 0;        
        _synced_em = 0;
        _synced_scanline = 0;

        // dma
        _fb = nullptr;
        _diff = nullptr;
        _dma_state = ILI9341_T4_DMA_IDLE;
        _last_delta = 0;         
        _timeframestart = 0;
        _last_y = 0;

        // spi
        _cs = cs;
        _dc = dc;
        _sclk = sclk;
        _mosi = mosi;
        _miso = miso;
        _rst = rst;
        _touch_cs = touch_cs;
        _touch_irq = touch_irq;
        _cspinmask = 0;
        _csport = NULL;
        _hardware_dc = false;

        _setTouchInterrupt();
        _timerinit();

        statsReset();
        }





    FLASHMEM bool ILI9341Driver::begin(uint32_t spi_clock, uint32_t spi_clock_read)
        {
        static const uint8_t init_commands[] = {
                                                 4, 0xEF, 0x03, 0x80, 0x02,                 // undocumented commands            
                                                 4, 0xCF, 0x00, 0xC1, 0x30,                 //
                                                 5, 0xED, 0x64, 0x03, 0X12, 0X81,           //
                                                 4, 0xE8, 0x85, 0x00, 0x78,                 //
                                                 6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,     //
                                                 2, 0xF7, 0x20,                             //
                                                 3, 0xEA, 0x00, 0x00,                       //
                                                 2, ILI9341_T4_PWCTR1, 0x23, // Power control 0x23 (or 0x20)
                                                 2, ILI9341_T4_PWCTR2, 0x10, // Power control
                                                 3, ILI9341_T4_VMCTR1, 0x3e, 0x28, // VCM control
                                                 2, ILI9341_T4_VMCTR2, 0x86, // VCM control2
                                                 2, ILI9341_T4_MADCTL, 0x48, // Memory Access Control
                                                 2, ILI9341_T4_PIXFMT, 0x55, 
                                                 3, ILI9341_T4_FRMCTR1, 0x00, 0x13, 
                                                 4, ILI9341_T4_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
                                                 2, 0xF2, 0x00, // Gamma Function Disable
                                                 2, ILI9341_T4_GAMMASET, 0x01, // Gamma curve selected
                                                16, ILI9341_T4_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
                                                16, ILI9341_T4_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma                                                                                             
                                                 5, 0x2B, 0x00, 0x00, 0x01, 0x3f,
                                                 5, 0x2A, 0x00, 0x00, 0x00, 0xef,
                                                 0 };

        _print("\n\n----------------- ILI9341_T4 begin() ------------------\n\n");
        statsReset();
        resync(); // resync at first upload
        _mirrorfb = nullptr; // force full redraw.
        _ongoingDiff = nullptr;

        if (_touch_cs != 255)
            { // set touch CS high to prevent interference.
            digitalWrite(_touch_cs, HIGH);
            pinMode(_touch_cs, OUTPUT);
            digitalWrite(_touch_cs, HIGH);
            }
        
        if (_cs != 255)
            { // set screen CS high also. 
            digitalWrite(_cs, HIGH);
            pinMode(_cs, OUTPUT);
            digitalWrite(_cs, HIGH);
            }
        

        // verify SPI pins are valid
        int spinum_MOSI = -1; 
        if (SPI.pinIsMOSI(_mosi)) spinum_MOSI = 0;  else if (SPI1.pinIsMOSI(_mosi)) spinum_MOSI = 1; else if (SPI2.pinIsMOSI(_mosi)) spinum_MOSI = 2;
        if (spinum_MOSI < 0)
            {
            _printf("\n*** ERROR: MOSI on pin %d is not a valid SPI pin ! ***\n\n", _mosi);
            return false;
            }
        else _printf("- MOSI on pin %d [SPI%d]\n", _mosi, spinum_MOSI);

        int spinum_MISO = -1; 
        if (SPI.pinIsMISO(_miso)) spinum_MISO = 0;  else if (SPI1.pinIsMISO(_miso)) spinum_MISO = 1; else if (SPI2.pinIsMISO(_miso)) spinum_MISO = 2;
        if (spinum_MISO < 0)
            {
            _printf("\n*** ERROR: MISO on pin %d is not a valid SPI pin ! ***\n\n", _miso);
            return false;
            }
        else _printf("- MISO on pin %d [SPI%d]\n", _miso, spinum_MISO);

        int spinum_SCK = -1; 
        if (SPI.pinIsSCK(_sclk)) spinum_SCK = 0;  else if (SPI1.pinIsSCK(_sclk)) spinum_SCK = 1; else if (SPI2.pinIsSCK(_sclk)) spinum_SCK = 2;
        if (spinum_SCK < 0)
            {
            _printf("\n*** ERROR: SCK on pin %d is not a valid SPI pin ! ***\n\n", _sclk);
            return false;
            }
        else _printf("- SCK on pin %d [SPI%d]\n", _sclk, spinum_SCK);

        if ((spinum_SCK != spinum_MISO) || (spinum_SCK != spinum_MOSI))
            {
            _printf("\n*** ERROR: SCK, MISO and MOSI must be on the same SPI bus ! ***\n\n", _sclk);
            return false;
            }

        if (spinum_SCK == 0)
            {
            _pspi = &SPI;
            _spi_num = 0; // Which buss is this spi on?
            _pimxrt_spi = &IMXRT_LPSPI4_S; // Could hack our way to grab this from SPI object, but...        
            }
        else if (spinum_SCK == 1)
            {
            _pspi = &SPI1;
            _spi_num = 1; // Which buss is this spi on?
            _pimxrt_spi = &IMXRT_LPSPI3_S; // Could hack our way to grab this from SPI object, but...
            }
        else 
            {
            _pspi = &SPI2;
            _spi_num = 2; // Which buss is this spi on?
            _pimxrt_spi = &IMXRT_LPSPI1_S; // Could hack our way to grab this from SPI object, but...
            }

        // Make sure we have all of the proper SPI pins selected.
        _pspi->setMOSI(_mosi);
        _pspi->setSCK(_sclk);
        _pspi->setMISO(_miso);

        // Hack to get hold of the SPI Hardware information...
        uint32_t* pa = (uint32_t*)((void*)_pspi);
        _spi_hardware = (SPIClass::SPI_Hardware_t*)(void*)pa[1];
        _pspi->begin();

        _pending_rx_count = 0; // Make sure it is zero if we we do a second begin...

        // CS pin direct access via port.
        if (_cs < 255)
            {
            _printf("- CS on pin %d\n", _cs);
            _csport = portOutputRegister(_cs);
            _cspinmask = digitalPinToBitMask(_cs);
            pinMode(_cs, OUTPUT);
            _directWriteHigh(_csport, _cspinmask);
            }
        else
            {
            _print("- CS not connected to the Teensy (make sure CS is pulled down on the ILI9341 side...)\n");
            _csport = NULL; 
            }

        _spi_tcr_current = _pimxrt_spi->TCR; // get the current TCR value

        _hardware_dc = _pspi->pinIsChipSelect(_dc);

        if (_hardware_dc)
            {
            if (!_pspi->pinIsChipSelect(_dc))
                {
                _printf("\n*** ERROR: DC (here on pin %d) is not a valid cs pin for SPI%d ***\n\n", _dc, _spi_num);
                return false; // ERROR, DC is not a hardware CS pin for the SPI bus. 
                }
            _printf("- DC on pin %d [SPI%d] Hardware accelerated !\n", _dc, _spi_num);            
            uint8_t dc_cs_index = _pspi->setCS(_dc);
            dc_cs_index--; // convert to 0 based
            _tcr_dc_assert = LPSPI_TCR_PCS(dc_cs_index);              
            _tcr_dc_not_assert = LPSPI_TCR_PCS(3);            
            }
        else
            {                                 
            _printf("- DC on pin %d  >>>> *** No hardware acceleration : put DC on a hardware chip select pin for speedup *** \n", _dc, _spi_num);
            _tcr_dc_assert = LPSPI_TCR_PCS(1);
            _tcr_dc_not_assert = LPSPI_TCR_PCS(3);                                  
            _dcport = portOutputRegister(_dc);
            _dcpinmask = digitalPinToBitMask(_dc);
            pinMode(_dc, OUTPUT);
            _directWriteHigh(_dcport, _dcpinmask);
            }           

        // attach interrupt            
        switch (_spi_num)
            {
            case 0:
                attachInterruptVector(IRQ_LPSPI4, (_hardware_dc) ? _spiInterruptSPI0_hardware : _spiInterruptSPI0_software);
                NVIC_SET_PRIORITY(IRQ_LPSPI4, ILI9341_T4_IRQ_PRIORITY);
                NVIC_ENABLE_IRQ(IRQ_LPSPI4);
                break;
            case 1:
                attachInterruptVector(IRQ_LPSPI3, (_hardware_dc) ? _spiInterruptSPI1_hardware : _spiInterruptSPI1_software);
                NVIC_SET_PRIORITY(IRQ_LPSPI3, ILI9341_T4_IRQ_PRIORITY);
                NVIC_ENABLE_IRQ(IRQ_LPSPI3);
                break;
            case 2:
                attachInterruptVector(IRQ_LPSPI1, (_hardware_dc) ? _spiInterruptSPI2_hardware : _spiInterruptSPI2_software);
                NVIC_SET_PRIORITY(IRQ_LPSPI1, ILI9341_T4_IRQ_PRIORITY);
                NVIC_ENABLE_IRQ(IRQ_LPSPI1);
                break;
            }


        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(7)); // drive DC high now. 

        if (_rst < 255) _printf("- RST on pin %d\n", _rst); else _print("- RST pin not connected (set it to +3.3V).\n");
        if (_touch_cs < 255)
            {
            _printf("\n[Touchscreen is CONNECTED]\n- TOUCH_CS on pin %d\n", _touch_cs);
            if (_touch_irq < 255) _printf("- TOUCH_IRQ on pin %d\n", _touch_irq); else _print("- TOUCH_IRQ not connected\n");
            }
        else
            {
            _print("\n[Touchscreen NOT connected]\n");
            }

        _spi_clock = spi_clock;
        if (_spi_clock < 0) _spi_clock = ILI9341_T4_DEFAULT_SPICLOCK;
        _spi_clock_read = spi_clock_read;
        if (_spi_clock_read < 0) _spi_clock_read = ILI9341_T4_DEFAULT_SPICLOCK_READ;
        _printf("\n- SPI write speed : %.2fMhz\n", spi_clock / 1000000.0f);
        _printf("- SPI read speed : %.2fMhz\n\n", spi_clock_read / 1000000.0f);


        _rotation = 0; // default rotation

        int r = ILI9341_T4_RETRY_INIT;
        while(1)
            { // sometimes, init may fail because of instable power supply. Retry in this case.         
            if (_rst < 255)
                { // Reset the screen
                pinMode(_rst, OUTPUT);
                digitalWrite(_rst, HIGH);
                delay(10);
                digitalWrite(_rst, LOW);
                delay(20);
                digitalWrite(_rst, HIGH);
                }
            else
                {
                _beginSPITransaction(_spi_clock / 4); // quarter speed for setup !              
                for(int i=0; i<5; i++) _writecommand_cont(ILI9341_T4_NOP); // send NOPs
                _writecommand_last(ILI9341_T4_SWRESET); // issue a software reset
                _endSPITransaction();                   
                }
            delay(150); // mandatory !
           
            _beginSPITransaction(_spi_clock / 4); // quarter speed for setup ! 
            const uint8_t* addr = init_commands;
            while (1)
                {
                uint8_t count = *addr++;
                if (count-- == 0) break;
                _writecommand_cont(*addr++);
                while (count-- > 0) { _writedata8_cont(*addr++); }
                }
            _writecommand_last(ILI9341_T4_SLPOUT); // Exit Sleep            
            _endSPITransaction();

            delay(150); // must wait for the screen to exit sleep mode. 
            _beginSPITransaction(_spi_clock / 4); // quarter speed for setup ! 
            _writecommand_last(ILI9341_T4_DISPON); // Display on
            _endSPITransaction();
                     
            // if everything is ok, we should have:
            // - Display Power Mode = 0x9C
            // - Pixel Format = 0x5
            // - Image Format = 0x0
            // - Self Diagnostic = 0xC0 
            int res_RDMODE = _readcommand8(ILI9341_T4_RDMODE);
            int res_RDPIXFMT = _readcommand8(ILI9341_T4_RDPIXFMT);
            int res_RDIMGFMT = _readcommand8(ILI9341_T4_RDIMGFMT);
            int res_RDSELFDIAG = _readcommand8(ILI9341_T4_RDSELFDIAG);
            _print("\nReading status registers...\n");
            _print("  - Display Power Mode : 0x"); _println(res_RDMODE, HEX);
            _print("  - Pixel Format       : 0x"); _println(res_RDPIXFMT, HEX);
            _print("  - Image Format       : 0x"); _println(res_RDIMGFMT, HEX);
            _print("  - Self Diagnostic    : 0x"); _println(res_RDSELFDIAG, HEX);

            bool ok = true;
            if ((res_RDMODE == 0) && (res_RDPIXFMT == 0) && (res_RDIMGFMT == 0) && (res_RDSELFDIAG == 0))
                {
                _print("\n*** ERROR: Cannot read screen registers. Check the MISO line or decrease SPI read speed ***\n\n");
                ok = false;
                }
            else
                {
                if (res_RDMODE != 0x9C)
                    { // wrong power display mode
                    _print("\n*** ERROR: incorrect power mode ! ***\n\n");
                    ok = false;
                    }
                if (res_RDPIXFMT != 0x5)
                    { // wrong pixel format
                    _print("\n*** ERROR: incorrect pixel format ! ***\n\n");
                    ok = false;
                    }
                if (res_RDIMGFMT != 0x0)
                    { // wrong image format
                    _print("\n*** ERROR: incorrect image format ! ***\n\n");
                    ok = false;
                    }
                if (res_RDSELFDIAG != ILI9341_T4_SELFDIAG_OK)
                    { // wrong self diagnotic value
                    _print("\n*** ERROR: incorrect self-diagnotic value ! ***\n\n");
                    ok = false;
                    }
                }
            if (ok)
                {
                // all good, ready to warp pixels :-)
                // ok, we can talk to the display so we set the (max) refresh rate to read its exact values
                setRefreshMode(0);
                _period_mode0 = _period; // save the period for fastest mode. 
                _print("\nOK. Screen initialization successful !\n\n");
                return true;
                }
            // error
            if (--r <= 0)
                {
                _print("\n*** CANNOT CONNECT TO ILI9341 SCREEN. ABORTING... ***\n\n");
                }
            if (_spi_clock_read > 100000)_spi_clock_read /= 2;
            _printf("Retrying connexion with slower SPI read speed : %.2fMhz", _spi_clock_read / 1000000.0f);
            delay(1000);
            }
        }


    FLASHMEM int ILI9341Driver::selfDiagStatus()
        {
        _waitUpdateAsyncComplete();
        resync();
        return _readcommand8(ILI9341_T4_RDSELFDIAG);
        }


    FLASHMEM void ILI9341Driver::printStatus()
        {
        _waitUpdateAsyncComplete();
        _print("---------------- ILI9341Driver Status-----------------\n");
        uint8_t x = _readcommand8(ILI9341_T4_RDMODE);
        _print("- Display Power Mode  : 0x"); _println(x, HEX);
        x = _readcommand8(ILI9341_T4_RDMADCTL);
        _print("- MADCTL Mode         : 0x"); _println(x, HEX);
        x = _readcommand8(ILI9341_T4_RDPIXFMT);
        _print("- Pixel Format        : 0x"); _println(x, HEX);
        x = _readcommand8(ILI9341_T4_RDIMGFMT);
        _print("- Image Format        : 0x"); _println(x, HEX);
        x = _readcommand8(ILI9341_T4_RDSGNMODE);
        _print("- Display Signal Mode : 0x"); _println(x, HEX);       
        x = _readcommand8(ILI9341_T4_RDSELFDIAG); 
        _print("- Self Diagnostic     : 0x"); _print(x, HEX);
        if (x == ILI9341_T4_SELFDIAG_OK) _println(" [OK].\n"); else _println(" [ERROR].\n");
        resync();
        }




    FLASHMEM void ILI9341Driver::setSpiClock(int spi_clock)
        {
        _waitUpdateAsyncComplete();
        _spi_clock = spi_clock;
        statsReset();
        resync();
        };


    FLASHMEM void ILI9341Driver::setSpiClockRead(int spi_clock)
        {
        _waitUpdateAsyncComplete();
        _spi_clock_read = spi_clock;
        statsReset();
        resync();
        }



    /**********************************************************************************************************
    * Misc. commands.
    ***********************************************************************************************************/



    FLASHMEM void ILI9341Driver::sleep(bool enable)
        {
        _waitUpdateAsyncComplete();

        _mirrorfb = nullptr; // force full redraw.
        _ongoingDiff = nullptr;

        _beginSPITransaction(_spi_clock / 4); // quarter speed
        if (enable)
            {
            _writecommand_cont(ILI9341_T4_DISPOFF);
            _writecommand_last(ILI9341_T4_SLPIN);
            _endSPITransaction();
            delay(200);
            }
        else
            {
            _writecommand_cont(ILI9341_T4_DISPON);
            _writecommand_last(ILI9341_T4_SLPOUT);
            _endSPITransaction();
            delay(20);
            }
        resync();
        }



    FLASHMEM void ILI9341Driver::invertDisplay(bool i)
        {
        _waitUpdateAsyncComplete();
        _beginSPITransaction(_spi_clock / 4); // quarter speed
        _writecommand_last(i ? ILI9341_T4_INVON : ILI9341_T4_INVOFF);
        _endSPITransaction();
        resync();
        }


    FLASHMEM void ILI9341Driver::setScroll(int offset)
        {
        if (offset < 0)
            {
            offset += (((-offset) / ILI9341_T4_TFTHEIGHT) + 1) * ILI9341_T4_TFTHEIGHT;
            }
        offset = offset % 320;
        _waitUpdateAsyncComplete();
        _beginSPITransaction(_spi_clock);
        _writecommand_cont(ILI9341_T4_VSCRSADD);
        _writedata16_cont(offset);
        _writecommand_cont(ILI9341_T4_RAMWR); // must send RAMWR because two consecutive VSCRSADD command may stall 
        _writecommand_last(ILI9341_T4_NOP);
        _endSPITransaction();
        }



    /**********************************************************************************************************
    * Screen orientation
    ***********************************************************************************************************/


    FLASHMEM void ILI9341Driver::setRotation(uint8_t m)
        {
        m = _clip(m, (uint8_t)0, (uint8_t)3);
        if (m == _rotation) return;
        _waitUpdateAsyncComplete();
        _mirrorfb = nullptr; // force full redraw.
        _ongoingDiff = nullptr;

        statsReset();
        _rotation = m;
        switch (m)
            {
            case 0: // portrait 240x320
            case 2: // portrait 240x320
                _width = ILI9341_T4_TFTWIDTH;
                _height = ILI9341_T4_TFTHEIGHT;
                break;
            case 1: // landscape 320x240
            case 3: // landscape 320x240
                _width = ILI9341_T4_TFTHEIGHT;
                _height = ILI9341_T4_TFTWIDTH;
                break;
            }
        resync();
        }




    /**********************************************************************************************************
    * About timing and vsync.
    ***********************************************************************************************************/


    FLASHMEM void ILI9341Driver::setRefreshMode(int mode)
        {
        if ((mode < 0) || (mode > 31)) return; // invalid mode, do nothing. 
        _refreshmode = mode;
        uint8_t diva = 0;
        if (mode >= 16) 
            {
            mode -= 16; 
            diva = 1;
            }
        _waitUpdateAsyncComplete();
        _beginSPITransaction(_spi_clock / 4); // quarter speed 
        _writecommand_cont(ILI9341_T4_FRMCTR1); // Column addr set
        _writedata8_cont(diva);
        _writedata8_last(0x10 + mode);
        _endSPITransaction();
        delayMicroseconds(50); 
        _sampleRefreshRate(); // estimate the real refreshrate
        statsReset();
        resync();
        }


    FLASHMEM void ILI9341Driver::setRefreshRate(float refreshrate_hz)
        {
        const int m = _modeForRefreshRate(refreshrate_hz);
        setRefreshMode(m);
        resync();
        }


    FLASHMEM void ILI9341Driver::printRefreshMode()
        {
        const int om = getRefreshMode();
        _print("------------ ILI9341Driver Refresh Modes -------------\n");
        for(int m = 0; m <= 31; m++)
            {
            setRefreshMode(m);
            float r = getRefreshRate();
            _printf("- mode %u : %fHz (%u FPS with vsync_spacing = 2).\n", m, r, (uint32_t)round(r/2));
            }
        _println("");
        setRefreshMode(om);
        }


    /** return the current scanline in [0, 319]. Sync with SPI only if required */
    int ILI9341Driver::_getScanLine(bool sync)
        {
        if (!sync)
            {
            return ( _synced_scanline + ((((uint64_t)_synced_em)* ILI9341_T4_NB_SCANLINES) / _period) ) % ILI9341_T4_NB_SCANLINES;
            }
    
        _beginSPITransaction(_spi_clock_read);

        _maybeUpdateTCR(_tcr_dc_assert | LPSPI_TCR_FRAMESZ(7));
        _pimxrt_spi->TDR = 0x45; // send command
        _pimxrt_spi->TCR = _spi_tcr_current;
        while ((_pimxrt_spi->FSR & 0x1f)); // make sure command has been sent
        delayMicroseconds(3); // wait as requeted per manual

        uint32_t val = 0; 

        while ((_pimxrt_spi->RSR & LPSPI_RSR_RXEMPTY) != 0);
        val = _pimxrt_spi->RDR; // Read pending RX bytes

        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(7));
        _pimxrt_spi->TDR = 0;

        while ((_pimxrt_spi->RSR & LPSPI_RSR_RXEMPTY) != 0);
        val = _pimxrt_spi->RDR; // Read pending RX bytes
       
        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(8)); // AHAH, the value is 9 bits ! lol...
        _pimxrt_spi->TDR = 0;

        while ((_pimxrt_spi->RSR & LPSPI_RSR_RXEMPTY) != 0);
        val = _pimxrt_spi->RDR; // Read pending RX bytes

        _synced_em = 0;
        _synced_scanline = (val > 319) ? 0 : val;  // save the scanline

        _endSPITransaction();
        return _synced_scanline;
        }


    void ILI9341Driver::_sampleRefreshRate()
        {
        const int NB_SAMPLE_FRAMES = 10;
        elapsedMicros em = 0; // start counter 
        // wait to reach scanline 0
        while (_getScanLine(true) != 0)
            {
            if (em > 1000000)
                { // hanging...
                em = 0;
                _println("\n\nILI9341_T4 : Hanging inside _sampleRefreshRate(), cannot get to scan line 0...");
                }
            }
        while (_getScanLine(true) == 0)
            {
            if (em > 1000000)
                { // hanging...
                em = 0;
                _println("\n\nILI9341_T4 : Hanging inside _sampleRefreshRate() cannot get to scan line 1...");
                }
            } 
        // ok, just start at scanline 1. 
        em = 0;
        for (int i = 0; i < NB_SAMPLE_FRAMES; i++)
            {
            delayMicroseconds(5000); // must be less than 200 FPS so wait at least 5ms
            // wait to reach scanline 0
            while (_getScanLine(true) != 0)
                {
                if (em > 1000000)
                    { // hanging...
                    em = 0;
                    _println("\n\nILI9341_T4 : Hanging inside _sampleRefreshRate(), cannot get to scan line 0 (bis)...");
                    }
                }
            // wait to begin scanline 1. 
            while (_getScanLine(true) == 0);  
                {
                if (em > 1000000)
                    { // hanging...
                    em = 0;
                    _println("\n\nILI9341_T4 : Hanging inside _sampleRefreshRate(), cannot get to scan line 0 (bis)...");
                    }
                }
            }
        _period = (uint32_t)round(((float)em) / NB_SAMPLE_FRAMES);
        }


    float ILI9341Driver::_refreshRateForMode(int mode) const
        { 
        float freq = 1000000.0f / _period_mode0;
        if (mode >= 16)
            {
            freq /= 2.0f; 
            mode -= 16;
            }
        return (freq*16.0f)/(16.0f + mode);
        }


    int ILI9341Driver::_modeForRefreshRate(float hz) const
        {
        if (hz <= _refreshRateForMode(31)) return 31;
        if (hz >= _refreshRateForMode(0)) return 0;
        int a = 0;
        int b = 31;
        while (b - a > 1)
            { // dichotomy. 
            int c = (a + b) / 2;
            ((hz < _refreshRateForMode(c)) ? a : b) = c;
            }
        float da = _refreshRateForMode(a) - hz;
        float db = hz - _refreshRateForMode(b);
        return (da < db ? a : b);
        }



    /**********************************************************************************************************
    * VSync spacing
    ***********************************************************************************************************/


    FLASHMEM void ILI9341Driver::setVSyncSpacing(int vsync_spacing)
        {
        _waitUpdateAsyncComplete();
        _vsync_spacing = ILI9341Driver::_clip<int>((int)vsync_spacing, (int)-1, (int)ILI9341_T4_MAX_VSYNC_SPACING);
        statsReset();
        resync();
        }


    FLASHMEM void ILI9341Driver::setLateStartRatio(float ratio)
        {
        _waitUpdateAsyncComplete(); // no need to wait for sync. 
        _late_start_ratio = ILI9341Driver::_clip<float>(ratio, 0.1f, 0.9f);
        statsReset();
        resync();
        }


    /**********************************************************************************************************
    * Buffering mode
    ***********************************************************************************************************/



    FLASHMEM void ILI9341Driver::setFramebuffer(uint16_t* fb1)
        {
        _waitUpdateAsyncComplete();
        _mirrorfb = nullptr; // complete redraw needed.
        _ongoingDiff = nullptr;
        _fb1 = fb1;        
        if (_fb1) memset(_fb1, 0, ILI9341_T4_NB_PIXELS*2);   
        resync();
        }




    /**********************************************************************************************************
    * Differential updates
    ***********************************************************************************************************/


    FLASHMEM void ILI9341Driver::setDiffBuffers(DiffBuffBase* diff1, DiffBuffBase* diff2)
        {
        _waitUpdateAsyncComplete();
        if (diff1)
            {
            _diff1 = diff1;
            _diff2 = diff2;
            }
        else
            {
            _diff1 = diff2;
            _diff2 = diff1;
            }
        }


    FLASHMEM void ILI9341Driver::setDiffGap(int gap)
        {
        _waitUpdateAsyncComplete();
        _diff_gap = ILI9341Driver::_clip<int>((int)gap, (int)2, (int)ILI9341_T4_NB_PIXELS);
        statsReset();
        resync();
        }


    FLASHMEM void ILI9341Driver::setDiffCompareMask(uint16_t mask)
        {
        if (mask == 65535) mask = 0;
        _compare_mask = mask;
        }


    FLASHMEM void ILI9341Driver::setDiffCompareMask(int bitskip_red, int bitskip_green, int bitskip_blue)
        {
        _compare_mask = (((uint16_t)(((0xFF >> bitskip_red) << bitskip_red) & 31)) << 11)
                      | (((uint16_t)(((0xFF >> bitskip_green) << bitskip_green) & 63)) << 5)
                      | ((uint16_t)(((0xFF >> bitskip_blue) << bitskip_blue) & 31));
        if (_compare_mask == 65535) _compare_mask = 0; 
        }


    /**********************************************************************************************************
    * Update
    ***********************************************************************************************************/


    FLASHMEM void ILI9341Driver::clear(uint16_t color)
        {
        _waitUpdateAsyncComplete();    
        _beginSPITransaction(_spi_clock);
        _writecommand_cont(ILI9341_T4_PASET);
        _writedata16_cont(0);
        _writedata16_cont(ILI9341_T4_TFTHEIGHT-1);
        _writecommand_cont(ILI9341_T4_CASET);
        _writedata16_cont(0);
        _writedata16_cont(ILI9341_T4_TFTWIDTH-1);
        _writecommand_cont(ILI9341_T4_RAMWR);           
        for(int i=0; i < ILI9341_T4_NB_PIXELS; i++) _writedata16_cont(color);           
        _writecommand_last(ILI9341_T4_NOP); 
        _endSPITransaction();            
        if (_fb1)
            {
            for(int i=0; i < ILI9341_T4_NB_PIXELS; i++) _fb1[i] = color;
            _mirrorfb = _fb1;
            _ongoingDiff = nullptr;
            }
        resync();
        }


    FLASHMEM void ILI9341Driver::updateRegion(bool redrawNow, const uint16_t* fb, int xmin, int xmax, int ymin, int ymax, int stride)
        {
        if (fb == nullptr) return;
        if (stride < 0) stride = xmax - xmin + 1;
        switch (bufferingMode())
            {
            case NO_BUFFERING:
                // the only thing we can do is to push the sub-frame right away. 
                // without DMA and without DIFF so we just upload the rectangle. 
                // TODO : add vsync ? 
                _mirrorfb = nullptr; 
                _ongoingDiff = nullptr;
                _updateRectNow(fb, xmin, xmax, ymin, ymax, stride);
                return;

            default:
                // DOUBLE_BUFFERING                 
                if (_diff2 == nullptr) // differential updates only with 2 diffs buffers.  
                    { // NO DIFFERENTIAL UPDATES: copy into the framebuffer and update the screen if required                    
                    _ongoingDiff = nullptr;    // no diff
                    _waitUpdateAsyncComplete(); // wait if there is still an update in progress                                 
                    _dummydiff1->computeDiff(_fb1, nullptr, fb, xmin, xmax, ymin, ymax, stride, _rotation, _diff_gap, true, _compare_mask); // create a diff and copy to fb1.
                    if (redrawNow) 
                        {                         
                        if (_mirrorfb)
                            { // _fb1 mirrors the screen so we just need to draw the region 
                            _updateRectNow(fb, xmin, xmax, ymin, ymax, stride); // note that we can use the method with fb and not _fb1. ***** TODO: replace by an async draw
                            }
                        else
                            { // redraw everything, via DMA
                            _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                            _updateAsync(_fb1, _dummydiff1); // launch update
                            }
                        _mirrorfb = _fb1;                         
                        }
                    else
                        {
                        _mirrorfb = nullptr;                         
                        }
                    return;
                    }

                // we have 2 diff buffers and a framebuffer.                 
                if (_mirrorfb)
                    { // the framebuffer mirrors the screen                
                    if (asyncUpdateActive())
                        {
                        _diff2->computeDiff(_fb1, nullptr, fb, xmin, xmax, ymin, ymax, stride, _rotation, _diff_gap, false, _compare_mask); // create diff while async update
                        _waitUpdateAsyncComplete();
                        DiffBuffBase::copyfb(_fb1, fb, xmin, xmax, ymin, ymax, stride, _rotation); // copy to fb1
                        }
                    else
                        {
                        _diff2->computeDiff(_fb1, nullptr, fb, xmin, xmax, ymin, ymax, stride, _rotation, _diff_gap, true, _compare_mask); // create a diff and copy to fb1.
                        }
                    _swapdiff();
                    if (redrawNow)
                        {
                        _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                        _updateAsync(_fb1, _diff1);
                        _mirrorfb = _fb1;
                        _ongoingDiff = nullptr;
                        }
                    else
                        {
                        _mirrorfb = nullptr; 
                        _ongoingDiff = _diff1;
                        }
                    return;
                    }

                if (_ongoingDiff != nullptr)
                    { // we are "in advance" w.r.t the screen
                    if (asyncUpdateActive())
                        {
                        _diff2->computeDiff(_fb1, _diff1, fb, xmin, xmax, ymin, ymax, stride, _rotation, _diff_gap, false, _compare_mask); // create diff while asyn update
                        _waitUpdateAsyncComplete();
                        DiffBuffBase::copyfb(_fb1, fb, xmin, xmax, ymin, ymax, stride, _rotation); // copy to fb1
                        }
                    else
                        {
                        _diff2->computeDiff(_fb1, _diff1, fb, xmin, xmax, ymin, ymax, stride, _rotation, _diff_gap, true, _compare_mask); // create a diff and copy to fb1.
                        }
                    _swapdiff();
                    if (redrawNow)
                        {
                        _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                        _updateAsync(_fb1, _diff1);
                        _mirrorfb = _fb1;
                        _ongoingDiff = nullptr;
                        }
                    else
                        {
                        _mirrorfb = nullptr; 
                        _ongoingDiff = _diff1;
                        }
                    return;
                    }

                // here, the framebuffer does not mirror the screen 
                _waitUpdateAsyncComplete();
                DiffBuffBase::copyfb(_fb1, fb, xmin, xmax, ymin, ymax, stride, _rotation); // copy the region into to fb1
                if (redrawNow)
                    { // redraw everything
                    _dummydiff1->computeDiff(_fb1, fb, _rotation, _diff_gap, false, _compare_mask); // create a dummy diff
                    _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                    _updateAsync(_fb1, _dummydiff1);
                    _mirrorfb = _fb1; // now we mirror the screen !
                    }
                return;
                
            }
        }
        


    FLASHMEM void ILI9341Driver::update(const uint16_t* fb, bool force_full_redraw)
        {
        if (fb == nullptr) return;

        if (fb == _fb1)
            { // ahah, someone is trying to cheat but I caught you !
            force_full_redraw = true; // a diff with itself would result in nothing to draw !
            }

        _ongoingDiff = nullptr; // here we just ignore possible ongoing diff and just redraw everything if _mirrorfb == nullptr. 
                                // We could do better but don't care since its an edge case relevant only when swapping between 
                                // methods updateRegion() and  update() which are not usually mixed. 

        switch (bufferingMode())
            {
            case NO_BUFFERING:
                {
                _waitUpdateAsyncComplete(); // wait until update is done (normally useless but still). 
                _mirrorfb = nullptr;
                _dummydiff1->computeDummyDiff(); 
                _updateNow(fb, _dummydiff1);
                return;
                }

            case DOUBLE_BUFFERING:
                {                
                if ((_vsync_spacing == -1) && (asyncUpdateActive())) { return; } // just drop the frame. 

                if ((_diff1 == nullptr)|| (_mirrorfb == nullptr) || (force_full_redraw))
                    { // do not use differential update
                    _waitUpdateAsyncComplete(); // wait until update is done. 
                    _dummydiff1->computeDiff(_fb1, fb, getRotation(), _diff_gap, true, _compare_mask); // create a dummy diff and copy to fb1. 
                    _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                    _updateAsync(_fb1, _dummydiff1); // launch update
                    _mirrorfb = _fb1; // set as mirror
                    return;
                    }

                if (_diff2 == nullptr)
                    { // double buffering with a single diff
                    _waitUpdateAsyncComplete(); // wait until update is done. 
                    if ((_mirrorfb == nullptr) || (force_full_redraw))
                        { // complete redraw needed. 
                        _dummydiff1->computeDiff(_fb1, fb, getRotation(), _diff_gap, true, _compare_mask); // create a dummy diff and copy to fb1. 
                        _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                        _updateAsync(_fb1, _dummydiff1); // launch update
                        }
                    else
                        { // diff redraw 
                        _diff1->computeDiff(_fb1, fb, getRotation(), _diff_gap, true, _compare_mask); // create a diff and copy to fb1. 
                        _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                        _updateAsync(_fb1, _diff1); // launch update
                        }
                    _mirrorfb = _fb1; // set as mirror
                    return;
                    }

                // double buffering with two diffs 
                if (asyncUpdateActive())
                    { // _diff2 is available so we use it to create the diff while update is in progress. 
                    _diff2->computeDiff(_fb1, fb, getRotation(), _diff_gap, false, _compare_mask); // create a diff without copying                    
                    _waitUpdateAsyncComplete(); // wait until update is done.                    
                    DiffBuff::copyfb(_fb1, fb, getRotation()); // save the framebuffer in fb1               
                    _swapdiff();  // swap the diffs so that diff1 contain the new diff.                     
                    _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                    _updateAsync(_fb1, _diff1); // launch update
                    }
                else
                    {
                    _diff1->computeDiff(_fb1, fb, getRotation(), _diff_gap, true, _compare_mask); // create a diff and copy
                    _flush_cache(_fb1, ILI9341_T4_NB_PIXELS * 2);
                    _updateAsync(_fb1, _diff1); // launch update
                    }
                _mirrorfb = _fb1; // set as mirror
                return;
                }
            }
        }



    void  ILI9341Driver::_pushpixels_mode0(const uint16_t* fb, int x, int y, int len)
        {
        const uint16_t* p = fb + x + (y * ILI9341_T4_TFTWIDTH);
        while (len-- > 0) { _writedata16_cont(*p++); }
        }


    void  ILI9341Driver::_pushpixels_mode1(const uint16_t* fb, int xx, int yy, int len)
        {
        int x = yy;
        int y = ILI9341_T4_TFTWIDTH - 1 - xx;
        while (len-- > 0)
            {
            _writedata16_cont(fb[x + ILI9341_T4_TFTHEIGHT*y]);
            y--;
            if (y < 0) { y = ILI9341_T4_TFTWIDTH - 1; x++; }
            }
        }


    void  ILI9341Driver::_pushpixels_mode2(const uint16_t* fb, int xx, int yy, int len)
        {
        int x = ILI9341_T4_TFTWIDTH - 1 - xx;
        int y = ILI9341_T4_TFTHEIGHT - 1 - yy;
        const uint16_t* p = fb + x + (y * ILI9341_T4_TFTWIDTH);
        while (len-- > 0) { _writedata16_cont(*p--); }
        }


    void  ILI9341Driver::_pushpixels_mode3(const uint16_t* fb, int xx, int yy, int len)
        {
        int x = ILI9341_T4_TFTHEIGHT - 1 - yy;
        int y = xx;    
        while (len-- > 0)
            {                     
            _writedata16_cont(fb[x + ILI9341_T4_TFTHEIGHT*y]);
            y++;
            if (y >= ILI9341_T4_TFTWIDTH) { y = 0; x--; }
            }
        }


    FLASHMEM void ILI9341Driver::_updateNow(const uint16_t* fb, DiffBuffBase* diff)
        {
        if ((fb == nullptr) || (diff == nullptr)) return;
        _waitUpdateAsyncComplete();
        _startframe(_vsync_spacing > 0);
        _margin = ILI9341_T4_NB_SCANLINES;
        _stats_nb_uploaded_pixels = 0;
        diff->initRead();
        int x = 0, y = 0, len = 0;
        int sc1 = diff->readDiff(x, y, len, 0); // scanline at 0 so sc1 will contain the scanline start position. 
        if (sc1 < 0)
            { // Diff is empty
            if (_vsync_spacing > 0)
                { // note the next time. 
                uint32_t t1 = micros() + _microToReachScanLine(0, true) - _vsync_spacing * _period;
                if (t1 > _timeframestart) _timeframestart = t1;
                _late_start_ratio_override = false;
                _last_delta = _vsync_spacing;
                }
            _endframe();
            return;
            }
        // ok we have at least one instruction
        if (_vsync_spacing > 0)
            {
            const uint32_t dd = (_timeframestart + ((_vsync_spacing - 1) * _period)) - micros();
            _pauseUploadTime();
            _delayMicro(dd); // wait until the previous frame is displayed the correct number of times. 
            _restartUploadTime();
            // we should now be around scanline 0 (or possibly late). 
            int sc2 = sc1 + ((ILI9341_T4_NB_SCANLINES - 1 - sc1) * _late_start_ratio); // maximum 'late' scanline
            uint32_t t2 = _microToReachScanLine(sc2, true); // with resync
            uint32_t t = _microToReachScanLine(sc1, false); // without resync            
            if (_late_start_ratio_override)
                { // force resync which is the same as asking _late_start_ratio=0;
                _late_start_ratio_override = false; // oneshot. 
                }
            else
                {
                if (t2 < t) t = 0;  // late, start right away. 
                }
            _pauseUploadTime();
            if (t > 0) delayMicroseconds(t);                                                   // wait if needed
            while ((t = _microToExitRange(0, sc1))) { delayMicroseconds(t); }                 // make sure we are good (in case delayMicroseconds() in not precise enough).
            _restartUploadTime();
            // ok, scanline is just after sc1 (if not late).
            _slinitpos = _getScanLine(false);   // save initial scanline position
            _em_async = 0;                      // start the counter
            const uint32_t tfs = micros() + _microToReachScanLine(0, false);        // time when this frame will start being displayed on the screen. 
            _last_delta = (int)round(((double)(tfs - _timeframestart)) / (_period));
            _timeframestart = tfs;
            }
        _beginSPITransaction(_spi_clock);
        // write full PASET/CASET now and we shall only update the start position from now on. 
        _writecommand_cont(ILI9341_T4_CASET);
        _writedata16_cont(x);
        _writedata16_cont(ILI9341_T4_TFTWIDTH);
        _writecommand_cont(ILI9341_T4_PASET);
        _writedata16_cont(y);
        _writedata16_last(ILI9341_T4_TFTHEIGHT);
        int prev_x = x;
        int prev_y = y;
        while (1)
            {
            int asl = (_vsync_spacing > 0) ? (_slinitpos + _nbScanlineDuring(_em_async)) : (2*ILI9341_T4_TFTHEIGHT);
            int r = diff->readDiff(x, y, len, asl);            
            if (r > 0)
                { // we must wait                             
                int t = _timeForScanlines(r - asl + 1);
                if (t < ILI9341_T4_MIN_WAIT_TIME) t = ILI9341_T4_MIN_WAIT_TIME;
                _pauseUploadTime();
                _delayMicro(t);
                _restartUploadTime();
                continue;
                }
            if (r < 0)
                { // finished                
                _writecommand_last(ILI9341_T4_NOP);
                _endSPITransaction();
                _endframe();
                return;
                }
            _stats_nb_uploaded_pixels += len;
            _stats_nb_transactions++;
            if (x != prev_x)
                {
                _writecommand_cont(ILI9341_T4_CASET);
                _writedata16_cont(x);
                prev_x = x;
                }
            if (y != prev_y)
                {
                _writecommand_cont(ILI9341_T4_PASET);
                _writedata16_cont(y);
                prev_y = y;
                }
            _writecommand_cont(ILI9341_T4_RAMWR);
            _pushpixels(fb, x, y, len);
            if (_vsync_spacing > 0)
                {
                int m = (ILI9341_T4_TFTWIDTH*y + x + len)/ILI9341_T4_TFTWIDTH + ILI9341_T4_TFTHEIGHT - _slinitpos - _nbScanlineDuring(_em_async);
                if (m < _margin) _margin = m;
                }           
            }
        }



        
    FLASHMEM void ILI9341Driver::_updateRectNow(const uint16_t* sub_fb, int xmin, int xmax, int ymin, int ymax, int stride)
        {
        int x1, x2, y1, y2;
        DiffBuffBase::rotationBox(_rotation, xmin, xmax, ymin, ymax, x1, x2, y1, y2);
        const int w = x2 - x1 + 1;

        if ((sub_fb == nullptr) || (x2 < x1) || (y2 < y1)) return;
        _waitUpdateAsyncComplete();
        _startframe(false);
        _stats_nb_uploaded_pixels = 0;

        _beginSPITransaction(_spi_clock);
        _writecommand_cont(ILI9341_T4_CASET);
        _writedata16_cont(x1);
        _writedata16_cont(x2);
        _writecommand_cont(ILI9341_T4_PASET);
        _writedata16_cont(y1);
        _writedata16_cont(y2);
        _writecommand_cont(ILI9341_T4_RAMWR);

        int mdelta = 0;
        switch (_rotation)
            {
            case PORTRAIT_240x320: mdelta = 1; break;
            case LANDSCAPE_320x240: mdelta = -stride; break;
            case PORTRAIT_240x320_FLIPPED: mdelta = -1; break;
            case LANDSCAPE_320x240_FLIPPED: mdelta = stride; break;
            }
        for (int yc = y1; yc <= y2; yc++)
            {
            int m = 0;
            switch (_rotation)
                {
                case PORTRAIT_240x320: m = stride * (yc - y1); break;
                case LANDSCAPE_320x240: m = (yc - y1) + stride * (x2 - x1); break;
                case PORTRAIT_240x320_FLIPPED: m = stride * (y2 - yc) + (x2 - x1); break;
                case LANDSCAPE_320x240_FLIPPED: m = y2 - yc; break;
                }   
            for (int n = 0; n < w; n++, m += mdelta) _writedata16_cont(sub_fb[m]);
            }
        _writecommand_last(ILI9341_T4_NOP);
        _endSPITransaction();
        _endframe();
        return;
        }
        

    FLASHMEM void ILI9341Driver::_pushRect(uint16_t color, int xmin, int xmax, int ymin, int ymax)
        {
        int x1, x2, y1, y2;
        DiffBuffBase::rotationBox(_rotation, xmin, xmax, ymin, ymax, x1, x2, y1, y2);
        if ((x2 < x1) || (y2 < y1)) return;
        const int l = (x2 - x1 + 1) * (y2 - y1 + 1);
        _waitUpdateAsyncComplete();
        _beginSPITransaction(_spi_clock);
        _writecommand_cont(ILI9341_T4_CASET);
        _writedata16_cont(x1);
        _writedata16_cont(x2);
        _writecommand_cont(ILI9341_T4_PASET);
        _writedata16_cont(y1);
        _writedata16_cont(y2);
        _writecommand_cont(ILI9341_T4_RAMWR);
        for(int i=0; i < l; i++) _writedata16_cont(color);
        _writecommand_last(ILI9341_T4_NOP);
        _endSPITransaction();        
        return;
        }



    FLASHMEM void ILI9341Driver::_updateAsync(const uint16_t* fb, DiffBuffBase* diff)
        {
        /*
        //override and use _updateNow instead
        int o = _rotation;
        _rotation = 0;
        _updateNow(fb, diff);
        _rotation = o;
        return;
        */
        if ((fb == nullptr)|| (diff == nullptr)) return; // do not call callback for invalid param.
        _waitUpdateAsyncComplete();
        _startframe(_vsync_spacing > 0);
        _stats_nb_uploaded_pixels = 0;
        _margin = ILI9341_T4_NB_SCANLINES;
        _dma_state = ILI9341_T4_DMA_ON;
        _dmaObject[_spi_num] = this; // set up object callback.
        //_flush_cache(fb, 2 * ILI9341_T4_NB_PIXELS); // BEWARE THAT CACHE IF FLUSHED BEFORE CALLING THIS METHOD !
        _fb = fb;
        _diff = diff;                
        diff->initRead();
        int x = 0, y = 0, len = 0;        
        int sc1 = diff->readDiff(x, y, len, 0); // scanline at 0 so sc1 will contain the scanline start position. 
        if (sc1 < 0)
            { // Diff is empty. 
            _dmaObject[_spi_num] = nullptr;
            if (_vsync_spacing > 0)
                { // note the next time. 
                uint32_t t1 = micros() + _microToReachScanLine(0, true) - _vsync_spacing  * _period;
                if (t1 > _timeframestart) _timeframestart = t1; 
                _late_start_ratio_override = false;
                _last_delta = _vsync_spacing;
                }
            _endframe();
            if (_touch_request_read)
                {
                _updateTouch2(); // touch update requested. do it now.
                _touch_request_read = false;
                }
            _dmaObject[_spi_num] = nullptr;
            _dma_state = ILI9341_T4_DMA_IDLE;
            return;
            }

        // write full PASET/CASET now and we shall only update the start position from now on. 
        _beginSPITransaction(_spi_clock);
        _writecommand_cont(ILI9341_T4_CASET); 
        _writedata16_cont(x);
        _writedata16_cont(ILI9341_T4_TFTWIDTH);
        _writecommand_cont(ILI9341_T4_PASET);
        _writedata16_cont(y);
        _writedata16_last(ILI9341_T4_TFTHEIGHT);
        _endSPITransaction();
        _prev_caset_x = x;
        _prev_paset_y = y;
        _slinitpos = sc1; // save the requested scanline initial position

        if (_vsync_spacing <= 0)
            { // call next method asap
            _pauseUploadTime();
            _setTimerIn(1, &ILI9341Driver::_subFrameTimerStartcb); // call now
            }
        else
            { // time the call of the next method with vsync 
            _pauseUploadTime();
            _setTimerAt(_timeframestart + (_vsync_spacing - 1) * _period, &ILI9341Driver::_subFrameTimerStartcb); // call at start of screen refresh. 
            }

        _pauseCpuTime();
        return;
        }


    void ILI9341Driver::_subFrameTimerStartcb()
        {
        // we should be around scanline 0 (unless we are late). 
        _restartCpuTime();
        _restartUploadTime();
        if (_vsync_spacing <= 0)
            {
            _pauseUploadTime();
            _setTimerIn(1, &ILI9341Driver::_subFrameTimerStartcb2);
            }
        else
            {
            int sc1 = _slinitpos;
            int sc2 = sc1 + ((ILI9341_T4_NB_SCANLINES - 1 - sc1) * _late_start_ratio); // maximum 'late' scanline
            uint32_t t2 = _microToReachScanLine(sc2, true); // with resync
            uint32_t t = _microToReachScanLine(sc1, false); // without resync            
            if (_late_start_ratio_override)
                { // force resync which is the same as asking _late_start_ratio=0;
                _late_start_ratio_override = false; // oneshot. 
                }
            else
                {
                if (t2 < t) t = 0;  // late, start right away. 
                }
            _pauseUploadTime();
            _setTimerIn(t, &ILI9341Driver::_subFrameTimerStartcb2); // call when ready to start transfer.        
            }
        _pauseCpuTime();
        return;
        }



    void ILI9341Driver::_subFrameTimerStartcb2()
        {
        _restartUploadTime();
        _restartCpuTime();

        if (_vsync_spacing > 0)
            {
            int t;
            while((t = _microToExitRange(0, _slinitpos))) { delayMicroseconds(t); }  // make sure we are good
            // ok, scanline is just after sc1 (if not late).
            _slinitpos = _getScanLine(false);   // save initial scanline position
            _em_async = 0;                      // start the counter
            const uint32_t tfs = micros() + _microToReachScanLine(0, false);        // time when this frame will start being displayed on the screen. 
            _last_delta = (int)round(((double)(tfs - _timeframestart)) / (_period));           
            _timeframestart = tfs;
            }

        // read the first instruction
        int x = 0, y = 0, len = 0;
        int asl = (_vsync_spacing > 0) ? _slinitpos  : (2 * ILI9341_T4_TFTHEIGHT);
        int r = _diff->readDiff(x, y, len, asl);
        if ((r != 0)||(len == 0)||(x != _prev_caset_x)||(y != _prev_paset_y))
            { // this should not happen, but try to fail gracefully.            
            _endframe();
            if (_touch_request_read)
                {
                _updateTouch2(); // touch update requested. do it now.
                _touch_request_read = false;
                }
            _dmaObject[_spi_num] = nullptr;
            _dma_state = ILI9341_T4_DMA_IDLE;
            return;
            }

        _dma_set_base_tcr();

        _last_y = (ILI9341_T4_TFTWIDTH * y + x + len) / ILI9341_T4_TFTWIDTH;
        _stats_nb_uploaded_pixels = len;

        _dmatx.sourceBuffer(_fb + x + (y * ILI9341_T4_TFTWIDTH), 2 * len);
        _dmatx.destination(_pimxrt_spi->TDR);
        _dmatx.TCD->ATTR_DST = 1;
        _dmatx.interruptAtCompletion();
        _dmatx.disableOnCompletion();
        
        _dmatx.triggerAtHardwareEvent(_spi_hardware->tx_dma_channel);
        if (_spi_num == 0) _dmatx.attachInterrupt(_dmaInterruptSPI0Diff);
        else if (_spi_num == 1) _dmatx.attachInterrupt(_dmaInterruptSPI1Diff);
        else _dmatx.attachInterrupt(_dmaInterruptSPI2Diff);
       
        // start spi transaction
        _beginSPITransaction(_spi_clock);

        _pimxrt_spi->FCR = 0;
        _maybeUpdateTCR(_tcr_dc_assert | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_RXMSK); // bug with the continu flag | LPSPI_TCR_CONT , why ?
        _pimxrt_spi->DER = LPSPI_DER_TDDE;
        _pimxrt_spi->SR = 0x3f00;

        if (_hardware_dc)
            {            
            _pimxrt_spi->FCR = LPSPI_FCR_TXWATER(2); // DO NOT CHANGE, WE NEED TO HAVE ALWAYS 14 WORD AVALAILABLE IN TX !!!
            }
        else
            { 
            // optimization depending on the spi speed. 
            // we want to start before the end of the last DMA transfer but not to early otherwise we will wait. 
            if (_spi_clock < 50000000) _pimxrt_spi->FCR = LPSPI_FCR_TXWATER(0);
            else if (_spi_clock < 80000000) _pimxrt_spi->FCR = LPSPI_FCR_TXWATER(1);
            else _pimxrt_spi->FCR = LPSPI_FCR_TXWATER(2);
            }

        _dma_assert_dc();
        _pimxrt_spi->TDR = ILI9341_T4_RAMWR;
        _dma_deassert_dc();

        noInterrupts();
        NVIC_SET_PRIORITY(IRQ_DMA_CH0 + _dmatx.channel, ILI9341_T4_IRQ_PRIORITY);
        _dmatx.begin(false);        
        _dmatx.enable(); // go !
        NVIC_SET_PRIORITY(IRQ_DMA_CH0 + _dmatx.channel, ILI9341_T4_IRQ_PRIORITY);
        interrupts();
        _pauseCpuTime();
        }



    void ILI9341Driver::_subFrameInterruptDiff()
        {
        if (_vsync_spacing > 0)
            { // check margin when using vsync
            int m = _last_y + ILI9341_T4_TFTHEIGHT - _slinitpos - _nbScanlineDuring(_em_async);
            if (m < _margin) _margin = m;
            }
        int x = 0, y = 0, len = 0;
        int asl = (_vsync_spacing > 0) ? (_slinitpos + _nbScanlineDuring(_em_async)) : (2 * ILI9341_T4_TFTHEIGHT);
        int r = _diff->readDiff(x, y, len, asl);
        if (r < 0)
            { // we are done !                    
            while (_pimxrt_spi->FSR & 0x1f);        // wait for transmit fifo to be empty
            while (_pimxrt_spi->SR & LPSPI_SR_MBF); // wait while spi bus is busy. 
            _pimxrt_spi->FCR = LPSPI_FCR_TXWATER(15); // Transmit Data Flag (TDF) should now be set when there if less or equal than 15 words in the transmit fifo
            _pimxrt_spi->DER = 0; // DMA no longer doing TX (nor RX)
            _pimxrt_spi->CR = LPSPI_CR_MEN | LPSPI_CR_RRF | LPSPI_CR_RTF; // enable module (MEM), reset RX fifo (RRF), reset TX fifo (RTF)
            _pimxrt_spi->SR = 0x3f00; // clear out all of the other status...
            /*
            _maybeUpdateTCR(_tcr_dc_assert | LPSPI_TCR_FRAMESZ(7)); // output Command with 8 bits            
            _writecommand_last(ILI9341_T4_NOP);
            */
            _endSPITransaction();
            _endframe();
            if (_touch_request_read)
                {
                _updateTouch2(); // touch update requested. do it now.
                _touch_request_read = false;
                }
            // _flush_cache(_fb, 2 * ILI9341_T4_NB_PIXELS);   /// NOT USEFUL AFTER NO ????
            _dmaObject[_spi_num] = nullptr;
            _dma_state = ILI9341_T4_DMA_IDLE;
            return;
            }
        else if (r > 0)
            { // we must wait
            int t = _timeForScanlines(r - asl + 1);
            if (t < ILI9341_T4_MIN_WAIT_TIME) t = ILI9341_T4_MIN_WAIT_TIME;
            //while (_pimxrt_spi->FSR & 0x1f);        // wait for transmit fifo to be empty
            //while (_pimxrt_spi->SR & LPSPI_SR_MBF); // wait while spi bus is busy. 
            _pauseUploadTime();
            _setTimerIn(t, &ILI9341Driver::_subFrameInterruptDiff2);
            _pauseCpuTime();            
            return;
            }
        // new instruction        
        if (_hardware_dc)
            { 
            // give all commands directly using the FIFO and hardware toogle of DC                    
            if (x != _prev_caset_x)
                {
                _pimxrt_spi->TCR = _dma_spi_tcr_assert;
                _pimxrt_spi->TDR = ILI9341_T4_CASET;
                _pimxrt_spi->TCR = _dma_spi_tcr_deassert;
                _pimxrt_spi->TDR = x;
                _prev_caset_x = x;
                }
            if (y != _prev_paset_y)
                {
                _pimxrt_spi->TCR = _dma_spi_tcr_assert;
                _pimxrt_spi->TDR = ILI9341_T4_PASET;
                _pimxrt_spi->TCR = _dma_spi_tcr_deassert;
                _pimxrt_spi->TDR = y;
                _prev_paset_y = y;
                }

            _pimxrt_spi->TCR = _dma_spi_tcr_assert;
            _pimxrt_spi->TDR = ILI9341_T4_RAMWR;
            _pimxrt_spi->TCR = _dma_spi_tcr_deassert;

            const uint16_t* p = _fb + x + (y * ILI9341_T4_TFTWIDTH);
            _last_y = (ILI9341_T4_TFTWIDTH * y + x + len) / ILI9341_T4_TFTWIDTH;
            _stats_nb_uploaded_pixels += len;

            if (len < 4)
                { // short buffer, use the fifo
                _pimxrt_spi->TDR = p[0];
                if (len >= 2)
                    {
                    _pimxrt_spi->TDR = p[1];
                    if (len >= 3) _pimxrt_spi->TDR = p[2];
                    }
                _pimxrt_spi->SR = 0x3f00; // clear flag
                _pimxrt_spi->IER = LPSPI_IER_TDIE; // interrupt when only 2 words remain in the fifo
                }
            else
                { // long buffer, use DMA. 
                _dmatx.sourceBuffer(p, len * 2);
                _dmatx.enable();
                }
            }
        else
            { // use the SPI interrupt to synchronize DC toogling without busy wait

            // prepare interrupt commands
            _spi_int_phase = 0;
            _spi_int_command[0] = ILI9341_T4_RAMWR;

            if (y != _prev_paset_y)
                {
                _spi_int_command[++_spi_int_phase] = y;
                _spi_int_command[++_spi_int_phase] = ILI9341_T4_PASET;
                _prev_paset_y = y;
                }
            if (x != _prev_caset_x)
                {
                _spi_int_command[++_spi_int_phase] = x;
                _spi_int_command[++_spi_int_phase] = ILI9341_T4_CASET;
                _prev_caset_x = x;
                }

            // prepare next DMA transfer
            _dmatx.sourceBuffer(_fb + x + (y * ILI9341_T4_TFTWIDTH), len * 2);
            _last_y = (ILI9341_T4_TFTWIDTH * y + x + len) / ILI9341_T4_TFTWIDTH;
            _stats_nb_uploaded_pixels += len;
           
            // make sure previous dma transfer is finished and assert DC and set frame(7)
            _dma_assert_dc(); // THIS IS THE ONLY BUSY WAIT: TXWATER is choosen to minimize this wait according to the SPI speed

            // clear flags
            _pimxrt_spi->SR = 0x3f00;

            // interrupt after each frame
            _pimxrt_spi->IER = LPSPI_IER_WCIE;  

            _pimxrt_spi->TDR = _spi_int_command[_spi_int_phase--]; // send the first command
            _pimxrt_spi->TCR = _dma_spi_tcr_deassert; // and then back to frame(15)

            // now we wait for the interrupt before toogling DC and sending the next frame... 
            }
        return;
        }


    void ILI9341Driver::_subFrameInterruptDiff2()
        {
        //noInterrupts();        // UNNEEDED ?
        _restartUploadTime();
        _restartCpuTime();
        _subFrameInterruptDiff();
        _pauseCpuTime();
        //interrupts();        // UNNEEDED ?
        }



    /**********************************************************************************************************
    * DMA Interrupts
    ***********************************************************************************************************/

    ILI9341Driver* volatile ILI9341Driver::_dmaObject[3] = { nullptr, nullptr, nullptr }; 


    void ILI9341Driver::_dmaInterruptDiff()
        { 
        //noInterrupts();        // UNNEEDED ?
        _dmatx.clearInterrupt();
        _dmatx.clearComplete();
        _restartCpuTime();
        _stats_nb_transactions++; // count a spi transaction
        _subFrameInterruptDiff();
        _pauseCpuTime();
        //interrupts();        // UNNEEDED ?
        return;
        }











    /**********************************************************************************************************
    * IntervalTimer
    ***********************************************************************************************************/

    ILI9341Driver* volatile ILI9341Driver::_pitObj[4] = {nullptr, nullptr, nullptr, nullptr};   // definition 

    void ILI9341Driver::_timerinit()
        {
        _istimer = false; 
        for (int i = 0; i < 4; i++)
            {
            if (_pitObj[i] == nullptr)
                {
                _pitObj[i] = this;
                _pitindex = i;
                return;
                }
            }
        // OUCH !Boom boom boom booom...
        _print("\n *** TOO MANY INSTANCES OF ILI9341Driver CREATED ***\n\n");
        }










    /**********************************************************************************************************
    * SPI
    ***********************************************************************************************************/


    void ILI9341Driver::_drawRect(int xmin, int xmax, int ymin, int ymax, uint16_t color)
        {
        _waitUpdateAsyncComplete();
        _beginSPITransaction(_spi_clock);
        _writecommand_cont(ILI9341_T4_PASET);
        _writedata16_cont(ymin);
        _writedata16_cont(ymax);
        _writecommand_cont(ILI9341_T4_CASET);
        _writedata16_cont(xmin);
        _writedata16_cont(xmax);
        _writecommand_cont(ILI9341_T4_RAMWR);
        for (int i = 0; i < (xmax - xmin + 1) * (ymax - ymin + 1); i++) _writedata16_cont(color);
        _writecommand_last(ILI9341_T4_NOP);
        _endSPITransaction();
        _mirrorfb = nullptr;
        _ongoingDiff = nullptr;
        }


    uint8_t ILI9341Driver::_readcommand8(uint8_t c, uint8_t index, int timeout_ms)
        {
        // Bail if not valid miso
        if (_miso == 0xff) return 0;        
        uint8_t r = 0;
        _beginSPITransaction(_spi_clock_read);
        // Lets assume that queues are empty as we just started transaction.
        _pimxrt_spi->CR = LPSPI_CR_MEN | LPSPI_CR_RRF /* | LPSPI_CR_RTF */; // actually clear both...        
        _maybeUpdateTCR(_tcr_dc_assert | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_CONT);
        _pimxrt_spi->TDR = 0xD9; // writecommand(0xD9); // sekret command        
        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_CONT);
        _pimxrt_spi->TDR = 0x10 + index; // writedata(0x10 + index);        
        _maybeUpdateTCR(_tcr_dc_assert | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_CONT);
        _pimxrt_spi->TDR = c; // writecommand(c);        
        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(7));
        _pimxrt_spi->TDR = 0; // readdata
        // Now wait until completed.
        elapsedMillis ems; 
        uint8_t rx_count = 4;
        while (rx_count && ((timeout_ms <= 0) || (ems < (uint32_t)timeout_ms)))
            {
            if ((_pimxrt_spi->RSR & LPSPI_RSR_RXEMPTY) == 0)
                {
                r = _pimxrt_spi->RDR; // Read any pending RX bytes in
                rx_count--;           // decrement count of bytes still levt
                }
            }
        _endSPITransaction();
        if (rx_count) return 0; // timeout
        return r; // get the received byte... should check for it first...
        }



    void ILI9341Driver::_waitFifoNotFull()
        {
        uint32_t tmp __attribute__((unused));
        do
            {
            if ((_pimxrt_spi->RSR & LPSPI_RSR_RXEMPTY) == 0)
                {
                tmp = _pimxrt_spi->RDR; // Read any pending RX bytes in
                if (_pending_rx_count) _pending_rx_count--; // decrement count of bytes still levt
                }
            } 
        while ((_pimxrt_spi->SR & LPSPI_SR_TDF) == 0);
        }


    void ILI9341Driver::_waitTransmitComplete()
        {           
        uint32_t tmp __attribute__((unused));
        while (_pending_rx_count)
            {
            if ((_pimxrt_spi->RSR & LPSPI_RSR_RXEMPTY) == 0)
                {
                tmp = _pimxrt_spi->RDR; // Read any pending RX bytes in
                _pending_rx_count--;     // decrement count of bytes still levt
                }
            }
        _pimxrt_spi->CR = LPSPI_CR_MEN | LPSPI_CR_RRF; // Clear RX FIFO
        }





    /**********************************************************************************************************
    * Drawing characters
    * (adapted from the tgx library)
    ***********************************************************************************************************/



    uint32_t ILI9341Driver::_fetchbits_unsigned(const uint8_t* p, uint32_t index, uint32_t required)
        {
        uint32_t val;
        uint8_t* s = (uint8_t*)&p[index >> 3];
    #ifdef UNALIGNED_IS_SAFE        // is this defined anywhere ? 
        val = *(uint32_t*)s; // read 4 bytes - unaligned is ok
        val = __builtin_bswap32(val); // change to big-endian order
    #else
        val = s[0] << 24;
        val |= (s[1] << 16);
        val |= (s[2] << 8);
        val |= s[3];
    #endif
        val <<= (index & 7); // shift out used bits
        if (32 - (index & 7) < required) 
            { // need to get more bits
            val |= (s[4] >> (8 - (index & 7)));
            }
        val >>= (32 - required); // right align the bits
        return val;
        }


    uint32_t ILI9341Driver::_fetchbits_signed(const uint8_t* p, uint32_t index, uint32_t required)
        {
        uint32_t val = _fetchbits_unsigned(p, index, required);
        if (val & (1 << (required - 1))) 
            {
            return (int32_t)val - (1 << required);
            }
        return (int32_t)val;
        }


    bool ILI9341Driver::_clipit(int& x, int& y, int& sx, int& sy, int & b_left, int & b_up, int lx, int ly)
        {
        b_left = 0;
        b_up = 0; 
        if ((sx < 1) || (sy < 1) || (y >= ly) || (y + sy <= 0) || (x >= lx) || (x + sx <= 0))
            { // completely outside of image
            return false;
            }
        if (y < 0)
            {
            b_up = -y;
            sy += y;
            y = 0;
            }
        if (y + sy > ly)
            {
            sy = ly - y;
            }
        if (x < 0)
            {
            b_left = -x;
            sx += x;
            x = 0;
            }
        if (x + sx > lx)
            {
            sx = lx - x;
            }
        return true;
        }



    FLASHMEM void ILI9341Driver::_measureChar(char c, int pos_x, int pos_y, int& min_x, int& max_x, int& min_y, int& max_y, const void* pfont, int& xadvance)
        {
        const ILI9341_t3_font_t & font = *((const ILI9341_t3_font_t *)pfont);
        uint8_t n = (uint8_t)c;
        if ((n >= font.index1_first) && (n <= font.index1_last))
            {
            n -= font.index1_first;
            }
        else if ((n >= font.index2_first) && (n <= font.index2_last))
            {
            n = (n - font.index2_first) + (font.index1_last - font.index1_first + 1);
            }
        else
            { // no char to draw
            xadvance = 0;
            return; // nothing to draw. 
            }
        uint8_t* data = (uint8_t*)font.data + _fetchbits_unsigned(font.index, (n * font.bits_index), font.bits_index);
        int32_t off = 0;
        uint32_t encoding = _fetchbits_unsigned(data, off, 3);
        if (encoding != 0)
            { // wrong/unsupported format
            xadvance = 0;
            return; 
            }
        off += 3;
        const int sx = (int)_fetchbits_unsigned(data, off, font.bits_width);
        off += font.bits_width;
        const int sy = (int)_fetchbits_unsigned(data, off, font.bits_height);
        off += font.bits_height;
        const int xoffset = (int)_fetchbits_signed(data, off, font.bits_xoffset);
        off += font.bits_xoffset;
        const int yoffset = (int)_fetchbits_signed(data, off, font.bits_yoffset);
        off += font.bits_yoffset;
        xadvance = (int)_fetchbits_unsigned(data, off, font.bits_delta);
        const int x = pos_x + xoffset;
        const int y = pos_y - sy - yoffset;
        min_x = x;
        max_x = x + sx - 1;
        min_y = y;
        max_y = y + sy - 1;
        }


    FLASHMEM void ILI9341Driver::_measureText(const char* text, int pos_x, int pos_y, int& min_x, int& max_x, int& min_y, int& max_y, const void * pfont, bool start_newline_at_0)
        {
        const int startx = start_newline_at_0 ? 0 : pos_x;        
        min_x = pos_x; 
        max_x = pos_x; 
        min_y = pos_y; 
        max_y = pos_y;        
        const size_t l = strlen(text);
        for (size_t i = 0; i < l; i++)
            {
            const char c = text[i];
            if (c == '\n')
                {
                pos_x = startx;
                pos_y += ((const ILI9341_t3_font_t*)pfont)->line_space;
                }
            else
                {
                int xa = 0;
                int mx = min_x; 
                int Mx = max_x;
                int my = min_y;
                int My = max_y;
                _measureChar(c, pos_x, pos_y, mx, Mx, my, My, pfont, xa);
                if (mx < min_x) min_x = mx; 
                if (my < min_y) min_y = my;
                if (Mx > max_x) max_x = Mx;
                if (My > max_y) max_y = My;
                pos_x += xa;
                }
            }
        }



    FLASHMEM void ILI9341Driver::_drawTextILI(const char* text, int pos_x , int  pos_y, uint16_t col, const void * pfont, bool start_newline_at_0, int lx, int ly, int stride, uint16_t* buffer, float opacity)
        {
        if (opacity > 1.0f) opacity = 1.0f;
        const int startx = start_newline_at_0 ? 0 : pos_x;
        const size_t l = strlen(text);
        for (size_t i = 0; i < l; i++)
            {
            const char c = text[i];
            if (c == '\n')
                {
                pos_x = startx;
                pos_y += ((const ILI9341_t3_font_t*)pfont)->line_space;
                }
            else
                {
                _drawCharILI(c, pos_x, pos_y, col, pfont, lx, ly, stride, buffer, opacity);
                }
            }
        }


    FLASHMEM void ILI9341Driver::_drawCharILI(char c, int & pos_x, int & pos_y, uint16_t col, const void * pfont, int lx, int ly, int stride, uint16_t* buffer, float opacity)
        {
        const ILI9341_t3_font_t& font = *((const ILI9341_t3_font_t*)pfont);
        uint8_t n = (uint8_t)c;
        if ((n >= font.index1_first) && (n <= font.index1_last))
            {
            n -= font.index1_first;
            }
        else if ((n >= font.index2_first) && (n <= font.index2_last))
            {
            n = (n - font.index2_first) + (font.index1_last - font.index1_first + 1);
            }
        else
            { // no char to draw
            return;
            }
        uint8_t * data = (uint8_t *)font.data + _fetchbits_unsigned(font.index, (n*font.bits_index), font.bits_index);
        int32_t off = 0; 
        uint32_t encoding = _fetchbits_unsigned(data, off, 3);
        if (encoding != 0) return; // wrong/unsupported format
        off += 3;
        int sx = (int)_fetchbits_unsigned(data, off, font.bits_width);
        off += font.bits_width;         
        int sy = (int)_fetchbits_unsigned(data, off, font.bits_height);
        off += font.bits_height;            
        const int xoffset = (int)_fetchbits_signed(data, off, font.bits_xoffset);
        off += font.bits_xoffset;
        const int yoffset = (int)_fetchbits_signed(data, off, font.bits_yoffset);
        off += font.bits_yoffset;
        const int delta = (int)_fetchbits_unsigned(data, off, font.bits_delta);
        off += font.bits_delta;
        int x = pos_x + xoffset;
        int y = pos_y - sy - yoffset;
        const int rsx = sx; // save the real bitmap width; 
        int b_left, b_up;
        if ((_clipit(x, y, sx, sy, b_left, b_up, lx, ly)) && (font.version == 23) && (font.reserved == 2))
            { // only draw antialised, 4bpp, v2.3 characters
            data += (off >> 3) + ((off & 7) ? 1 : 0); // bitmap begins at the next byte boundary
            _drawCharBitmap_4BPP(data, rsx, b_up, b_left, sx, sy, x, y, col, stride, buffer, opacity);
            }
        pos_x += delta;
        return;
        }


    FLASHMEM void ILI9341Driver::_drawCharBitmap_4BPP(const uint8_t* bitmap, int rsx, int b_up, int b_left, int sx, int sy, int x, int y, uint16_t col, int stride, uint16_t * buffer, float opacity)
        {
        const int iop = 137 * (int)(256 * opacity);
        if (sx >= 2)
            { // each row has at least 2 pixels
            for (int dy = 0; dy < sy; dy++)
                {
                int32_t off = (b_up + dy) * (rsx) + (b_left);
                uint16_t* p = buffer + (stride) * (y + dy) + (x);
                int dx = sx;
                if (off & 1)
                    {// not at the start of a bitmap byte: we first finish it. 
                    const uint8_t b = bitmap[off >> 1];
                    const int v = (b & 15); 
                    *p = _blend32(*p, col, (v * iop) >> 14);
                    p++;
                    off++; dx--;
                    }
                while (dx >= 2)
                    {
                    const uint8_t b = bitmap[off >> 1];
                    if (b)
                        {
                        { const int v = ((b & 240) >> 4); p[0] = _blend32(p[0], col, (v * iop) >> 14); }
                        { const int v = (b & 15); p[1] = _blend32(p[1], col, (v * iop) >> 14); }
                        }
                    off += 2;
                    p += 2;
                    dx -= 2;
                    }
                if (dx > 0)
                    {
                    const uint8_t b = bitmap[off >> 1];
                    const int v = ((b & 240) >> 4); *p = _blend32(*p, col, (v * iop) >> 14);
                    }
                }
            }
        else
            { // each row has a single pixel 
            uint16_t* p = buffer + (stride) * (y) + (x);
            int32_t off = (b_up) * (rsx) + (b_left);
            while (sy-- > 0)
                {
                const uint8_t b = bitmap[off >> 1];
                const int v = (off & 1) ? (b & 15) : ((b & 240) >> 4);
                *p = _blend32(*p, col, (v * iop) >> 14);
                p += stride;
                off += rsx;
                }
            }
        }


    FLASHMEM void ILI9341Driver::_fillRect(int xmin, int xmax, int ymin, int ymax, int lx, int ly, int stride, uint16_t* buffer, uint16_t color, float opacity)
        {
        if (xmin < 0) xmin = 0; 
        if (xmax >= lx) xmax = lx - 1;
        if (ymin < 0) ymin = 0;
        if (ymax >= lx) ymax = ly - 1;

        const uint32_t a = (opacity >= 1.0f) ? 32 : (32 * opacity);
        for (int j = ymin; j <= ymax; j++)
            {
            for (int i = xmin; i <= xmax; i++)
                {
                auto& d = buffer[i + j * stride];
                d = _blend32(d, color, a);
                }
            }
        }






    FLASHMEM void ILI9341Driver::_uploadText(const char* text, int pos_x, int pos_y, uint16_t col, uint16_t col_bg, const void* pfont, bool start_newline_at_0)
        {
        const int startx = start_newline_at_0 ? 0 : pos_x;
        const size_t l = strlen(text);
        for (size_t i = 0; i < l; i++)
            {
            const char c = text[i];
            if (c == '\n')
                {
                pos_x = startx;
                pos_y += ((const ILI9341_t3_font_t*)pfont)->line_space;
                }
            else
                {
                _uploadChar(c, pos_x, pos_y, col, col_bg, pfont);
                }
            }
        }


    FLASHMEM void ILI9341Driver::_uploadChar(char c, int& pos_x, int& pos_y, uint16_t col, uint16_t col_bg, const void* pfont)
        {
        const int MAX_CHAR_SIZE_LX = 20; 
        const int MAX_CHAR_SIZE_LY = 20; 

        uint16_t buffer[MAX_CHAR_SIZE_LX * MAX_CHAR_SIZE_LY]; // memory buffer large enough to hold 1 char

        for (int i = 0; i < MAX_CHAR_SIZE_LX * MAX_CHAR_SIZE_LY; i++) buffer[i] = col_bg; // clear to the background color

        int min_x, min_y, max_x, max_y, xa; 
        _measureChar(c, 0, 0, min_x, max_x, min_y, max_y, pfont, xa); // find out the dimension of the char

        max_x -= min_x;         // set top left corner at (0,0)
        int nx = -min_x;        //
        min_x = 0;              // 
        max_y -= min_y;         //
        int ny = -min_y;        //
        min_y = 0;              //

        _drawCharILI(c, nx, ny, col, pfont, MAX_CHAR_SIZE_LX, MAX_CHAR_SIZE_LY, MAX_CHAR_SIZE_LX, buffer, 1.0f); // draw the char on the buffer
        _updateRectNow(buffer, pos_x, pos_x + max_x , pos_y - ny, pos_y + max_y - ny, MAX_CHAR_SIZE_LX); // upload it to the screen

        pos_x += (nx + min_x);

        }















    FLASHMEM void ILI9341Driver::overlayFPS(uint16_t* fb, int position, uint16_t fg_color, uint16_t bk_color, float opacity)
        {
        char text[8] = "--- FPS";
        int text_off = 0;
        int fps = statsCurrentFPS();
        if (fps > 999) fps = 999;
        if (fps > 0)
            {
            text_off = 2;
            text[text_off] = '0' + (fps % 10);
            if (fps >= 10)
                {
                text_off--;
                fps /= 10; 
                text[text_off] = '0' + (fps % 10);
                }
            if (fps >= 10)
                {
                text_off--;
                fps /= 10; 
                text[text_off] = '0' + (fps % 10);
                }
            }

        int x = 0;
        int y = 0;
        int _fps_xmin, _fps_xmax, _fps_ymin, _fps_ymax;
        _measureText(text + text_off, x, y, _fps_xmin, _fps_xmax, _fps_ymin, _fps_ymax, &font_ILI9341_T4_OpenSans_Bold_10, false);
        _fps_xmin -= 2;
        _fps_xmax += 2;
        _fps_ymin -= 3;
        _fps_ymax += 2;

        int dx, dy;
        switch (position)
            {
            case 1: 
                {
                dx = _width - 1 - _fps_xmax;
                dy = _height - 1 - _fps_ymax;
                break;
                }
            case 2:
                {
                dx = -_fps_xmin;
                dy = _height - 1 - _fps_ymax;
                break;
                }
            case 3:
                {
                dx = -_fps_xmin;
                dy = -_fps_ymin;
                break;
                }
            default:
                {
                dx = _width - 1 - _fps_xmax;
                dy = -_fps_ymin;
                break;
                }
            }

        x += dx;
        _fps_xmin += dx;
        _fps_xmax += dx;
        y += dy;
        _fps_ymin += dy;
        _fps_ymax += dy;

        _fillRect(_fps_xmin, _fps_xmax, _fps_ymin, _fps_ymax, _width, _height, _width, fb, bk_color, opacity);
        _drawTextILI(text + text_off, x, y, fg_color, &font_ILI9341_T4_OpenSans_Bold_10, false, _width, _height, _width, fb, opacity + 0.3f);
        }





    /**********************************************************************************************************
    * Statistics
    ***********************************************************************************************************/


    FLASHMEM void ILI9341Driver::statsReset()
        {
        _stats_nb_frame = 0;
        _stats_elapsed_total = 0;       
        _statsvar_cputime.reset();
        _statsvar_uploadtime.reset();
        _statsvar_uploaded_pixels.reset(); 
        _statsvar_transactions.reset();
        _statsvar_margin.reset(); 
        _statsvar_vsyncspacing.reset();
        _nbteared = 0;       

        _stats_current_fps = -1;
        _statsvar_fps.reset();
        _fps_ticks = 0; 
        _fps_counter = 0;
        }


    FLASHMEM void ILI9341Driver::printStats(bool diff_stats) const
        {
        if (_outputStream)
            {
            //_waitUpdateAsyncComplete();
            _print("----------------- ILI9341Driver Stats ----------------\n");
            _print("[Configuration]\n");
            _printf("- SPI speed          : write=%u  read=%u\n", _spi_clock, _spi_clock_read);
            _print("- screen orientation : ");
            switch (getRotation())
                {
                case 0: _print("0 (PORTRAIT_240x320)\n"); break;
                case 1: _print("1 (LANDSCAPE_320x240)\n"); break;
                case 2: _print("2 (PORTRAIT_240x320_FLIPPED)\n"); break;
                case 3: _print("3 (LANDSCAPE_320x240_FLIPPED)\n"); break;
                }

            _printf("- refresh rate       : %.1fHz  (mode %u)\n", getRefreshRate(), getRefreshMode());
            int m = bufferingMode();
            _printf("- buffering mode     : %u", m);
            switch (m)
                {
                case NO_BUFFERING:  _print(" (NO BUFFERING)\n"); break;
                case DOUBLE_BUFFERING: _print(" (DOUBLE BUFFERING)\n"); break;
                }
            _printf("- vsync_spacing      : %i ", _vsync_spacing);
            if (_vsync_spacing <= 0)
                _print(" (VSYNC DISABLED).\n");
            else
                _print(" (VSYNC ENABLED).\n");

            _print("- requested FPS      : ");
            if (_vsync_spacing == -1)
                _print("max fps [drop frames when busy]\n");
            else if (_vsync_spacing == 0)
                _print("max fps [do not drop frames]\n");
            else
                _printf("%.1fHz [=refresh_rate/vsync_spacing]\n", getRefreshRate() / _vsync_spacing);

            if (diffUpdateActive())
                {
                if (_diff2)
                    {
                    _print("- diff. updates      : ENABLED - 2 diffs buffers.\n");
                    }
                else
                    {
                    _print("- diff. updates      : ENABLED - 1 diff buffer.\n");
                    }
                _printf("- diff [gap]         : %u\n", _diff_gap);
                if (_compare_mask == 0)
                    {
                    _print("- diff [compare_mask]: STRICT COMPARISON.");
                    }
                else
                    {
                    _print("- diff [compare_mask]: R=");
                    for (int i = 15; i >= 11; i--) { _print(bitRead(_compare_mask, i) ? '1' : '0'); }
                    _print(" G=");
                    for (int i = 10; i >= 5; i--) { _print(bitRead(_compare_mask, i) ? '1' : '0'); }
                    _print(" B=");
                    for (int i = 4; i >= 0; i--) { _print(bitRead(_compare_mask, i) ? '1' : '0'); }
                    }
                }
            else
                {
                if (_diff1 == nullptr)
                    _print("- diff. updates      : DISABLED.\n");
                else
                    _print("- differential update: DISABLED [ONLY 1 DIFF BUFFER PROVIDED WHEN 2 ARE NEEDED WITH TRIPLE BUFFERING]\n");
                }

            _print("\n\n[Statistics]\n");
            _printf("- average framerate  : %.1f FPS  (%u frames in %ums)\n", statsFramerate(), statsNbFrames(), statsTotalTime());
            _printf("- instant. framerate : %d FPS  [min=%d , max=%d] std=%.1f\n", statsCurrentFPS(), statsFPS().min(), statsFPS().max(), statsFPS().std());
            if (diffUpdateActive())
                _printf("- upload rate        : %.1f FPS  (%.2fx compared to full redraw)\n", 1000000.0f / _statsvar_uploadtime.avg(), statsDiffSpeedUp());
            else
                _printf("- upload rate        : %.1f FPS\n", 1000000.0f / _statsvar_uploadtime.avg());
            _print("- upload time / frame: "); _statsvar_uploadtime.print("us", "\n", _outputStream);
            _print("- CPU time / frame   : "); _statsvar_cputime.print("us", "\n", _outputStream);
            _print("- pixels / frame     : "); _statsvar_uploaded_pixels.print("", "\n", _outputStream);
            _print("- transact. / frame  : "); _statsvar_transactions.print("", "\n", _outputStream);
            if (_vsync_spacing > 0)
                {
                _printf("- teared frames      : %u (%.1f%%)\n", statsNbTeared(), 100 * statsRatioTeared());
                _print("- real vsync spacing : "); _statsvar_vsyncspacing.print("", "\n", _outputStream, true);
                _print("- margin / frame     : "); _statsvar_margin.print("", "\n", _outputStream);
                }
            _print("\n");

            if (diff_stats)
                {
                if (_diff1)
                    {
                    _print("First diff buffer:\n"); 
                    _diff1->printStats(_outputStream);
                    }
                if (_diff2)
                    {
                    _print("Second diff buffer:\n");
                    _diff2->printStats(_outputStream);
                    }
                }
            _print("------------------------------------------------------\n\n");
            }
        }



    void ILI9341Driver::_endframe()
        {
        _fps_ticks++;
        const uint32_t tfps = _fps_counter;
        if (tfps > 1000)
            {
            uint32_t new_fps = roundf((1000.0f * _fps_ticks) / tfps);
            if (_stats_current_fps > 0)
                {                
                _statsvar_fps.push(new_fps);
                }
            _stats_current_fps = new_fps;
            _fps_counter = 0;
            _fps_ticks = 0; 
            }

        _stats_nb_frame++;

        _stats_cputime += _stats_elapsed_cputime;
        _statsvar_cputime.push(_stats_cputime);

        _stats_uploadtime += _stats_elapsed_uploadtime;
        _statsvar_uploadtime.push(_stats_uploadtime);

        _statsvar_uploaded_pixels.push(_stats_nb_uploaded_pixels);

        _statsvar_transactions.push(_stats_nb_transactions);

        if (_vsync_spacing > 0)
            {
            if (_statsvar_margin.count() > 0) _statsvar_vsyncspacing.push(_last_delta);

            if (_margin < 0) _nbteared++;

            _statsvar_margin.push(_margin);
            }
        }








    /**********************************************************************************************************
    * Touch
    ***********************************************************************************************************/


    ILI9341Driver* volatile ILI9341Driver::_touchObjects[4] = { nullptr, nullptr, nullptr, nullptr };


    /** set the touch interrupt routine */
    FLASHMEM void ILI9341Driver::_setTouchInterrupt()
        {
        _touch_z_threshold = ILI9341_T4_TOUCH_Z_THRESHOLD;
        _touch_has_calibration = false;

        _touch_request_read = false;
        _touched = true;;
        _touched_read = true;
        _touch_x = _touch_y = _touch_z = 0;

        bool slotfound = false;
        if ((_touch_irq >= 0) && (_touch_irq < 42)) // valid digital pin
            {
            pinMode(_touch_irq, INPUT);
            if ((!slotfound) && (_touchObjects[0] == nullptr)) { _touchObjects[0] = this; attachInterrupt(_touch_irq, _touch_int0, FALLING); slotfound = true; }
            if ((!slotfound) && (_touchObjects[1] == nullptr)) { _touchObjects[1] = this; attachInterrupt(_touch_irq, _touch_int1, FALLING); slotfound = true; }
            if ((!slotfound) && (_touchObjects[2] == nullptr)) { _touchObjects[2] = this; attachInterrupt(_touch_irq, _touch_int2, FALLING); slotfound = true; }
            if ((!slotfound) && (_touchObjects[3] == nullptr)) { _touchObjects[3] = this; attachInterrupt(_touch_irq, _touch_int3, FALLING); slotfound = true; }
            }
        if (!slotfound) { _touch_irq = 255; } // disable touch irq
        }


    int32_t ILI9341Driver::lastTouched()
        {
        const bool b = _touched;
        _touched = false;
        return (b && (_touch_irq != 255)) ? ((int32_t)_em_touched_irq) : -1;
        }


    void ILI9341Driver::_updateTouch2()
        {
        int16_t data[6];
        int z;
        _pspi->beginTransaction(SPISettings(_spi_clock_read, MSBFIRST, SPI_MODE0));
        digitalWrite(_touch_cs, LOW);
        _pspi->transfer(0xB1);
        int16_t z1 = _pspi->transfer16(0xC1 /* Z2 */) >> 3;
        z = z1 + 4095;
        int16_t z2 = _pspi->transfer16(0x91 /* X */) >> 3;
        z -= z2;
        if (z >= _touch_z_threshold)
            {
            _pspi->transfer16(0x91 /* X */);  // dummy X measure, 1st is always noisy
            data[0] = _pspi->transfer16(0xD1 /* Y */) >> 3;
            data[1] = _pspi->transfer16(0x91 /* X */) >> 3; // make 3 x-y measurements
            data[2] = _pspi->transfer16(0xD1 /* Y */) >> 3;
            data[3] = _pspi->transfer16(0x91 /* X */) >> 3;
            }
        else
            {
            data[0] = data[1] = data[2] = data[3] = 0;  // Compiler warns these values may be used unset on early exit.
            }
        data[4] = _pspi->transfer16(0xD0 /* Y */) >> 3; // Last Y touch power down
        data[5] = _pspi->transfer16(0) >> 3;
        digitalWrite(_touch_cs, HIGH);
        _pspi->endTransaction();

        if (z < _touch_z_threshold)
            {
            _touch_z = 0;
            if (z < ILI9341_T4_TOUCH_Z_THRESHOLD_INT)
                {
                if (_touch_irq != 255) _touched_read = false;
                }
            return;
            }

        int x = _besttwoavg(data[1], data[3], data[5]);
        int y = _besttwoavg(data[0], data[2], data[4]);

        _touch_x = x;
        _touch_y = y;
        _touch_z = z;
        _em_touched_read = 0; // good read completed, set wait
        }


    void ILI9341Driver::_updateTouch()
        {
        if (_em_touched_read < ILI9341_T4_TOUCH_MSEC_THRESHOLD) return; // read not so long ago
        if ((_touch_irq != 255) && (_touched_read == false)) return; // nothing to do. 
        if (asyncUpdateActive())
            {
            _touch_request_read = true; // request read at end of transfer
            while ((_touch_request_read) && (asyncUpdateActive())); // wait until transfer complete or reading done. 
            if (!_touch_request_read) return; // reading was done, nothing to do. 
            _touch_request_read = false; // remove request. 
            }
        // we can do the reading now
        _updateTouch2();
        return;
        }



    bool ILI9341Driver::readTouch(int& x, int& y, int& z)
        {
        _updateTouch();
        if (_touch_z < _touch_z_threshold) return false;
        z = _touch_z;
        if (_touch_has_calibration)
            { // coord in orientation 0. 
            int xx  = _mapTouchX(_touch_x, _touch_calib[0], _touch_calib[1]);
            int yy = _mapTouchY(_touch_y, _touch_calib[2], _touch_calib[3]); 
            switch (_rotation)
                {
            case 0:
                x = xx;
                y = yy;
                break;
            case 1:
                x = yy;
                y = ILI9341_T4_TFTWIDTH - 1 - xx;
                break;
            case 2:
                x = ILI9341_T4_TFTWIDTH - 1 - xx;
                y = ILI9341_T4_TFTHEIGHT - 1 - yy;
                break;
            case 3:
                x = ILI9341_T4_TFTHEIGHT - 1 - yy;
                y = xx;
                break;
                }
            }
        else
            { // raw values
            x = _touch_x;
            y = _touch_y;
            }
        return true;
        }



    int16_t ILI9341Driver::_besttwoavg(int16_t x, int16_t y, int16_t z)
        {
        int16_t da, db, dc;
        int16_t reta = 0;
        if (x > y) da = x - y; else da = y - x;
        if (x > z) db = x - z; else db = z - x;
        if (z > y) dc = z - y; else dc = y - z;
        if (da <= db && da <= dc) reta = (x + y) >> 1;
        else if (db <= da && db <= dc) reta = (x + z) >> 1;
        else reta = (y + z) >> 1;   //    else if ( dc <= da && dc <= db ) reta = (x + y) >> 1;
        return (reta);
        }


    FLASHMEM void ILI9341Driver::setTouchCalibration(int touchCalibration[4])
        {
        if (touchCalibration)
            {
            _touch_has_calibration = true;
            for (int i = 0; i < 4; i++) _touch_calib[i] = touchCalibration[i];
            }
        else
            {
            _touch_has_calibration = false;
            }
        }


    FLASHMEM bool ILI9341Driver::getTouchCalibration(int touchCalibration[4])
        {
        if (_touch_has_calibration)
            {
            for (int i = 0; i < 4; i++) touchCalibration[i] = _touch_calib[i];
            return true;
            }
        else
            {
            return false;
            }
        }



    FLASHMEM void ILI9341Driver::_calibRect(int cx, int cy, int R, uint16_t col_ext, uint16_t col_int)
        {
        const int R1 = R / 2;
        _pushRect(col_ext, cx - R, cx + R, cy - R, cy + R);
        _pushRect(col_int, cx - R1, cx + R1, cy - R1, cy + R1);
        const int dx = (cx > 120) ? -1 : 1;
        const int dy = (cy > 160) ? -1 : 1;
        for (int k = 0; k < 8 * R; k++)
            {
            _pushRect(col_ext, cx + (R + 1 + k)*dx, cx + (R + 1 + k) * dx + 1, cy + (R + 1 + k) * dy, cy + (R + 1 + k) * dy + 1);
            }
        }


    FLASHMEM void ILI9341Driver::_calibTouch(int& x, int& y, int& z, int prv_x, int prv_y)
        {
        const int NB_SAMPLE = 3;
        const int MIN_DIST = 500;
        do 
            {
            _updateTouch();
            delay(10);
            } 
        while (_touch_z > 0);

        while (1)
            {
            int nbs = 0;
            x = 0;
            y = 0;
            while(nbs < NB_SAMPLE)
                {
                _touch_z = 0;            
                _updateTouch();
                if (_touch_z >= _touch_z_threshold)
                    {
                    nbs++;
                    x += _touch_x;
                    y += _touch_y;
                    } 
                delay(10);
                }
            x /= NB_SAMPLE;
            y /= NB_SAMPLE;
            if (((abs(x - prv_x) > MIN_DIST) || (prv_x < 0)) || ((abs(y - prv_y) > MIN_DIST) || (prv_y < 0))) return;
            }
        }


    FLASHMEM void ILI9341Driver::calibrateTouch(int touchCalibration[4])
        {
        _waitUpdateAsyncComplete();

        int oldrot = getRotation(); 
        setRotation(0); 

        const uint16_t BLACK = 0;
        const uint16_t WHITE = 0xFFFF;
        const uint16_t GREY = 25 + (25 << 11) + (51 << 5);
        const uint16_t RED = 31 << 11;
        const uint16_t BLUE = 31;
        const uint16_t GREEN = 50 << 5;
        const int RADIUS = 6;

        _print("\n\n------------- Touch Calibration ---------------\n");

    restart_calib:

        int x[4];
        int y[4];
        int z[4];

        for (int i = 0; i < 4; i++)
            {
            char tn[4] = { (char)('1' + i),'/', '4', 0};
            clear(WHITE);         
            _uploadText("Touchscreen calibration", 30, 120, BLACK, WHITE, (void*)(&font_ILI9341_T4_OpenSans_Bold_16), true);
            _uploadText("Touch the center of the", 50, 170, BLUE, WHITE, (void*)(&font_ILI9341_T4_OpenSans_Bold_12), true);
            _uploadText("red square in the corner", 50, 185, BLUE, WHITE, (void*)(&font_ILI9341_T4_OpenSans_Bold_12), true);
            _uploadText(tn, 105, 215, 31, 0xFFFF, (void*)(&font_ILI9341_T4_OpenSans_Bold_16), true);            
            _printf("\n- corner %s: touch the center of the red rectangle... ", tn);
            switch (i)
                {
                case 0:
                    _calibRect(ILI9341_T4_TFTWIDTH - 1 - RADIUS, ILI9341_T4_TFTHEIGHT - 1 - RADIUS, RADIUS, BLUE, RED);
                    break;

                case 1:
                    _calibRect(RADIUS, ILI9341_T4_TFTHEIGHT - 1 - RADIUS, RADIUS, BLUE, RED);
                    break;

                case 2:
                    _calibRect(RADIUS, RADIUS, RADIUS, BLUE, RED);
                    break;

                case 3:
                    _calibRect(ILI9341_T4_TFTWIDTH - 1 - RADIUS, RADIUS, RADIUS, BLUE, RED);
                    break;
                }
            int j = (i + 2) % 4;
            delay(200);
            _calibTouch(x[j], y[j], z[j]);
            _printf("\n%d  %d  %d\n", x[j], y[j], z[j]);
            }

        clear(WHITE);  
        _pushRect(BLACK, 2, 93, 280, 318);
        _pushRect(GREY, 4, 91, 282, 316);
        _pushRect(BLACK, 97, 237, 280, 318);
        _pushRect(GREY, 99, 235, 282, 316);
        _uploadText("GOOD", 27, 305, GREEN, GREY, (void*)(&font_ILI9341_T4_OpenSans_Bold_14), true);
        _uploadText("REDO CALIB.", 130, 305, RED, GREY, (void*)(&font_ILI9341_T4_OpenSans_Bold_14), true);
        _uploadText("Draw below to test calibration", 22, 15, BLACK, WHITE, (void*)(&font_ILI9341_T4_OpenSans_Bold_14), true);

        float xa = (x[0] + x[3]) / 2.0f;
        float xb = (x[1] + x[2]) / 2.0f;
        float ya = (y[0] + y[1]) / 2.0f;
        float yb = (y[2] + y[3]) / 2.0f;
        
        const float xc = (xa + xb) / 2;
        const float ex = ((float)ILI9341_T4_TFTWIDTH) / (ILI9341_T4_TFTWIDTH - 2*RADIUS);
        xa = xc + (xa - xc) * ex;
        xb = xc + (xb - xc) * ex;

        const float yc = (ya + yb) / 2;
        const float ey = ((float)ILI9341_T4_TFTHEIGHT) / (ILI9341_T4_TFTHEIGHT - 2*RADIUS);
        ya = yc + (ya - yc) * ey;
        yb = yc + (yb - yc) * ey;

        int touch_calib[4];
        touch_calib[0] = (int)roundf(xa);
        touch_calib[1] = (int)roundf(xb);
        touch_calib[2] = (int)roundf(ya);
        touch_calib[3] = (int)roundf(yb);

        setTouchCalibration(touch_calib);
        if (touchCalibration)
            {
            for (int i = 0; i < 4; i++) touchCalibration[i] = touch_calib[i];
            }

        while (1)
            {
            delay(1);
            int x, y, z;
            if (readTouch(x, y, z))
                {
                if (y > 283)
                    {
                    if (x < 95)
                        {
                        // done !
                        _print("\n\nDONE !");
                        _printf("\n\nCalibration values = {%d, %d, %d, %d }\n\n", touch_calib[0], touch_calib[1], touch_calib[2], touch_calib[3]);
                        
                        clear(WHITE);

                        char tt[50] = { 0 };
                        sprintf(tt, "{ %i ,  %i ,  %i ,  %i }", touch_calib[0], touch_calib[1], touch_calib[2], touch_calib[3]);

                        int minx, maxx, miny, maxy;
                        _measureText(tt, 0, 0, minx, maxx, miny, maxy, (void*)(&font_ILI9341_T4_OpenSans_Bold_16), false);
                        _uploadText("Calibration data:", 60, 130, BLACK, WHITE, (void*)(&font_ILI9341_T4_OpenSans_Bold_16), true);
                        _uploadText(tt, 120 - (maxx - minx + 1)/2, 170, RED, WHITE, (void*)(&font_ILI9341_T4_OpenSans_Bold_16), true);
                        _pushRect(BLACK, 85, 155, 280, 318);
                        _pushRect(GREY, 87, 153, 282, 316);
                        _uploadText("QUIT", 102, 305, BLACK, GREY, (void*)(&font_ILI9341_T4_OpenSans_Bold_16), true);
                        delay(300);

                        while (1)
                            {
                            if (readTouch(x, y, z))
                                {
                                if ((y > 283) && (x > 87) && (x < 153))
                                    {                                    
                                    setRotation(oldrot);
                                    _mirrorfb = nullptr;
                                    _ongoingDiff = nullptr;
                                    resync();
                                    return;
                                    }
                                }
                            }
                        }
                    else
                        {                        
                        _print("\n\nrestarting calibration procedure...\n\n");
                        goto restart_calib;
                        }
                    }
                else
                    {
                    _pushRect(BLACK, x - 2, x + 2, y - 2, y + 2);
                    }
                }
            }
        }





}


/** end of file */

