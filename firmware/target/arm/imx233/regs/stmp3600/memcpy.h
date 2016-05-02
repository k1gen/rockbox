/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * This file was automatically generated by headergen, DO NOT EDIT it.
 * headergen version: 3.0.0
 * stmp3600 version: 2.4.0
 * stmp3600 authors: Amaury Pouly
 *
 * Copyright (C) 2015 by the authors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#ifndef __HEADERGEN_STMP3600_MEMCPY_H__
#define __HEADERGEN_STMP3600_MEMCPY_H__

#define HW_MEMCPY_CTRL                      HW(MEMCPY_CTRL)
#define HWA_MEMCPY_CTRL                     (0x80014000 + 0x0)
#define HWT_MEMCPY_CTRL                     HWIO_32_RW
#define HWN_MEMCPY_CTRL                     MEMCPY_CTRL
#define HWI_MEMCPY_CTRL                     
#define HW_MEMCPY_CTRL_SET                  HW(MEMCPY_CTRL_SET)
#define HWA_MEMCPY_CTRL_SET                 (HWA_MEMCPY_CTRL + 0x4)
#define HWT_MEMCPY_CTRL_SET                 HWIO_32_WO
#define HWN_MEMCPY_CTRL_SET                 MEMCPY_CTRL
#define HWI_MEMCPY_CTRL_SET                 
#define HW_MEMCPY_CTRL_CLR                  HW(MEMCPY_CTRL_CLR)
#define HWA_MEMCPY_CTRL_CLR                 (HWA_MEMCPY_CTRL + 0x8)
#define HWT_MEMCPY_CTRL_CLR                 HWIO_32_WO
#define HWN_MEMCPY_CTRL_CLR                 MEMCPY_CTRL
#define HWI_MEMCPY_CTRL_CLR                 
#define HW_MEMCPY_CTRL_TOG                  HW(MEMCPY_CTRL_TOG)
#define HWA_MEMCPY_CTRL_TOG                 (HWA_MEMCPY_CTRL + 0xc)
#define HWT_MEMCPY_CTRL_TOG                 HWIO_32_WO
#define HWN_MEMCPY_CTRL_TOG                 MEMCPY_CTRL
#define HWI_MEMCPY_CTRL_TOG                 
#define BP_MEMCPY_CTRL_SFTRST               31
#define BM_MEMCPY_CTRL_SFTRST               0x80000000
#define BV_MEMCPY_CTRL_SFTRST__RUN          0x0
#define BV_MEMCPY_CTRL_SFTRST__RESET        0x1
#define BF_MEMCPY_CTRL_SFTRST(v)            (((v) & 0x1) << 31)
#define BFM_MEMCPY_CTRL_SFTRST(v)           BM_MEMCPY_CTRL_SFTRST
#define BF_MEMCPY_CTRL_SFTRST_V(e)          BF_MEMCPY_CTRL_SFTRST(BV_MEMCPY_CTRL_SFTRST__##e)
#define BFM_MEMCPY_CTRL_SFTRST_V(v)         BM_MEMCPY_CTRL_SFTRST
#define BP_MEMCPY_CTRL_CLKGATE              30
#define BM_MEMCPY_CTRL_CLKGATE              0x40000000
#define BV_MEMCPY_CTRL_CLKGATE__RUN         0x0
#define BV_MEMCPY_CTRL_CLKGATE__NO_CLKS     0x1
#define BF_MEMCPY_CTRL_CLKGATE(v)           (((v) & 0x1) << 30)
#define BFM_MEMCPY_CTRL_CLKGATE(v)          BM_MEMCPY_CTRL_CLKGATE
#define BF_MEMCPY_CTRL_CLKGATE_V(e)         BF_MEMCPY_CTRL_CLKGATE(BV_MEMCPY_CTRL_CLKGATE__##e)
#define BFM_MEMCPY_CTRL_CLKGATE_V(v)        BM_MEMCPY_CTRL_CLKGATE
#define BP_MEMCPY_CTRL_PRESENT              29
#define BM_MEMCPY_CTRL_PRESENT              0x20000000
#define BV_MEMCPY_CTRL_PRESENT__UNAVAILABLE 0x0
#define BV_MEMCPY_CTRL_PRESENT__AVAILABLE   0x1
#define BF_MEMCPY_CTRL_PRESENT(v)           (((v) & 0x1) << 29)
#define BFM_MEMCPY_CTRL_PRESENT(v)          BM_MEMCPY_CTRL_PRESENT
#define BF_MEMCPY_CTRL_PRESENT_V(e)         BF_MEMCPY_CTRL_PRESENT(BV_MEMCPY_CTRL_PRESENT__##e)
#define BFM_MEMCPY_CTRL_PRESENT_V(v)        BM_MEMCPY_CTRL_PRESENT
#define BP_MEMCPY_CTRL_BURST                16
#define BM_MEMCPY_CTRL_BURST                0x10000
#define BF_MEMCPY_CTRL_BURST(v)             (((v) & 0x1) << 16)
#define BFM_MEMCPY_CTRL_BURST(v)            BM_MEMCPY_CTRL_BURST
#define BF_MEMCPY_CTRL_BURST_V(e)           BF_MEMCPY_CTRL_BURST(BV_MEMCPY_CTRL_BURST__##e)
#define BFM_MEMCPY_CTRL_BURST_V(v)          BM_MEMCPY_CTRL_BURST
#define BP_MEMCPY_CTRL_XFER_SIZE            0
#define BM_MEMCPY_CTRL_XFER_SIZE            0xffff
#define BF_MEMCPY_CTRL_XFER_SIZE(v)         (((v) & 0xffff) << 0)
#define BFM_MEMCPY_CTRL_XFER_SIZE(v)        BM_MEMCPY_CTRL_XFER_SIZE
#define BF_MEMCPY_CTRL_XFER_SIZE_V(e)       BF_MEMCPY_CTRL_XFER_SIZE(BV_MEMCPY_CTRL_XFER_SIZE__##e)
#define BFM_MEMCPY_CTRL_XFER_SIZE_V(v)      BM_MEMCPY_CTRL_XFER_SIZE

#define HW_MEMCPY_DATA              HW(MEMCPY_DATA)
#define HWA_MEMCPY_DATA             (0x80014000 + 0x10)
#define HWT_MEMCPY_DATA             HWIO_32_RW
#define HWN_MEMCPY_DATA             MEMCPY_DATA
#define HWI_MEMCPY_DATA             
#define HW_MEMCPY_DATA_SET          HW(MEMCPY_DATA_SET)
#define HWA_MEMCPY_DATA_SET         (HWA_MEMCPY_DATA + 0x4)
#define HWT_MEMCPY_DATA_SET         HWIO_32_WO
#define HWN_MEMCPY_DATA_SET         MEMCPY_DATA
#define HWI_MEMCPY_DATA_SET         
#define HW_MEMCPY_DATA_CLR          HW(MEMCPY_DATA_CLR)
#define HWA_MEMCPY_DATA_CLR         (HWA_MEMCPY_DATA + 0x8)
#define HWT_MEMCPY_DATA_CLR         HWIO_32_WO
#define HWN_MEMCPY_DATA_CLR         MEMCPY_DATA
#define HWI_MEMCPY_DATA_CLR         
#define HW_MEMCPY_DATA_TOG          HW(MEMCPY_DATA_TOG)
#define HWA_MEMCPY_DATA_TOG         (HWA_MEMCPY_DATA + 0xc)
#define HWT_MEMCPY_DATA_TOG         HWIO_32_WO
#define HWN_MEMCPY_DATA_TOG         MEMCPY_DATA
#define HWI_MEMCPY_DATA_TOG         
#define BP_MEMCPY_DATA_DATA         0
#define BM_MEMCPY_DATA_DATA         0xffffffff
#define BF_MEMCPY_DATA_DATA(v)      (((v) & 0xffffffff) << 0)
#define BFM_MEMCPY_DATA_DATA(v)     BM_MEMCPY_DATA_DATA
#define BF_MEMCPY_DATA_DATA_V(e)    BF_MEMCPY_DATA_DATA(BV_MEMCPY_DATA_DATA__##e)
#define BFM_MEMCPY_DATA_DATA_V(v)   BM_MEMCPY_DATA_DATA

#define HW_MEMCPY_DEBUG                     HW(MEMCPY_DEBUG)
#define HWA_MEMCPY_DEBUG                    (0x80014000 + 0x20)
#define HWT_MEMCPY_DEBUG                    HWIO_32_RW
#define HWN_MEMCPY_DEBUG                    MEMCPY_DEBUG
#define HWI_MEMCPY_DEBUG                    
#define BP_MEMCPY_DEBUG_DST_END_CMD         30
#define BM_MEMCPY_DEBUG_DST_END_CMD         0x40000000
#define BF_MEMCPY_DEBUG_DST_END_CMD(v)      (((v) & 0x1) << 30)
#define BFM_MEMCPY_DEBUG_DST_END_CMD(v)     BM_MEMCPY_DEBUG_DST_END_CMD
#define BF_MEMCPY_DEBUG_DST_END_CMD_V(e)    BF_MEMCPY_DEBUG_DST_END_CMD(BV_MEMCPY_DEBUG_DST_END_CMD__##e)
#define BFM_MEMCPY_DEBUG_DST_END_CMD_V(v)   BM_MEMCPY_DEBUG_DST_END_CMD
#define BP_MEMCPY_DEBUG_DST_KICK            29
#define BM_MEMCPY_DEBUG_DST_KICK            0x20000000
#define BF_MEMCPY_DEBUG_DST_KICK(v)         (((v) & 0x1) << 29)
#define BFM_MEMCPY_DEBUG_DST_KICK(v)        BM_MEMCPY_DEBUG_DST_KICK
#define BF_MEMCPY_DEBUG_DST_KICK_V(e)       BF_MEMCPY_DEBUG_DST_KICK(BV_MEMCPY_DEBUG_DST_KICK__##e)
#define BFM_MEMCPY_DEBUG_DST_KICK_V(v)      BM_MEMCPY_DEBUG_DST_KICK
#define BP_MEMCPY_DEBUG_DST_DMA_REQ         28
#define BM_MEMCPY_DEBUG_DST_DMA_REQ         0x10000000
#define BF_MEMCPY_DEBUG_DST_DMA_REQ(v)      (((v) & 0x1) << 28)
#define BFM_MEMCPY_DEBUG_DST_DMA_REQ(v)     BM_MEMCPY_DEBUG_DST_DMA_REQ
#define BF_MEMCPY_DEBUG_DST_DMA_REQ_V(e)    BF_MEMCPY_DEBUG_DST_DMA_REQ(BV_MEMCPY_DEBUG_DST_DMA_REQ__##e)
#define BFM_MEMCPY_DEBUG_DST_DMA_REQ_V(v)   BM_MEMCPY_DEBUG_DST_DMA_REQ
#define BP_MEMCPY_DEBUG_SRC_KICK            25
#define BM_MEMCPY_DEBUG_SRC_KICK            0x2000000
#define BF_MEMCPY_DEBUG_SRC_KICK(v)         (((v) & 0x1) << 25)
#define BFM_MEMCPY_DEBUG_SRC_KICK(v)        BM_MEMCPY_DEBUG_SRC_KICK
#define BF_MEMCPY_DEBUG_SRC_KICK_V(e)       BF_MEMCPY_DEBUG_SRC_KICK(BV_MEMCPY_DEBUG_SRC_KICK__##e)
#define BFM_MEMCPY_DEBUG_SRC_KICK_V(v)      BM_MEMCPY_DEBUG_SRC_KICK
#define BP_MEMCPY_DEBUG_SRC_DMA_REQ         24
#define BM_MEMCPY_DEBUG_SRC_DMA_REQ         0x1000000
#define BF_MEMCPY_DEBUG_SRC_DMA_REQ(v)      (((v) & 0x1) << 24)
#define BFM_MEMCPY_DEBUG_SRC_DMA_REQ(v)     BM_MEMCPY_DEBUG_SRC_DMA_REQ
#define BF_MEMCPY_DEBUG_SRC_DMA_REQ_V(e)    BF_MEMCPY_DEBUG_SRC_DMA_REQ(BV_MEMCPY_DEBUG_SRC_DMA_REQ__##e)
#define BFM_MEMCPY_DEBUG_SRC_DMA_REQ_V(v)   BM_MEMCPY_DEBUG_SRC_DMA_REQ
#define BP_MEMCPY_DEBUG_WRITE_STATE         2
#define BM_MEMCPY_DEBUG_WRITE_STATE         0xc
#define BF_MEMCPY_DEBUG_WRITE_STATE(v)      (((v) & 0x3) << 2)
#define BFM_MEMCPY_DEBUG_WRITE_STATE(v)     BM_MEMCPY_DEBUG_WRITE_STATE
#define BF_MEMCPY_DEBUG_WRITE_STATE_V(e)    BF_MEMCPY_DEBUG_WRITE_STATE(BV_MEMCPY_DEBUG_WRITE_STATE__##e)
#define BFM_MEMCPY_DEBUG_WRITE_STATE_V(v)   BM_MEMCPY_DEBUG_WRITE_STATE
#define BP_MEMCPY_DEBUG_READ_STATE          0
#define BM_MEMCPY_DEBUG_READ_STATE          0x3
#define BF_MEMCPY_DEBUG_READ_STATE(v)       (((v) & 0x3) << 0)
#define BFM_MEMCPY_DEBUG_READ_STATE(v)      BM_MEMCPY_DEBUG_READ_STATE
#define BF_MEMCPY_DEBUG_READ_STATE_V(e)     BF_MEMCPY_DEBUG_READ_STATE(BV_MEMCPY_DEBUG_READ_STATE__##e)
#define BFM_MEMCPY_DEBUG_READ_STATE_V(v)    BM_MEMCPY_DEBUG_READ_STATE

#endif /* __HEADERGEN_STMP3600_MEMCPY_H__*/