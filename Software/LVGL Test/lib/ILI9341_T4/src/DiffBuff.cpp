#include "DiffBuff.h"



namespace ILI9341_T4
{

        void DiffBuffBase::rotationBox(int orientation, int xmin, int xmax, int ymin, int ymax, int& x1, int& x2, int& y1, int& y2)
            {
            switch (orientation)
                {
            case LANDSCAPE_320x240:
                x1 = DiffBuffBase::LX - 1 - ymax;
                x2 = DiffBuffBase::LX - 1 - ymin;
                y1 = xmin;
                y2 = xmax;
                break;
            case PORTRAIT_240x320_FLIPPED:
                x1 = DiffBuffBase::LX - 1 - xmax;
                x2 = DiffBuffBase::LX - 1 - xmin;
                y1 = DiffBuffBase::LY - 1 - ymax;
                y2 = DiffBuffBase::LY - 1 - ymin;
                break;
            case LANDSCAPE_320x240_FLIPPED:
                x1 = ymin;
                x2 = ymax;
                y1 = DiffBuffBase::LY - 1 - xmax;
                y2 = DiffBuffBase::LY - 1 - xmin;
                break;
            default: // case PORTRAIT_240x320:
                x1 = xmin;
                x2 = xmax;
                y1 = ymin;
                y2 = ymax;
                break;
                }
            }


        void DiffBuffBase::copyfb(uint16_t* fb_old, const uint16_t* fb_new, int fb_new_orientation)
            {
            switch (fb_new_orientation)
                {
                case PORTRAIT_240x320:
                    _copy_rotate_0(fb_old, fb_new);
                    return;
                case LANDSCAPE_320x240:
                    _copy_rotate_90(fb_old, fb_new);
                    return;
                case PORTRAIT_240x320_FLIPPED:
                    _copy_rotate_180(fb_old, fb_new);
                    return;
                case LANDSCAPE_320x240_FLIPPED:
                    _copy_rotate_270(fb_old, fb_new);
                    return;
                }
            // hum...
            return;
            }


        void DiffBuffBase::copyfb(uint16_t* fb_old, const uint16_t* fb_new, int xmin, int xmax, int ymin, int ymax, int src_stride, int fb_new_orientation)
            {
            int x1, x2, y1, y2;
            rotationBox(fb_new_orientation, xmin, xmax, ymin, ymax, x1, x2, y1, y2);
            const int w = x2 - x1 + 1;
            const int h = y2 - y1 + 1;
            switch (fb_new_orientation)
                {
                case PORTRAIT_240x320:
                    _copy_rotate_0(fb_old, fb_new, x1, x2, y1, y2, w, h, src_stride);
                    return;
                case LANDSCAPE_320x240:
                    _copy_rotate_90(fb_old, fb_new, x1, x2, y1, y2, w, h, src_stride);
                    return;
                case PORTRAIT_240x320_FLIPPED:
                    _copy_rotate_180(fb_old, fb_new, x1, x2, y1, y2, w, h, src_stride);
                    return;
                case LANDSCAPE_320x240_FLIPPED:
                    _copy_rotate_270(fb_old, fb_new, x1, x2, y1, y2, w, h, src_stride);
                    return;
                }
            // hum...
            return;
            }


    
        void DiffBuffBase::_copy_rotate_0(uint16_t* fb_dest, const uint16_t* fb_src)
            {
            memcpy(fb_dest, fb_src, sizeof(uint16_t) * DiffBuffBase::LX * DiffBuffBase::LY);
            }


        void DiffBuffBase::_copy_rotate_90(uint16_t* fb_dest, const uint16_t* fb_src)
            {
            uint16_t* p = fb_dest;
            for (int i = 0; i < DiffBuffBase::LY; i++)
                {
                int j = DiffBuffBase::LX - 1;
                while (j >= 0)
                    {
                    *(p++) = fb_src[i + DiffBuffBase::LY * (j--)];
                    *(p++) = fb_src[i + DiffBuffBase::LY * (j--)];
                    *(p++) = fb_src[i + DiffBuffBase::LY * (j--)];
                    *(p++) = fb_src[i + DiffBuffBase::LY * (j--)];
                    }
                }
            }


        void DiffBuffBase::_copy_rotate_180(uint16_t* fb_dest, const uint16_t* fb_src)
            {
            uint16_t* p = fb_dest;
            for (int j = DiffBuffBase::LY - 1; j >= 0; j--)
                {
                int i = DiffBuffBase::LX - 1;
                while (i >= 0)
                    {
                    const int oo = DiffBuffBase::LX * j;
                    *(p++) = fb_src[(i--) + oo];
                    *(p++) = fb_src[(i--) + oo];
                    *(p++) = fb_src[(i--) + oo];
                    *(p++) = fb_src[(i--) + oo];
                    }
                }
            }


        void DiffBuffBase::_copy_rotate_270(uint16_t* fb_dest, const uint16_t* fb_src)
            {
            uint16_t* p = fb_dest;
            for (int i = DiffBuffBase::LY - 1; i >= 0; i--)
                {
                int j = 0;
                while (j < DiffBuffBase::LX)
                    {
                    *(p++) = fb_src[i + DiffBuffBase::LY * (j++)];
                    *(p++) = fb_src[i + DiffBuffBase::LY * (j++)];
                    *(p++) = fb_src[i + DiffBuffBase::LY * (j++)];
                    *(p++) = fb_src[i + DiffBuffBase::LY * (j++)];
                    }
                }
            }


        void DiffBuffBase::_copy_rotate_0(uint16_t* fb_dest, const uint16_t* fb_src, int x1, int x2, int y1, int y2, int w, int h, int src_stride)
            {
            uint16_t* p = fb_dest + x1 + (DiffBuffBase::LX*y1);
            for (int j = 0; j < h; j++)
                {
                for (int i = 0; i < w; i++)
                    {
                    *(p++)= fb_src[i + (src_stride * j)];
                    }
                p += (DiffBuffBase::LX - w);
                }
            }


        void DiffBuffBase::_copy_rotate_90(uint16_t* fb_dest, const uint16_t* fb_src, int x1, int x2, int y1, int y2, int w, int h, int src_stride)
            {
            uint16_t* p = fb_dest + x1 + (DiffBuffBase::LX*y1);
            for (int j = 0; j < h; j++)
                {   
                for (int i = w-1; i >= 0 ; i--)                    
                    {
                    *(p++) = fb_src[j + (src_stride * i)];
                    }
                p += (DiffBuffBase::LX - w);
                }
            }
        

        void DiffBuffBase::_copy_rotate_180(uint16_t* fb_dest, const uint16_t* fb_src, int x1, int x2, int y1, int y2, int w, int h, int src_stride)
            {
            uint16_t* p = fb_dest + x1 + (DiffBuffBase::LX*y1);
            for (int j = h - 1; j >= 0; j--)
                {
                for (int i = w - 1; i >= 0; i--)
                    {
                    *(p++)= fb_src[i + (src_stride * j)];
                    }
                p += (DiffBuffBase::LX - w);
                }
            }


        void DiffBuffBase::_copy_rotate_270(uint16_t* fb_dest, const uint16_t* fb_src, int x1, int x2, int y1, int y2, int w, int h, int src_stride)
            {
            uint16_t* p = fb_dest + x1 + (DiffBuffBase::LX*y1);
            for (int j = h - 1; j >= 0; j--)
                {   
                for (int i = 0; i < w ; i++)   
                    {
                    *(p++) = fb_src[j + (src_stride * i)];
                    }
                p += (DiffBuffBase::LX - w);
                }
            }


        template<bool COPY_NEW_OVER_OLD, bool USE_MASK>
        void DiffBuff::_computeDiff(uint16_t* fb_old, const uint16_t* fb_new, int fb_new_orientation, int gap, uint16_t compare_mask)
            {
            switch (fb_new_orientation)
                {
                case PORTRAIT_240x320:
                    _computeDiff0<COPY_NEW_OVER_OLD, USE_MASK>(fb_old, fb_new, gap, compare_mask);
                    return;
                case LANDSCAPE_320x240:
                    _computeDiff1<COPY_NEW_OVER_OLD, USE_MASK>(fb_old, fb_new, gap, compare_mask);
                    return;
                case PORTRAIT_240x320_FLIPPED:
                    _computeDiff2<COPY_NEW_OVER_OLD, USE_MASK>(fb_old, fb_new, gap, compare_mask);
                    return;
                case LANDSCAPE_320x240_FLIPPED:
                    _computeDiff3<COPY_NEW_OVER_OLD, USE_MASK>(fb_old, fb_new, gap, compare_mask);
                    return;
                }
            // hum...
            return;
            }


       
        
#define COMPUTE_DIFF_LOOP_SUB            {                                                       \
                                         if (COPY_NEW_OVER_OLD) { fb_old[n] = fb_new[ind]; }     \
                                         if (cgap >= gap)                                        \
                                             {                                                   \
                                             if (!_write_chunk(n - pos - cgap, cgap)) return;    \
                                             pos = n;                                            \
                                             }                                                   \
                                         cgap = 0;                                               \
                                         }


#define COMPUTE_DIFF_LOOP_MASK(INDEX)    {                                                       \
                                         const int ind = (INDEX);                                \
                                         if (((fb_old[n]) ^ (fb_new[ind])) & compare_mask)       \
                                             COMPUTE_DIFF_LOOP_SUB                               \
                                         else { cgap++; }                                        \
                                         n++;                                                    \
                                         }

#define COMPUTE_DIFF_LOOP_NOMASK(INDEX)  {                                                       \
                                         const int ind = (INDEX);                                \
                                         if (fb_old[n] != fb_new[ind])                           \
                                             COMPUTE_DIFF_LOOP_SUB                               \
                                         else { cgap++; }                                        \
                                         n++;                                                    \
                                         }


#define COMPUTE_DIFF_LOOP(INDEX)    {                                       \
                                    if (USE_MASK)                           \
                                        {                                   \
                                        COMPUTE_DIFF_LOOP_MASK(INDEX)       \
                                        COMPUTE_DIFF_LOOP_MASK(INDEX)       \
                                        }                                   \
                                    else                                    \
                                        {                                   \
                                        COMPUTE_DIFF_LOOP_NOMASK(INDEX)     \
                                        COMPUTE_DIFF_LOOP_NOMASK(INDEX)     \
                                        }                                   \
                                    }


#define COMPUTE_DIFF_END    { const int cpos = DiffBuffBase::LX * DiffBuffBase::LY;                   \
                              if (cpos - pos - cgap != 0)                 \
                                  {                                       \
                                  _write_chunk(cpos - pos - cgap, cgap);  \
                                  }                                       \
                            }

                            


        template<bool COPY_NEW_OVER_OLD, bool USE_MASK>
        void DiffBuff::_computeDiff0(uint16_t* fb_old, const uint16_t* fb_new, int gap, uint16_t compare_mask)
            {
            int cgap = 0;   // current gap size;
            int pos = 0;    // number of pixel written in diffbuf
            int n = 0;      // current offset  
            int m = 0; 
            while(m < DiffBuffBase::LX*DiffBuffBase::LY)
                {
                COMPUTE_DIFF_LOOP((m++)) 
                }
            COMPUTE_DIFF_END
            }

        template<bool COPY_NEW_OVER_OLD, bool USE_MASK>
        void DiffBuff::_computeDiff1(uint16_t* fb_old, const uint16_t* fb_new, int gap, uint16_t compare_mask)
            {
            int cgap = 0;   // current gap size;
            int pos = 0;    // number of pixel written in diffbuf
            int n = 0;      // current offset  
            for (int i = 0; i < DiffBuffBase::LY; i++)
                {
                int j = DiffBuffBase::LX - 1;
                while (j >= 0)
                    {
                    COMPUTE_DIFF_LOOP((i + DiffBuffBase::LY * (j--)))
                    }
                }
            COMPUTE_DIFF_END
            }


        template<bool COPY_NEW_OVER_OLD, bool USE_MASK>
        void DiffBuff::_computeDiff2(uint16_t* fb_old, const uint16_t* fb_new, int gap, uint16_t compare_mask)
            {
            int cgap = 0;   // current gap size;
            int pos = 0;    // number of pixel written in diffbuf
            int n = 0;      // current offset  
            for (int j = DiffBuffBase::LY - 1; j >= 0; j--)
                {
                int i = DiffBuffBase::LX - 1;
                while (i >= 0)
                    {
                    const int oo = DiffBuffBase::LX * j;
                    COMPUTE_DIFF_LOOP(((i--) + oo))
                    }
                }
            COMPUTE_DIFF_END
            }


        template<bool COPY_NEW_OVER_OLD, bool USE_MASK>
        void DiffBuff::_computeDiff3(uint16_t* fb_old, const uint16_t* fb_new, int gap, uint16_t compare_mask)
            {
            int cgap = 0;   // current gap size;
            int pos = 0;    // number of pixel written in diffbuf
            int n = 0;      // current offset  
            for (int i = DiffBuffBase::LY - 1; i >= 0; i--)
                {
                int j = 0;
                while (j < DiffBuffBase::LX)
                    {
                    COMPUTE_DIFF_LOOP((i + DiffBuffBase::LY * (j++)))
                    }
                }
            COMPUTE_DIFF_END
            }


#undef COMPUTE_DIFF_LOOP_SUB
#undef COMPUTE_DIFF_LOOP_MASK
#undef COMPUTE_DIFF_LOOP_NOMASK
#undef COMPUTE_DIFF_LOOP
#undef COMPUTE_DIFF_END


        void DiffBuff::computeDiff(uint16_t* fb_old, const uint16_t* fb_new, int fb_new_orientation, int gap, bool copy_new_over_old, uint16_t compare_mask)
            {
            elapsedMicros em; // for stats. 
            if (gap < 1) gap = 1;
            if ((fb_new_orientation < 0) || (fb_new_orientation > 3)) fb_new_orientation = 0;
            _posw = 0; // reset buffer
            if ((_sizebuf <= 0) || (fb_old == nullptr) || (fb_new == nullptr))
                {
                _write_encoded(TAG_END);
                _posw = 0;
//                initRead();
                return;
                }
            if ((compare_mask != 0) && (compare_mask != 0xffff))
                {
                if (copy_new_over_old) 
                    _computeDiff<true, true>(fb_old, fb_new, fb_new_orientation, gap, compare_mask);
                else
                    _computeDiff<false, true>(fb_old, fb_new, fb_new_orientation, gap, compare_mask);
                }
            else
                {
                if (copy_new_over_old)
                    _computeDiff<true, false>(fb_old, fb_new, fb_new_orientation, gap, compare_mask);
                else
                    _computeDiff<false, false>(fb_old, fb_new, fb_new_orientation, gap, compare_mask);
                }

            _write_encoded(TAG_END);
            if ((unsigned int)size() >= (unsigned int)_sizebuf)
                { // diff is full so copy from new to old may not have been completed...
                if (copy_new_over_old) copyfb(fb_old, fb_new, fb_new_orientation); // copy again. 
                }
//            initRead();
            // done. record stats
            _stats_size.push(size());
            if ((unsigned int)size() >= (unsigned int)_sizebuf) _stat_overflow++;
            _stats_time.push(em);
            }


        int DiffBuff::readDiff(int& x, int& y, int& len, int scanline)
            {
            if (!_r_cont)
                { // we must load a new instruction. 
                int nb_write, nb_skip;
                while(1)
                    {
                    nb_write = _read_encoded(_posr);         // number of pixel to write
                    if (nb_write == TAG_END) return -1; // done !
                    if (nb_write == TAG_WRITE_ALL)
                        { // must write everything
                        nb_write = DiffBuffBase::LX * DiffBuffBase::LY - _off;
                        nb_skip = 0;
                        if (nb_write <= 0) return -1;
                        }
                    else
                        {
                        nb_skip = _read_encoded(_posr);      // number of pixels to skip
                        }
                    if (nb_write > 0) break;
                    _off += nb_skip;
                    }
                _r_y = _off / DiffBuffBase::LX;
                _r_x = _off - (DiffBuffBase::LX * _r_y);
                _off += nb_skip + nb_write;
                _r_len = nb_write;
                _r_cont = true;
                }            
            // we have a valid instruction in _r_x, _r_y, _r_len and _r_cont=true            
            x = _r_x;
            y = _r_y;
            if ((scanline < DiffBuffBase::LY) && (_r_y + MIN_SCANLINE_SPACE > scanline))
                { // we must wait a bit.
                len = 0;
                const int l = _r_y + MIN_SCANLINE_SPACE;
                return ((l < DiffBuffBase::LY) ? l : DiffBuffBase::LY);
                }
            if (_r_x > 0)
                { // not at the beginning of a line. 
                if (_r_x + _r_len <= DiffBuffBase::LX)
                    { // everything fits on the line
                    len = _r_len;
                    _r_cont = false;
                    return 0;
                    }
                len = DiffBuffBase::LX - _r_x;
                _r_len -= len; 
                _r_x = 0;
                _r_y++;
                return 0;
                }
            // at the beginning of a line 
            int maxl = scanline - _r_y; // max number of lines available now
            if (maxl > MAX_WRITE_LINE) maxl = MAX_WRITE_LINE; // clamp at max value. 
            const int nbw = maxl * DiffBuffBase::LX; // max number of pixels that we can write 
            if (_r_len <= nbw)
                { // ok, we can write everything now
                len = _r_len;
                _r_cont = false;
                return 0;
                }
            // cannot write everything yet. 
            len = nbw;
            _r_len -= nbw;
            _r_x = 0;
            _r_y += maxl;
            return 0;
            }


        void DiffBuff::computeDiff(uint16_t* fb_old, DiffBuffBase* diff_old, const uint16_t* sub_fb_new, int xmin, int xmax, int ymin, int ymax, int stride,
                int fb_new_orientation, int gap, bool copy_new_over_old, uint16_t compare_mask)
            {
            elapsedMicros em; // for stats. 
            if (gap < 1) gap = 1;
            if ((fb_new_orientation < 0) || (fb_new_orientation > 3)) fb_new_orientation = 0;
            _posw = 0; // reset buffer
            if ((_sizebuf <= 0) || (fb_old == nullptr) || (sub_fb_new == nullptr))
                {
                _write_encoded(TAG_END);
                _posw = 0;
//                initRead();
                return;
                }

            int x1, x2, y1, y2;
            DiffBuffBase::rotationBox(fb_new_orientation, xmin, xmax, ymin, ymax, x1, x2, y1, y2);

            _computeDiff(fb_old, diff_old, sub_fb_new, x1, x2, y1, y2, stride, fb_new_orientation, gap, copy_new_over_old, compare_mask);

            _write_encoded(TAG_END);
            if ((unsigned int)size() >= (unsigned int)_sizebuf)
                { // diff is full so copy from new to old may not have been completed...
                if (copy_new_over_old) copyfb(fb_old, sub_fb_new, xmin, xmax, ymin, ymax, stride, fb_new_orientation); // copy again. 
                }
            // done. record stats
//            initRead();
            _stats_size.push(size());
            if ((unsigned int)size() >= (unsigned int)_sizebuf) _stat_overflow++;
            _stats_time.push(em);
            }


        void DiffBuff::_computeDiff(uint16_t* fb_old, DiffBuffBase* diff_old, const uint16_t* sub_fb_new, int xmin, int xmax, int ymin, int ymax, int stride,
            int fb_new_orientation, int gap, bool copy_new_over_old, uint16_t compare_mask)
            {
            DiffBuffDummy dd; 
            if (diff_old)
                {
                diff_old->initRaw();
                }
            else
                {
                dd.setRawEmpty();
                diff_old = &dd; // set a dummy diff if none provided.                    
                }
 
            if (compare_mask == 0) compare_mask = 0xFFFF;
            int nb_write = 0, nb_skip = 0; // number of pixel to write / skip in the old diff
            diff_old->readRaw(nb_write, nb_skip);

            int prv = 0;   // last position written in the diff log
            int cgap = 0; // current gap

                {
                int cur = 0;   // current position
                int targetpos = DiffBuffBase::LX * ymin; // target to reach
                while (cur < targetpos)
                    {
                    if (nb_write > 0)
                        {
                        if (cgap >= gap)
                            {
                            if (!_write_chunk(cur - prv - cgap, cgap)) return;
                            prv = cur;
                            }
                        cgap = 0;
                        const int r = targetpos - cur;
                        if (nb_write < r)
                            {
                            cur += nb_write;
                            nb_write = 0;
                            }
                        else
                            {
                            nb_write -= r;
                            cur = targetpos;
                            }
                        }
                    else if (nb_skip > 0)
                        {
                        const int r = targetpos - cur;
                        if (nb_skip < r)
                            {
                            cur += nb_skip;
                            cgap += nb_skip;
                            nb_skip = 0;
                            }
                        else
                            {
                            nb_skip -= r;
                            cgap += r;
                            cur = targetpos;
                            }
                        }
                    else diff_old->readRaw(nb_write, nb_skip);
                    }
                }

            // We are at position (0, ymin)
            for (int yc = ymin; yc <= ymax; yc++)
                { 
                int xc = 0;
                while (xc < xmin)
                    {
                    if (nb_write > 0)
                        {
                        if (cgap >= gap)
                            {
                            const int cur = xc + (DiffBuffBase::LX * yc);
                            if (!_write_chunk(cur - prv - cgap, cgap)) return;
                            prv = cur;
                            }
                        cgap = 0;
                        const int r = xmin - xc;
                        if (nb_write < r)
                            {
                            xc += nb_write;
                            nb_write = 0;
                            }
                        else
                            {
                            nb_write -= r;
                            xc = xmin;
                            }
                        }
                    else if (nb_skip > 0)
                        {
                        const int r = xmin - xc;
                        if (nb_skip < r)
                            {
                            xc += nb_skip;
                            cgap += nb_skip;
                            nb_skip = 0;
                            }
                        else
                            {
                            nb_skip -= r;
                            cgap += r;
                            xc = xmin;
                            }
                        }
                    else diff_old->readRaw(nb_write, nb_skip);                    
                    }

                int m = 0, mdelta = 0;
                switch (fb_new_orientation)
                    {
                case PORTRAIT_240x320:
                    m = stride * (yc - ymin);
                    mdelta = 1;
                    break;
                case LANDSCAPE_320x240:
                    m = (yc - ymin) + stride*(xmax - xmin);
                    mdelta =  -stride;
                    break;
                case PORTRAIT_240x320_FLIPPED:
                    m = stride * (ymax - yc) + (xmax - xmin);
                    mdelta = -1;
                    break;
                case LANDSCAPE_320x240_FLIPPED:
                    m = ymax - yc;
                    mdelta = stride;
                    break;
                    }

                if (nb_write + nb_skip == 0) diff_old->readRaw(nb_write, nb_skip); // reload if needed

                const int nend = xmax + (DiffBuffBase::LX * yc);
                for (int n = xmin + (DiffBuffBase::LX * yc); n <= nend; n++, m += mdelta)
                    {
                    if (((fb_old[n]) ^ (sub_fb_new[m])) & compare_mask)
                        {
                        if (copy_new_over_old) 
                            { 
                            fb_old[n] = sub_fb_new[m]; 
                            }
                        if (cgap >= gap)
                            {
                            if (!_write_chunk(n - prv - cgap, cgap)) return;
                            prv = n;
                            }
                        cgap = 0;
                        if (nb_write > 0) nb_write--;
                        else
                            {
                            nb_skip--;
                            if (nb_skip == 0) diff_old->readRaw(nb_write, nb_skip); // reload if needed
                            }
                        }
                    else if (nb_write > 0)
                        { // same pixel, nb_write > 0
                        if (cgap >= gap)
                            {
                            if (!_write_chunk(n - prv - cgap, cgap)) return;
                            prv = n;
                            }
                        cgap = 0;
                        nb_write--;
                        }
                    else
                        { // same pixel, nb_write=0 and nb_skip>0
                        cgap++;
                        nb_skip--;
                        if (nb_skip == 0) diff_old->readRaw(nb_write, nb_skip); // reload if needed
                        }
                    }

                xc = xmax + 1;  
                while (xc < DiffBuffBase::LX)
                    { 
                    if (nb_write > 0)
                        {
                        if (cgap >= gap)
                            {
                            const int cur = xc + (DiffBuffBase::LX * yc);
                            if (!_write_chunk(cur - prv - cgap, cgap)) return;
                            prv = cur;
                            }
                        cgap = 0;
                        const int r = DiffBuffBase::LX - xc;
                        if (nb_write < r)
                            {
                            xc += nb_write;
                            nb_write = 0;
                            }
                        else
                            {
                            nb_write -= r;
                            xc = DiffBuffBase::LX;
                            }
                        }
                    else if (nb_skip > 0)
                        {
                        const int r = DiffBuffBase::LX - xc;
                        if (nb_skip < r)
                            {
                            xc += nb_skip;
                            cgap += nb_skip;
                            nb_skip = 0;
                            }
                        else
                            {
                            nb_skip -= r;
                            cgap += r;
                            xc = DiffBuffBase::LX;
                            }
                        }
                    else diff_old->readRaw(nb_write, nb_skip);
                    }
                }

                {
                int cur = DiffBuffBase::LX * (ymax + 1);
                int targetpos = DiffBuffBase::LX * DiffBuffBase::LY; // target to reach
                while (cur < targetpos)
                    {
                    if (nb_write > 0)
                        {
                        if (cgap >= gap)
                            {
                            if (!_write_chunk(cur - prv - cgap, cgap)) return;
                            prv = cur;
                            }
                        cgap = 0;
                        const int r = targetpos - cur;
                        if (nb_write < r)
                            {
                            cur += nb_write;
                            nb_write = 0;
                            }
                        else
                            {
                            nb_write -= r;
                            cur = targetpos;
                            }
                        }
                    else if (nb_skip > 0)
                        {
                        const int r = targetpos - cur;
                        if (nb_skip < r)
                            {
                            cur += nb_skip;
                            cgap += nb_skip;
                            nb_skip = 0;
                            }
                        else
                            {
                            nb_skip -= r;
                            cgap += r;
                            cur = targetpos;
                            }
                        }
                    else diff_old->readRaw(nb_write, nb_skip);
                    }
                }

            // complete the diff with the remaining part. 
            const int cpos = DiffBuffBase::LX * DiffBuffBase::LY;
            if (cpos - prv - cgap != 0)
                {                                       
                _write_chunk(cpos - prv - cgap, cgap);
                }

            }


        void DiffBuff::statsReset()
            {
            _stat_overflow = 0;
            _stats_size.reset();
            _stats_time.reset();
            }


        void DiffBuff::printStats(Stream* outputStream) const
            {
            if (outputStream)
                {
                outputStream->printf("------------------- DiffBuff Stats -------------------\n");
                outputStream->printf("- max. buffer size   : %u\n", _sizebuf + PADDING);
                outputStream->printf("- overflow ratio     : %.1f%%  (%u out of %u computed)\n", 100 * statsOverflowRatio(), statsNbOverflow(), statsNbComputed());
                outputStream->printf("- buffer size used   : "); _stats_size.print("", "\n", outputStream);
                outputStream->printf("- computation time   : "); _stats_time.print("us", "\n\n", outputStream);
                }
            }




        int DiffBuffDummy::readDiff(int& x, int& y, int& len, int scanline)
            {
            if (_current_line >= _end) return -1; // we are done. 
            if (scanline >= _end)
                { // scanline after end of drawing, go as fast as possible. 
                x = 0;
                y = _current_line;
                if (_current_line + MAX_WRITE_LINE <= _end)
                    {
                    len = MAX_WRITE_LINE * DiffBuffBase::LX;
                    _current_line += MAX_WRITE_LINE;
                    }
                else
                    {
                    len = (_end - _current_line) * DiffBuffBase::LX;
                    _current_line = _end;
                    }
                return 0;
                }
            int maxl = scanline - _current_line; // number of line available for drawing. 
            if (maxl < MIN_SCANLINE_SPACE)
                { // we must wait a bit. 
                const int l = _current_line + MIN_SCANLINE_SPACE;
                return ((l < _end) ? l : _end);
                }
            x = 0;
            y = _current_line;
            if (maxl > MAX_WRITE_LINE) maxl = MAX_WRITE_LINE; // not too much lines at once
            len = maxl * DiffBuffBase::LX;
            _current_line += maxl;
            return 0;
            }




}



/** end of file */

