

#include "rockmacros.h"
#include "defs.h"
#include "regs.h"
#include "hw.h"
#include "mem.h"
#include "lcd-gb.h"
#include "rc.h"
#include "fb.h"
#include "palette.h"
#ifdef USE_ASM
#include "asm.h"
#endif

struct lcd lcd;

struct scan scan IBSS_ATTR;

#define BG (scan.bg)
#define WND (scan.wnd)

#if LCD_DEPTH ==16
#define BUF (scan.buf)
#else
#define BUF (scan.buf[scanline_ind])
#endif

#define PRI (scan.pri)

#define PAL1 (scan.pal1)
#define PAL2 (scan.pal2)
#define PAL4 (scan.pal4)

#define VS (scan.vs) /* vissprites */
#define NS (scan.ns)

#define L (scan.l) /* line */
#define X (scan.x) /* screen position */
#define Y (scan.y)
#define S (scan.s) /* tilemap position */
#define T (scan.t)
#define U (scan.u) /* position within tile */
#define V (scan.v)
#define WX (scan.wx)
#define WY (scan.wy)
#define WT (scan.wt)
#define WV (scan.wv)

byte patpix[4096][8][8]
#if defined(CPU_COLDFIRE) && !defined(SIMULATOR)
     __attribute__ ((aligned(16))) /* to profit from burst mode */
#endif
     ;
byte patdirty[1024];
byte anydirty;

// static int scale = 1;

static int rgb332;

static int sprsort = 1;
static int sprdebug;
static int insync=0;
#if LCD_DEPTH < 16
static int scanline_ind=0;
#endif

#define DEF_PAL { 0x98d0e0, 0x68a0b0, 0x60707C, 0x2C3C3C }

static int dmg_pal[4][4] = { DEF_PAL, DEF_PAL, DEF_PAL, DEF_PAL };

static int usefilter, filterdmg;
static int filter[3][4] = {
                              { 195,  25,   0,  35 },
                              {  25, 170,  25,  35 },
                              {  25,  60, 125,  40 }
                          };

rcvar_t lcd_exports[] =
    {
        RCV_BOOL("rgb332", &rgb332),
        RCV_VECTOR("dmg_bgp", dmg_pal[0], 4),
        RCV_VECTOR("dmg_wndp", dmg_pal[1], 4),
        RCV_VECTOR("dmg_obp0", dmg_pal[2], 4),
        RCV_VECTOR("dmg_obp1", dmg_pal[3], 4),
        RCV_BOOL("sprsort", &sprsort),
        RCV_BOOL("sprdebug", &sprdebug),
        RCV_BOOL("colorfilter", &usefilter),
        RCV_BOOL("filterdmg", &filterdmg),
        RCV_VECTOR("red", filter[0], 4),
        RCV_VECTOR("green", filter[1], 4),
        RCV_VECTOR("blue", filter[2], 4),
        RCV_END
    };

fb_data *vdest;

#ifdef ALLOW_UNALIGNED_IO /* long long is ok since this is i386-only anyway? */
#define MEMCPY8(d, s) ((*(long long *)(d)) = (*(long long *)(s)))
#else
#define MEMCPY8(d, s) memcpy((d), (s), 8)
#endif




#ifndef ASM_UPDATEPATPIX
void updatepatpix(void)
{
    int i, j;
#if ((CONFIG_CPU != SH7034) && !defined(CPU_COLDFIRE)) || defined(SIMULATOR)
    int k, a, c;
#endif
    byte *vram = lcd.vbank[0];

    if (!anydirty) return;
    for (i = 0; i < 1024; i++)
    {
        if (i == 384) i = 512;
        if (i == 896) break;
        if (!patdirty[i]) continue;
        patdirty[i] = 0;
        for (j = 0; j < 8; j++)
        {
#if CONFIG_CPU == SH7034 && !defined(SIMULATOR)
            asm volatile (
                "mov.w   @%2,r1         \n"
                "swap.b  r1,r2          \n"

                "mov     #0,r0          \n"
                "shlr    r1             \n"
                "rotcl   r0             \n"
                "shlr    r2             \n"
                "rotcl   r0             \n"
                "mov.b   r0,@%0         \n"
                "mov.b   r0,@(7,%1)     \n"
                "mov     #0,r0          \n"
                "shlr    r1             \n"
                "rotcl   r0             \n"
                "shlr    r2             \n"
                "rotcl   r0             \n"
                "mov.b   r0,@(1,%0)     \n"
                "mov.b   r0,@(6,%1)     \n"
                "mov     #0,r0          \n"
                "shlr    r1             \n"
                "rotcl   r0             \n"
                "shlr    r2             \n"
                "rotcl   r0             \n"
                "mov.b   r0,@(2,%0)     \n"
                "mov.b   r0,@(5,%1)     \n"
                "mov     #0,r0          \n"
                "shlr    r1             \n"
                "rotcl   r0             \n"
                "shlr    r2             \n"
                "rotcl   r0             \n"
                "mov.b   r0,@(3,%0)     \n"
                "mov.b   r0,@(4,%1)     \n"
                "mov     #0,r0          \n"
                "shlr    r1             \n"
                "rotcl   r0             \n"
                "shlr    r2             \n"
                "rotcl   r0             \n"
                "mov.b   r0,@(4,%0)     \n"
                "mov.b   r0,@(3,%1)     \n"
                "mov     #0,r0          \n"
                "shlr    r1             \n"
                "rotcl   r0             \n"
                "shlr    r2             \n"
                "rotcl   r0             \n"
                "mov.b   r0,@(5,%0)     \n"
                "mov.b   r0,@(2,%1)     \n"
                "mov     #0,r0          \n"
                "shlr    r1             \n"
                "rotcl   r0             \n"
                "shlr    r2             \n"
                "rotcl   r0             \n"
                "mov.b   r0,@(6,%0)     \n"
                "mov.b   r0,@(1,%1)     \n"
                "mov     #0,r0          \n"
                "shlr    r1             \n"
                "rotcl   r0             \n"
                "shlr    r2             \n"
                "rotcl   r0             \n"
                "mov.b   r0,@(7,%0)     \n"
                "mov.b   r0,@%1         \n"
                : /* outputs */
                : /* inputs */
                /* %0 */ "r"(patpix[i+1024][j]),
                /* %1 */ "r"(patpix[i][j]),
                /* %2 */ "r"(&vram[(i<<4)|(j<<1)])
                : /* clobbers */
                "r0", "r1", "r2"
            );
#elif defined(CPU_COLDFIRE) && !defined(SIMULATOR)
            asm volatile (
                "move.b  (%2),%%d2      \n"
                "move.b  (1,%2),%%d1    \n"

                "addq.l  #8,%1          \n"
                "clr.l   %%d0           \n"
                "lsr.l   #1,%%d1        \n"
                "addx.l  %%d0,%%d0      \n"
                "lsr.l   #1,%%d2        \n"
                "addx.l  %%d0,%%d0      \n"
                "move.b  %%d0,-(%1)     \n"
                "lsl.l   #6,%%d0        \n"
                "lsr.l   #1,%%d1        \n"
                "addx.l  %%d0,%%d0      \n"
                "lsr.l   #1,%%d2        \n"
                "addx.l  %%d0,%%d0      \n"
                "move.b  %%d0,-(%1)     \n"
                "lsl.l   #6,%%d0        \n"
                "lsr.l   #1,%%d1        \n"
                "addx.l  %%d0,%%d0      \n"
                "lsr.l   #1,%%d2        \n"
                "addx.l  %%d0,%%d0      \n"
                "move.b  %%d0,-(%1)     \n"
                "lsl.l   #6,%%d0        \n"
                "lsr.l   #1,%%d1        \n"
                "addx.l  %%d0,%%d0      \n"
                "lsr.l   #1,%%d2        \n"
                "addx.l  %%d0,%%d0      \n"
                "move.l  %%d0,(%0)      \n"
                "move.b  %%d0,-(%1)     \n"
                "clr.l   %%d0           \n"
                "lsr.l   #1,%%d1        \n"
                "addx.l  %%d0,%%d0      \n"
                "lsr.l   #1,%%d2        \n"
                "addx.l  %%d0,%%d0      \n"
                "move.b  %%d0,-(%1)     \n"
                "lsl.l   #6,%%d0        \n"
                "lsr.l   #1,%%d1        \n"
                "addx.l  %%d0,%%d0      \n"
                "lsr.l   #1,%%d2        \n"
                "addx.l  %%d0,%%d0      \n"
                "move.b  %%d0,-(%1)     \n"
                "lsl.l   #6,%%d0        \n"
                "lsr.l   #1,%%d1        \n"
                "addx.l  %%d0,%%d0      \n"
                "lsr.l   #1,%%d2        \n"
                "addx.l  %%d0,%%d0      \n"
                "move.b  %%d0,-(%1)     \n"
                "lsl.l   #6,%%d0        \n"
                "lsr.l   #1,%%d1        \n"
                "addx.l  %%d0,%%d0      \n"
                "lsr.l   #1,%%d2        \n"
                "addx.l  %%d0,%%d0      \n"
                "move.l  %%d0,(4,%0)    \n"
                "move.b  %%d0,-(%1)     \n"
                : /* outputs */
                : /* inputs */
                /* %0 */ "a"(patpix[i+1024][j]),
                /* %1 */ "a"(patpix[i][j]),
                /* %2 */ "a"(&vram[(i<<4)|(j<<1)])
                : /* clobbers */
                "d0", "d1", "d2"
            );
#else
            a = ((i<<4) | (j<<1));
            for (k = 0; k < 8; k++)
            {
                c = vram[a] & (1<<k) ? 1 : 0;
                c |= vram[a+1] & (1<<k) ? 2 : 0;
                patpix[i+1024][j][k] = c;
            }
            for (k = 0; k < 8; k++)
                patpix[i][j][k] =
                    patpix[i+1024][j][7-k];
#endif
        }
#if CONFIG_CPU == SH7034 && !defined(SIMULATOR)
        asm volatile (
            "mov.l   @%0,r0         \n"
            "mov.l   @(4,%0),r1     \n"
            "mov.l   r0,@(56,%1)    \n"
            "mov.l   r1,@(60,%1)    \n"
            "mov.l   @(8,%0),r0     \n"
            "mov.l   @(12,%0),r1    \n"
            "mov.l   r0,@(48,%1)    \n"
            "mov.l   r1,@(52,%1)    \n"
            "mov.l   @(16,%0),r0    \n"
            "mov.l   @(20,%0),r1    \n"
            "mov.l   r0,@(40,%1)    \n"
            "mov.l   r1,@(44,%1)    \n"
            "mov.l   @(24,%0),r0    \n"
            "mov.l   @(28,%0),r1    \n"
            "mov.l   r0,@(32,%1)    \n"
            "mov.l   r1,@(36,%1)    \n"
            "mov.l   @(32,%0),r0    \n"
            "mov.l   @(36,%0),r1    \n"
            "mov.l   r0,@(24,%1)    \n"
            "mov.l   r1,@(28,%1)    \n"
            "mov.l   @(40,%0),r0    \n"
            "mov.l   @(44,%0),r1    \n"
            "mov.l   r0,@(16,%1)    \n"
            "mov.l   r1,@(20,%1)    \n"
            "mov.l   @(48,%0),r0    \n"
            "mov.l   @(52,%0),r1    \n"
            "mov.l   r0,@(8,%1)     \n"
            "mov.l   r1,@(12,%1)    \n"
            "mov.l   @(56,%0),r0    \n"
            "mov.l   @(60,%0),r1    \n"
            "mov.l   r0,@%1         \n"
            "mov.l   r1,@(4,%1)     \n"

            "add     %2,%0          \n"
            "add     %2,%1          \n"

            "mov.l   @%0,r0         \n"
            "mov.l   @(4,%0),r1     \n"
            "mov.l   r0,@(56,%1)    \n"
            "mov.l   r1,@(60,%1)    \n"
            "mov.l   @(8,%0),r0     \n"
            "mov.l   @(12,%0),r1    \n"
            "mov.l   r0,@(48,%1)    \n"
            "mov.l   r1,@(52,%1)    \n"
            "mov.l   @(16,%0),r0    \n"
            "mov.l   @(20,%0),r1    \n"
            "mov.l   r0,@(40,%1)    \n"
            "mov.l   r1,@(44,%1)    \n"
            "mov.l   @(24,%0),r0    \n"
            "mov.l   @(28,%0),r1    \n"
            "mov.l   r0,@(32,%1)    \n"
            "mov.l   r1,@(36,%1)    \n"
            "mov.l   @(32,%0),r0    \n"
            "mov.l   @(36,%0),r1    \n"
            "mov.l   r0,@(24,%1)    \n"
            "mov.l   r1,@(28,%1)    \n"
            "mov.l   @(40,%0),r0    \n"
            "mov.l   @(44,%0),r1    \n"
            "mov.l   r0,@(16,%1)    \n"
            "mov.l   r1,@(20,%1)    \n"
            "mov.l   @(48,%0),r0    \n"
            "mov.l   @(52,%0),r1    \n"
            "mov.l   r0,@(8,%1)     \n"
            "mov.l   r1,@(12,%1)    \n"
            "mov.l   @(56,%0),r0    \n"
            "mov.l   @(60,%0),r1    \n"
            "mov.l   r0,@%1         \n"
            "mov.l   r1,@(4,%1)     \n"
            : /* outputs */
            : /* inputs */
            /* %0 */ "r"(patpix[i][0]),
            /* %1 */ "r"(patpix[i+2048][0]),
            /* %2 */ "r"(1024*64)
            : /* clobbers */
            "r0", "r1"
        );
#elif defined(CPU_COLDFIRE) && !defined(SIMULATOR)
        asm volatile (
            "movem.l (%0),%%d0-%%d3     \n"
            "move.l  %%d0,%%d4          \n"
            "move.l  %%d1,%%d5          \n"
            "movem.l %%d2-%%d5,(48,%1)  \n"
            "movem.l (16,%0),%%d0-%%d3  \n"
            "move.l  %%d0,%%d4          \n"
            "move.l  %%d1,%%d5          \n"
            "movem.l %%d2-%%d5,(32,%1)  \n"
            "movem.l (32,%0),%%d0-%%d3  \n"
            "move.l  %%d0,%%d4          \n"
            "move.l  %%d1,%%d5          \n"
            "movem.l %%d2-%%d5,(16,%1)  \n"
            "movem.l (48,%0),%%d0-%%d3  \n"
            "move.l  %%d0,%%d4          \n"
            "move.l  %%d1,%%d5          \n"
            "movem.l %%d2-%%d5,(%1)     \n"

            "move.l  %2,%%d0            \n"
            "add.l   %%d0,%0            \n"
            "add.l   %%d0,%1            \n"

            "movem.l (%0),%%d0-%%d3     \n"
            "move.l  %%d0,%%d4          \n"
            "move.l  %%d1,%%d5          \n"
            "movem.l %%d2-%%d5,(48,%1)  \n"
            "movem.l (16,%0),%%d0-%%d3  \n"
            "move.l  %%d0,%%d4          \n"
            "move.l  %%d1,%%d5          \n"
            "movem.l %%d2-%%d5,(32,%1)  \n"
            "movem.l (32,%0),%%d0-%%d3  \n"
            "move.l  %%d0,%%d4          \n"
            "move.l  %%d1,%%d5          \n"
            "movem.l %%d2-%%d5,(16,%1)  \n"
            "movem.l (48,%0),%%d0-%%d3  \n"
            "move.l  %%d0,%%d4          \n"
            "move.l  %%d1,%%d5          \n"
            "movem.l %%d2-%%d5,(%1)     \n"
            : /* outputs */
            : /* inputs */
            /* %0 */ "a"(patpix[i][0]),
            /* %1 */ "a"(patpix[i+2048][0]),
            /* %2 */ "i"(1024*64)
            : /* clobbers */
            "d0", "d1", "d2", "d3", "d4", "d5"
        );
#else
        for (j = 0; j < 8; j++)
        {
            for (k = 0; k < 8; k++)
            {
                patpix[i+2048][j][k] =
                    patpix[i][7-j][k];
                patpix[i+3072][j][k] =
                    patpix[i+1024][7-j][k];
            }
        }
#endif
    }
    anydirty = 0;
}
#endif /* ASM_UPDATEPATPIX */



void tilebuf(void)
{
    int i, cnt;
    int base;
    byte *tilemap, *attrmap;
    int *tilebuf;
    int *wrap;
    static int wraptable[64] =
    {
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,-32
    };

    base = ((R_LCDC&0x08)?0x1C00:0x1800) + (T<<5) + S;
    tilemap = lcd.vbank[0] + base;
    attrmap = lcd.vbank[1] + base;
    tilebuf = BG;
    wrap = wraptable + S;
    cnt = ((WX + 7) >> 3) + 1;

    if (hw.cgb) {
        if (R_LCDC & 0x10)
            for (i = cnt; i > 0; i--)
            {
                *(tilebuf++) = *tilemap
                    | (((int)*attrmap & 0x08) << 6)
                    | (((int)*attrmap & 0x60) << 5);
                *(tilebuf++) = (((int)*attrmap & 0x07) << 2);
                attrmap += *wrap + 1;
                tilemap += *(wrap++) + 1;
            }
        else
            for (i = cnt; i > 0; i--)
            {
                *(tilebuf++) = (256 + ((n8)*tilemap))
                    | (((int)*attrmap & 0x08) << 6)
                    | (((int)*attrmap & 0x60) << 5);
                *(tilebuf++) = (((int)*attrmap & 0x07) << 2);
                attrmap += *wrap + 1;
                tilemap += *(wrap++) + 1;
            }
    }
    else
    {
        if (R_LCDC & 0x10)
            for (i = cnt; i > 0; i--)
            {
                *(tilebuf++) = *(tilemap++);
                tilemap += *(wrap++);
            }
        else
            for (i = cnt; i > 0; i--)
            {
                *(tilebuf++) = (256 + ((n8)*(tilemap++)));
                tilemap += *(wrap++);
            }
    }

    if (WX >= 160) return;

    base = ((R_LCDC&0x40)?0x1C00:0x1800) + (WT<<5);
    tilemap = lcd.vbank[0] + base;
    attrmap = lcd.vbank[1] + base;
    tilebuf = WND;
    cnt = ((160 - WX) >> 3) + 1;

    if (hw.cgb)
    {
        if (R_LCDC & 0x10)
            for (i = cnt; i > 0; i--)
            {
                *(tilebuf++) = *(tilemap++)
                    | (((int)*attrmap & 0x08) << 6)
                    | (((int)*attrmap & 0x60) << 5);
                *(tilebuf++) = (((int)*(attrmap++)&7) << 2);
            }
        else
            for (i = cnt; i > 0; i--)
            {
                *(tilebuf++) = (256 + ((n8)*(tilemap++)))
                    | (((int)*attrmap & 0x08) << 6)
                    | (((int)*attrmap & 0x60) << 5);
                *(tilebuf++) = (((int)*(attrmap++)&7) << 2);
            }
    }
    else

    {
        if (R_LCDC & 0x10)
            for (i = cnt; i > 0; i--)
                *(tilebuf++) = *(tilemap++);
        else
            for (i = cnt; i > 0; i--)
                *(tilebuf++) = (256 + ((n8)*(tilemap++)));
    }
}


// V = vertical line
// WX = WND start (if 0, no need to do anything) -> WY
// U = start...something...thingy... 7 at most
void bg_scan(void)
{
    int cnt;
    byte *src, *dest;
    int *tile;

    if (WX <= 0) return;
    cnt = WX;
    tile = BG;
    dest = BUF;

    src = patpix[*(tile++)][V] + U;
    memcpy(dest, src, 8-U);
    dest += 8-U;
    cnt -= 8-U;
    if (cnt <= 0) return;
    while (cnt >= 8)
    {
#if defined(CPU_COLDFIRE) && !defined(SIMULATOR)
      asm volatile (
         "move.l (%1)+,(%0)+ \n"
         "move.l (%1)+,(%0)+ \n"
         : /*outputs*/
         : /*inputs*/
         /* %0 */ "a" (dest),
         /* %1 */ "a" (patpix[*(tile++)][V])
         //: /* clobbers */
      );
#else
        src = patpix[*(tile++)][V];
        MEMCPY8(dest, src);
        dest += 8;
#endif
        cnt -= 8;
    }
    src = patpix[*tile][V];
    while (cnt--)
        *(dest++) = *(src++);
}

void wnd_scan(void)
{
    int cnt;
    byte *src, *dest;
    int *tile;

    if (WX >= 160) return;
    cnt = 160 - WX;
    tile = WND;
    dest = BUF + WX;

    while (cnt >= 8)
    {
#if defined(CPU_COLDFIRE) && !defined(SIMULATOR)
      asm volatile (
         "move.l (%1)+,(%0)+ \n"
         "move.l (%1)+,(%0)+ \n"
         : /*outputs*/
         : /*inputs*/
         /* %0 */ "a" (dest),
         /* %1 */ "a" (patpix[*(tile++)][WV])
         //: /* clobbers */
      );
#else
        src = patpix[*(tile++)][WV];
        MEMCPY8(dest, src);
        dest += 8;
#endif
        cnt -= 8;
    }
    src = patpix[*tile][WV];
    while (cnt--)
        *(dest++) = *(src++);
}

static void blendcpy(byte *dest, byte *src, byte b, int cnt)
{
    while (cnt--) *(dest++) = *(src++) | b;
}

static int priused(void *attr)
{
    un32 *a = attr;
    return (int)((a[0]|a[1]|a[2]|a[3]|a[4]|a[5]|a[6]|a[7])&0x80808080);
}

void bg_scan_pri(void)
{
    int cnt, i;
    byte *src, *dest;

    if (WX <= 0) return;
    i = S;
    cnt = WX;
    dest = PRI;
    src = lcd.vbank[1] + ((R_LCDC&0x08)?0x1C00:0x1800) + (T<<5);

    if (!priused(src))
    {
        memset(dest, 0, cnt);
        return;
    }

    memset(dest, src[i++&31]&128, 8-U);
    dest += 8-U;
    cnt -= 8-U;
    if (cnt <= 0) return;
    while (cnt >= 8)
    {
        memset(dest, src[i++&31]&128, 8);
        dest += 8;
        cnt -= 8;
    }
    memset(dest, src[i&31]&128, cnt);
}

void wnd_scan_pri(void)
{
    int cnt, i;
    byte *src, *dest;

    if (WX >= 160) return;
    i = 0;
    cnt = 160 - WX;
    dest = PRI + WX;
    src = lcd.vbank[1] + ((R_LCDC&0x40)?0x1C00:0x1800) + (WT<<5);

    if (!priused(src))
    {
        memset(dest, 0, cnt);
        return;
    }

    while (cnt >= 8)
    {
        memset(dest, src[i++]&128, 8);
        dest += 8;
        cnt -= 8;
    }
    memset(dest, src[i]&128, cnt);
}

void bg_scan_color(void)
{
    int cnt;
    byte *src, *dest;
    int *tile;

    if (WX <= 0) return;
    cnt = WX;
    tile = BG;
    dest = BUF;

    src = patpix[*(tile++)][V] + U;
    blendcpy(dest, src, *(tile++), 8-U);
    dest += 8-U;
    cnt -= 8-U;
    if (cnt <= 0) return;
    while (cnt >= 8)
    {
        src = patpix[*(tile++)][V];
#if defined(CPU_COLDFIRE) && !defined(SIMULATOR)
      asm volatile (
         "move.l (%2)+,%%d1 \n"

         "move.b %%d1,%%d2 \n"

         "move.b (%1)+,%%d0  \n"
         "or.l %%d2,%%d0      \n"
         "move.b %%d0,(%0)+   \n"

         "move.b (%1)+,%%d0  \n"
         "or.l %%d1,%%d0      \n"
         "move.b %%d0,(%0)+   \n"

         "move.b (%1)+,%%d0  \n"
         "or.l %%d2,%%d0      \n"
         "move.b %%d0,(%0)+   \n"

         "move.b (%1)+,%%d0  \n"
         "or.l %%d2,%%d0      \n"
         "move.b %%d0,(%0)+   \n"

         "move.b (%1)+,%%d0  \n"
         "or.l %%d2,%%d0      \n"
         "move.b %%d0,(%0)+   \n"

         "move.b (%1)+,%%d0  \n"
         "or.l %%d2,%%d0      \n"
         "move.b %%d0,(%0)+   \n"

         "move.b (%1)+,%%d0  \n"
         "or.l %%d2,%%d0      \n"
         "move.b %%d0,(%0)+   \n"

         "move.b (%1)+,%%d0  \n"
         "or.l %%d2,%%d0      \n"
         "move.b %%d0,(%0)+   \n"
   : /*outputs*/
   : /*inputs*/
   /* %0 */ "a" (dest),
   /* %1 */ "a" (src),
   /* %2 */ "a" (tile)
   : /* clobbers */
   "d0", "d1", "d2"
);
#else
        blendcpy(dest, src, *(tile++), 8);
        dest += 8;
#endif
        cnt -= 8;
    }
    src = patpix[*(tile++)][V];
    blendcpy(dest, src, *(tile++), cnt);
}

void wnd_scan_color(void)
{
    int cnt;
    byte *src, *dest;
    int *tile;

    if (WX >= 160) return;
    cnt = 160 - WX;
    tile = WND;
    dest = BUF + WX;

    while (cnt >= 8)
    {
        src = patpix[*(tile++)][WV];
        blendcpy(dest, src, *(tile++), 8);
        dest += 8;
        cnt -= 8;
    }
    src = patpix[*(tile++)][WV];
    blendcpy(dest, src, *(tile++), cnt);
}

static void recolor(byte *buf, byte fill, int cnt)
{
    while (cnt--) *(buf++) |= fill;
}

void spr_count(void)
{
    int i;
    struct obj *o;

    NS = 0;
    if (!(R_LCDC & 0x02)) return;

    o = lcd.oam.obj;

    for (i = 40; i; i--, o++)
    {
        if (L >= o->y || L + 16 < o->y)
            continue;
        if (L + 8 >= o->y && !(R_LCDC & 0x04))
            continue;
        if (++NS == 10) break;
    }
}

void spr_enum(void)
{
    int i, j;
    struct obj *o;
    struct vissprite ts[10];
    int v, pat;
    int l, x;

    NS = 0;
    if (!(R_LCDC & 0x02)) return;

    o = lcd.oam.obj;

    for (i = 40; i; i--, o++)
    {
        if (L >= o->y || L + 16 < o->y)
            continue;
        if (L + 8 >= o->y && !(R_LCDC & 0x04))
            continue;
        VS[NS].x = (int)o->x - 8;
        v = L - (int)o->y + 16;
        if (hw.cgb)
        {
            pat = o->pat | (((int)o->flags & 0x60) << 5)
                | (((int)o->flags & 0x08) << 6);
            VS[NS].pal = 32 + ((o->flags & 0x07) << 2);
        }
        else
        {
            pat = o->pat | (((int)o->flags & 0x60) << 5);
            VS[NS].pal = 32 + ((o->flags & 0x10) >> 2);
        }
        VS[NS].pri = (o->flags & 0x80) >> 7;
        if ((R_LCDC & 0x04))
        {
            pat &= ~1;
            if (v >= 8)
            {
                v -= 8;
                pat++;
            }
            if (o->flags & 0x40) pat ^= 1;
        }
        VS[NS].buf = patpix[pat][v];
        if (++NS == 10) break;
    }
    if (!sprsort||hw.cgb) return;
    /* not quite optimal but it finally works! */
    for (i = 0; i < NS; i++)
    {
        l = 0;
        x = VS[0].x;
        for (j = 1; j < NS; j++)
        {
            if (VS[j].x < x)
            {
                l = j;
                x = VS[j].x;
            }
        }
        ts[i] = VS[l];
        VS[l].x = 160;
    }
    memcpy(VS, ts, sizeof VS);
}

void spr_scan(void)
{
    int i, x;
    byte pal, b, ns = NS;
    byte *src, *dest, *bg, *pri;
    struct vissprite *vs;
    static byte bgdup[256];

    if (!ns) return;

    memcpy(bgdup, BUF, 256);
    vs = &VS[ns-1];

    for (; ns; ns--, vs--)
    {
        x = vs->x;
        if (x >= 160) continue;
        if (x <= -8) continue;
        if (x < 0)
        {
            src = vs->buf - x;
            dest = BUF;
            i = 8 + x;
        }
        else
        {
            src = vs->buf;
            dest = BUF + x;
            if (x > 152) i = 160 - x;
            else i = 8;
        }
        pal = vs->pal;
        if (vs->pri)
        {
            bg = bgdup + (dest - BUF);
            while (i--)
            {
                b = src[i];
                if (b && !(bg[i]&3)) dest[i] = pal|b;
            }
        }
        else if (hw.cgb)
        {
            bg = bgdup + (dest - BUF);
            pri = PRI + (dest - BUF);
            while (i--)
            {
                b = src[i];
                if (b && (!pri[i] || !(bg[i]&3)))
                    dest[i] = pal|b;
            }
        }
        else while (i--) if (src[i]) dest[i] = pal|src[i];
        /* else while (i--) if (src[i]) dest[i] = 31 + ns; */
    }
//    if (sprdebug) for (i = 0; i < NS; i++) BUF[i<<1] = 36;
}






void lcd_begin(void)
{
/*    if (fb.indexed)
    {
        if (rgb332) pal_set332();
        else pal_expire();
    }
    while (scale * 160 > fb.w || scale * 144 > fb.h) scale--; */
   if(options.fullscreen)
      vdest = fb.ptr;
   else
      vdest = fb.ptr + ((LCD_HEIGHT-144)/2)*LCD_WIDTH + ((LCD_WIDTH-160)/2);

    WY = R_WY;
}

char frameout[25];
void lcd_refreshline(void)
{
#if LCD_HEIGHT>=144
   int cnt=0, two;
#endif

    if (!fb.enabled) return;
    if(!insync) {
        if(R_LY!=0)
            return;
        else
           insync=1;
    }

    if (!(R_LCDC & 0x80))
        return; /* should not happen... */

#if LCD_HEIGHT < 144
    if ( (fb.mode==0&&(R_LY >= 128)) ||
         (fb.mode==1&&(R_LY < 16)) ||
         (fb.mode==2&&(R_LY<8||R_LY>=136)) ||
         (fb.mode==3&&((R_LY%9)==8))

#if LCD_HEIGHT == 64
        || (R_LY & 1) /* calculate only even lines */
#endif
        )
        return;
#endif

    updatepatpix();

    L = R_LY;
    X = R_SCX;
    Y = (R_SCY + L) & 0xff;
    S = X >> 3;
    T = Y >> 3;
    U = X & 7;
    V = Y & 7;

    WX = R_WX - 7;
    if (WY>L || WY<0 || WY>143 || WX<-7 || WX>159 || !(R_LCDC&0x20))
        WX = 160;
    WT = (L - WY) >> 3;
    WV = (L - WY) & 7;

    spr_enum();

    tilebuf();
    if (hw.cgb)
    {
        bg_scan_color();
        wnd_scan_color();
        if (NS)
        {
            bg_scan_pri();
            wnd_scan_pri();
        }
    }
    else
    {

        bg_scan();
        wnd_scan();
        recolor(BUF+WX, 0x04, 160-WX);
    }
    spr_scan();
#if LCD_DEPTH == 1
    if (scanline_ind == 7)
#elif LCD_DEPTH == 2
    if (scanline_ind == 3)
#endif
    {
#if LCD_HEIGHT < 144
        if(fb.mode!=3)
            vid_update(L);
        else
            vid_update(L-((int)(L/9)));
#else

   for(two=0;two<( (options.showstats ? (L&0x07)==0x05 : (L&0x07)==0x05 || (L&0x0F)==0x08) && options.fullscreen)+1;two++)
   {
      while (cnt < 160)
      {
         *vdest++ = scan.pal2[scan.buf[cnt++]];
         if( ((cnt&0x03)==0x03 || (cnt&0x07)==0x06) && options.fullscreen ) *vdest++ = scan.pal2[scan.buf[cnt]];
      }

      if(!options.fullscreen)
            vdest+=(LCD_WIDTH-160);
      cnt=0;
   }

   if(L==143)
   {
      if(options.showstats) {
         snprintf(frameout,sizeof(frameout),"FPS: %d \t %d ",options.fps, options.frameskip);
         if(options.fullscreen) rb->lcd_putsxy(0,166,frameout);
         else rb->lcd_putsxy((LCD_WIDTH-160)/2,(LCD_HEIGHT-144)/2,frameout);
      }
      if(options.fullscreen)
         rb->lcd_update();
      else
         rb->lcd_update_rect( (LCD_WIDTH-160)/2, (LCD_HEIGHT-144)/2, 160, 144 );
   }
#endif
    }
#if LCD_DEPTH == 1
    scanline_ind = (scanline_ind+1) % 8;
#elif LCD_DEPTH == 2
    scanline_ind = (scanline_ind+1) % 4;
#endif
}






#if HAVE_LCD_COLOR
static void updatepalette(int i)
{
    int c, r, g, b, y, u, v, rr, gg;

    c = (lcd.pal[i<<1] | ((int)lcd.pal[(i<<1)|1] << 8)) & 0x7FFF;
    r = (c & 0x001F) << 3;
    g = (c & 0x03E0) >> 2;
    b = (c & 0x7C00) >> 7;
    r |= (r >> 5);
    g |= (g >> 5);
    b |= (b >> 5);

    if (usefilter && (filterdmg||hw.cgb))
    {
        rr = ((r * filter[0][0] + g * filter[0][1] + b * filter[0][2]) >> 8) + filter[0][3];
        gg = ((r * filter[1][0] + g * filter[1][1] + b * filter[1][2]) >> 8) + filter[1][3];
        b = ((r * filter[2][0] + g * filter[2][1] + b * filter[2][2]) >> 8) + filter[2][3];
        r = rr;
        g = gg;
    }

    if (fb.yuv)
    {
        y = (((r *  263) + (g * 516) + (b * 100)) >> 10) + 16;
        u = (((r *  450) - (g * 377) - (b *  73)) >> 10) + 128;
        v = (((r * -152) - (g * 298) + (b * 450)) >> 10) + 128;
        if (y < 0) y = 0; if (y > 255) y = 255;
        if (u < 0) u = 0; if (u > 255) u = 255;
        if (v < 0) v = 0; if (v > 255) v = 255;
        PAL4[i] = (y<<fb.cc[0].l) | (y<<fb.cc[3].l)
                  | (u<<fb.cc[1].l) | (v<<fb.cc[2].l);
        return;
    }
/*
    if (fb.indexed)
    {
        pal_release(PAL1[i]);
        c = pal_getcolor(c, r, g, b);
        PAL1[i] = c;
        PAL2[i] = (c<<8) | c;
        PAL4[i] = (c<<24) | (c<<16) | (c<<8) | c;
        return;
    }*/

    r = (r >> fb.cc[0].r) << fb.cc[0].l;
    g = (g >> fb.cc[1].r) << fb.cc[1].l;
    b = (b >> fb.cc[2].r) << fb.cc[2].l;

#if LCD_PIXELFORMAT == RGB565
    c = r|g|b;
#elif LCD_PIXELFORMAT == RGB565SWAPPED
    c = swap16(r|g|b);
#endif


    switch (fb.pelsize)
    {
    case 1:
        PAL1[i] = c;
        PAL2[i] = (c<<8) | c;
        PAL4[i] = (c<<24) | (c<<16) | (c<<8) | c;
        break;
    case 2:
        PAL2[i] = c;
        PAL4[i] = (c<<16) | c;
        break;
    case 3:
    case 4:
        PAL4[i] = c;
        break;
    }
}
#endif

void pal_write(int i, byte b)
{
    if (lcd.pal[i] == b) return;
    lcd.pal[i] = b;
#if LCD_DEPTH ==16
    updatepalette(i>>1);
#endif
}

void pal_write_dmg(int i, int mapnum, byte d)
{
    int j;
    int *cmap = dmg_pal[mapnum];
    int c, r, g, b;

    if (hw.cgb) return;

    /* if (mapnum >= 2) d = 0xe4; */
    for (j = 0; j < 8; j += 2)
    {
        c = cmap[(d >> j) & 3];
        r = (c & 0xf8) >> 3;
        g = (c & 0xf800) >> 6;
        b = (c & 0xf80000) >> 9;
        c = r|g|b;
        /* FIXME - handle directly without faking cgb */
        pal_write(i+j, c & 0xff);
        pal_write(i+j+1, c >> 8);
    }
}

void vram_write(int a, byte b)
{
    lcd.vbank[R_VBK&1][a] = b;
    if (a >= 0x1800) return;
    patdirty[((R_VBK&1)<<9)+(a>>4)] = 1;
    anydirty = 1;
}

void vram_dirty(void)
{
    anydirty = 1;
    memset(patdirty, 1, sizeof patdirty);
}

void pal_dirty(void)
{
#if LCD_DEPTH ==16
    int i;
#endif
    if (!hw.cgb)
    {

        pal_write_dmg(0, 0, R_BGP);
        pal_write_dmg(8, 1, R_BGP);
        pal_write_dmg(64, 2, R_OBP0);
        pal_write_dmg(72, 3, R_OBP1);
    }
#if LCD_DEPTH ==16
    for (i = 0; i < 64; i++)
        updatepalette(i);
#endif
}

void lcd_reset(void)
{
    memset(&lcd, 0, sizeof lcd);
    lcd_begin();
    vram_dirty();
    pal_dirty();
}
















