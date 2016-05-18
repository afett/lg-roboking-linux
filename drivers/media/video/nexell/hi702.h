/*
     Copyright (c) 2010 MOSTiTECH co., ltd.
     All right reserved. by Seungwoo Kim. <ksw@mostitech.com>
     
     Hynix camera module YACBAC1SDDAS(Hi-702) driver for S3C64XX.
*/
#ifndef __HI_702_CAM_H__
#define __HI_702_CAM_H__
/*
0x03(P0) PAGEMODE
0x00 = control image size, windowing, sync, black level calibration
0x10 = control image format, image effect
0x11 = control ZLPF
0x12 = control dead pixel concealment.
0x13 = control edge enhancement
0x14 = control lens shading correction.
0x15 = control color correction.
0x16 = control gamma correction.
0x17 = control auto flicker cancellation
0x18 = control image scaling
0x20 = control auto exposure
0x22 = control auto white balance.
*/

#define HI702_PAGE_MAP 0x03
/* Device ID and Image Size and Windowing and Sync (Page Mode = 0) */
#define HI_702_PWRCTL  0x0001 /* (P0) Power sleep mode 0x51 Current                                                             */
#define HI_702_DEVID   0x0004 /* (P0) Device ID1 0x8C(RO)                                                                       */
#define HI_702_VDOCTL1 0x0010 /* (P0) Control sub-sampling and vsync type 0x00 Next                                             */
#define HI_702_VDOCTL2 0x0011 /* (P0) Control strobe, windowing, fixed frame rate, X/Y flip and skip frame 0x90 Next            */
#define HI_702_SYNCCTL 0x0012 /* (P0) Control polarity of H/VSYNC and PCLK, and Internal clock divider 0x04 Next                */
#define HI_702_WINROWH 0x0020 /* (P0) High byte of row start address for windowing 0x00 Next                                    */
#define HI_702_WINROWL 0x0021 /* (P0) Low byte of row start address for windowing 0x05 Next                                     */
#define HI_702_WINCOLH 0x0022 /* (P0) High byte of column start address for windowing 0x00 Next                                 */
#define HI_702_WINCOLL 0x0023 /* (P0) Low byte of column start address for windowing 0x07 Next                                  */
#define HI_702_WINHGTH 0x0024 /* (P0) High byte of height for windowing 0x01 Next                                               */
#define HI_702_WINHGTL 0x0025 /* (P0) Low byte of height for windowing 0xE0 Next                                                */
#define HI_702_WINWIDH 0x0026 /* (P0) High byte of width for windowing 0x02 Next                                                */
#define HI_702_WINWIDL 0x0027 /* (P0) Low byte of width for windowing 0x80 Next                                                 */
#define HI_702_HBLANKH 0x0040 /* (P0) High byte of duration for horizontal blanking 0x01 Next                                   */
#define HI_702_HBLANKL 0x0041 /* (P0) Low byte of duration for horizontal blanking 0x50 Next                                    */
#define HI_702_VSYNCH  0x0042 /* (P0) High byte of duration for vertical synchronization(Type1) 0x00 Next                       */
#define HI_702_VSYNCL  0x0043 /* (P0) Low byte of duration for vertical synchronization(Type1) 0x14 Next                        */
#define HI_702_VSCLIP  0x0044 /* (P0) Clipping duration in vertical synchronization 0x09 Current                                */
#define HI_702_VSCTL1  0x0045 /* (P0) High 4bits of start/stop row position for vertical synchronization(Type2) 0x01 Current    */
#define HI_702_VSCTL2  0x0046 /* (P0) Low byte of start row position for vertical synchronization(Type2) 0x02 Current           */
#define HI_702_VSCTL3  0x0047 /* (P0) Low byte of stop row position for vertical synchronization(Type2) 0xE2 Current            */
/* Black level calibration (Page Mode = 0) */
#define HI_702_BLCCTL       0x0080 /* (P0) Control optical black level calibration 0x08 Next                                    */
#define HI_702_BLCTIMETHON  0x0090 /* (P0) Exposure time threshold to enable additional BLC 0x07 Next                           */
#define HI_702_BLCTIMETHOFF 0x0091 /* (P0) Exposure time threshold to disable additional BLC 0x08 Next                          */
#define HI_702_BLCAGTHH     0x0092 /* (P0) AG threshold to enable additional BLC 0x60 Next                                      */
#define HI_702_BLCAGTHL     0x0093 /* (P0) AG threshold to disable additional BLC 0x60 Next                                     */
#define HI_702_BLCDGH       0x0094 /* (P0) Digital gain threshold to enable additional BLC 0xC8 Next                            */
#define HI_702_BLCDGL       0x0095 /* (P0) Digital gain threshold to disable additional BLC 0xC0 Next                           */
#define HI_702_DOFSB        0x00A0 /* (P0) Blue color1 offset for additional BLC 0x00 Current                                   */
#define HI_702_DOFSGB       0x00A2 /* (P0) Gb color1 offset for additional BLC 0x00 Current                                     */
#define HI_702_DOFSR        0x00A4 /* (P0) Red color1 offset for additional BLC 0x00 Current                                    */
#define HI_702_DOFSGR       0x00A6 /* (P0) Gr color1 offset for additional BLC 0x00 Current                                     */
/* Image Output Format and Image Effect (Page Mode = 10) */
#define HI_702_ISPCTL1 0x1010 /* (P10)                                                                                          */
#define HI_702_ISPCTL2 0x1011 /* (P10) Control image effect and color interpolation and color space conversion 0x03 Next        */
#define HI_702_ISPCTL3 0x1012 /* (P10) Control color offset and brightness 0x00 Next                                            */
#define HI_702_ISPCTL4 0x1013 /* (P10) Control the image effect 0x00 Current                                                    */
#define HI_702_ISPCTL5 0x1014 /* (P10) Control the format of image data 0x00 Current                                            */
#define HI_702_YOFS    0x1040 /* (P10) Offset of luminance 0x00 Current                                                         */
#define HI_702_DYOFS   0x1041 /* (P10) Offset of luminance at dark condition 0x00 Current                                       */
#define HI_702_UOFS    0x1042 /* (P10) Offset of U chrominance 0x00 Next                                                        */
#define HI_702_VOFS    0x1043 /* (P10) Offset of V chrominance 0x00 Next                                                        */
#define HI_702_UCON    0x1044 /* (P10) Constant of U chrominance 0x80 Next                                                      */
#define HI_702_VCON    0x1045 /* (P10) Constant of V chrominance 0x80 Next                                                      */
#define HI_702_BINARY  0x1047 /* (P10) Threshold of binary effect. 0x7F Current                                                 */
#define HI_702_AGBRT   0x1050 /* (P10) Threshold of AG in auto bright 0x60 Current                                              */
#define HI_702_SATCTL  0x1060 /* (P10) Control color saturation 0x00 Current                                                    */
#define HI_702_SATB    0x1061 /* (P10) Blue color saturation coefficient 0x80 Current                                           */
#define HI_702_SATR    0x1062 /* (P10) Red color saturation coefficient 0x80 Current                                            */
#define HI_702_AGSAT   0x1063 /* (P10) Threshold of AG in auto saturation 0x60 Current                                          */
/* Z-LPF (Page Mode = 11) */
#define HI_702_ZLPFCTL1  0x1110 /* (P11) Control Z-LPF 0x00 Current                                                             */
#define HI_702_ZLPFCTL2  0x1111 /* (P11) Control Z-LPF 0x1A Current                                                             */
#define HI_702_ZLPFCTL3  0x1120 /* (P11) Control Z-LPF 0x00 Current                                                             */
#define HI_702_ZLPFAGTH  0x1121 /* (P11) Threshold of AG for automatic LPF 0x20 Current                                         */
#define HI_702_ZLPFDGTH  0x1122 /* (P11) Threshold of Digital gain for automatic LPF 0xFF Current                               */
#define HI_702_ZLPFEXPTH 0x1123 /* (P11) Threshold of Exposure time for automatic LPF 0x10 Current                              */
#define HI_702_ZLPFTH    0x1160 /* (P11) Threshold for Z-LPF 0x40 Current                                                       */
#define HI_702_ZLPFHLVL  0x1161 /* (P11) Threshold of Z-LPF high level. 0x83 Current                                            */
#define HI_702_ZLPFLLVL  0x1162 /* (P11) Threshold of Z-LPF low level. 0x43 Current                                             */
#define HI_702_ZLPFDYTH  0x1163 /* (P11) Threshold of dynamic range for Z-LPF 0x23 Current                                      */
/* YC-LPF (Page Mode = 12) */
#define HI_702_YCLPFCTL1 0x1240 /* (P12) Control YC-LPF 0x00 Current                                                            */
#define HI_702_YCLPFCTL2 0x1241 /* (P12) Control YC-LPF 0x30 Current                                                            */
#define HI_702_YCLPFTH   0x1250 /* (P12) Threshold of YUV for YC-LPF 0x10 Current                                               */
/* B-LPF (Page Mode = 12) */
#define HI_702_BLPFCTL 0x1270 /* (P12) Control B-LPF 0x1C Current                                                               */
#define HI_702_BLPFTH1 0x1274 /* (P12) Threshold for B-LPF 0x10 Current                                                         */
#define HI_702_BLPFTH2 0x1275 /* (P12) Threshold for B-LPF 0x10 Current                                                         */
/* Dead Pixel Cancellation (Page Mode = 12) */
#define HI_702_DPCCTL 0x1290 /* (P12) Control DPC 0x5D Current                                                                  */
/* Edge Enhancement (Page Mode = 13) */
#define HI_702_EDGECTL1      0x1310 /* (P13) Control edge enhancement 0x18 Current                                              */
#define HI_702_EDGECTL2      0x1311 /* (P13) Control edge enhancement. 0x06 Current                                             */
#define HI_702_EDGECTL3      0x1314 /* (P13) Control edge enhancement. 0x00 Current                                             */
#define HI_702_EDGENGAIN     0x1320 /* (P13) Edge gain to emphasize negative edge data 0x07 Current                             */
#define HI_702_EDGEPGAIN     0x1321 /* (P13) Edge gain to emphasize positive edge data 0x07 Current                             */
#define HI_702_EDGEHCLIPTH   0x1323 /* (P13) High clip threshold. 0x30 Current                                                  */
#define HI_702_EDGELCLIPTH   0x1324 /* (P13) Low clip threshold. 0x00 Current                                                   */
#define HI_702_EDGETIMETH    0x1328 /* (P13) Threshold of Exposure time for automatic edge enhancement. 0x10 Current            */
#define HI_702_EDGEAGTH      0x1329 /* (P13) Threshold of AG for automatic edge enhancement. 0x30 Current                       */
#define HI_702_EDGE2DCTL1    0x1380 /* (P13) Control 2nd edge enhancement 0x06 Current                                          */
#define HI_702_EDGE2DCTL2    0x1381 /* (P13) Control 2nd edge enhancement 0x10 Current                                          */
#define HI_702_EDGE2DCTL3    0x1383 /* (P13) Control 2nd edge enhancement 0x5D Current                                          */
#define HI_702_EDGE2DNGAIN   0x1390 /* (P13) Edge gain to emphasize negative edge data for 2nd edge enhancement 0x07 Current    */
#define HI_702_EDGE2DPGAIN   0x1391 /* (P13) Edge gain to emphasize positive edge data for 2nd edge enhancement 0x07 Current    */
#define HI_702_EDGE2DHCLIPTH 0x1393 /* (P13) High clip threshold for 2nd edge enhancement 0x30 Current                          */
#define HI_702_EDGE2DLCLIPTH 0x1394 /* (P13) Low clip threshold for 2nd edge enhancement 0x00 Current                           */
/* Lens Shading (Page Mode = 14) */
#define HI_702_LENSCTL 0x1410 /* (P14) Control lens shading correction 0x00 Current                                             */
#define HI_702_XCEN    0x1420 /* (P14) Optical center on X-axis in pixel array for lens shading correction 0x80 Current         */
#define HI_702_YCEN    0x1421 /* (P14) Optical center on Y-axis in pixel array for lens shading correction 0x80 Current         */
#define HI_702_LENSR   0x1422 /* (P14) Lens shading correction coefficient in R color 0x60 Current                              */
#define HI_702_LENSG   0x1423 /* (P14) Lens shading correction coefficient in G color 0x60 Current                              */
#define HI_702_LENSB   0x1424 /* (P14) Lens shading correction coefficient in B color 0x60 Current                              */
#define HI_702_LAGOFF  0x1425 /* (P14) AG threshold to disable Lens Shading Correction Gain 0x60 Current                        */
#define HI_702_LAGON   0x1426 /* (P14) AG threshold to enable Lens Shading Correction Gain 0x60 Current                         */
/* Color Correction (Page Mode = 15) */
#define HI_702_CMCCTL   0x1510 /* (P15) Control color correction 0x0E Current                                                   */
#define HI_702_CMCOFSGH 0x1514 /* (P15) High threshold of color offset gain 0x38 Current                                        */
#define HI_702_CMCOFSGL 0x1516 /* (P15) Low threshold of color offset gain 0x20 Current                                         */
#define HI_702_CMCSIGN  0x1517 /* (P15) Sign bit of color correction coefficient 0x2F Current                                   */
#define HI_702_CMC11    0x1530 /* (P15) Color correction coefficient 11 0x4C Current                                            */
#define HI_702_CMC12    0x1531 /* (P15) Color correction coefficient 12 0x0C Current                                            */
#define HI_702_CMC13    0x1532 /* (P15) Color correction coefficient 13 0x00 Current                                            */
#define HI_702_CMC21    0x1533 /* (P15) Color correction coefficient 21 0x26 Current                                            */
#define HI_702_CMC22    0x1534 /* (P15) Color correction coefficient 22 0x80 Current                                            */
#define HI_702_CMC23    0x1535 /* (P15) Color correction coefficient 23 0x1A Current                                            */
#define HI_702_CMC31    0x1536 /* (P15) Color correction coefficient 31 0x20 Current                                            */
#define HI_702_CMC32    0x1537 /* (P15) Color correction coefficient 32 0x80 Current                                            */
#define HI_702_CMC33    0x1538 /* (P15) Color correction coefficient 33 0xE0 Current                                            */
#define HI_702_CMCOFS11 0x1540 /* (P15) Color correction offset coefficient 11 0x12 Current                                     */
#define HI_702_CMCOFS12 0x1541 /* (P15) Color correction offset coefficient 12 0xA0 Current                                     */
#define HI_702_CMCOFS13 0x1542 /* (P15) Color correction offset coefficient 13 0x0E Current                                     */
#define HI_702_CMCOFS21 0x1543 /* (P15) Color correction offset coefficient 21 0x84 Current                                     */
#define HI_702_CMCOFS22 0x1544 /* (P15) Color correction offset coefficient 22 0x08 Current                                     */
#define HI_702_CMCOFS23 0x1545 /* (P15) Color correction offset coefficient 23 0x84 Current                                     */
#define HI_702_CMCOFS31 0x1546 /* (P15) Color correction offset coefficient 31 0x8A Current                                     */
#define HI_702_CMCOFS32 0x1547 /* (P15) Color correction offset coefficient 32 0x02 Current                                     */
#define HI_702_CMCOFS33 0x1548 /* (P15) Color correction offset coefficient 33 0x08 Current                                     */
/* Gamma Correction (Page Mode = 16) */
#define HI_702_GMACTL 0x1610 /* (P16) Control gamma correction 0x01 Current                                                     */
#define HI_702_GMA0   0x1630 /* (P16) Gamma corrected output at 0(0)code in 10(8)bit linear input 0x00 Current                  */
#define HI_702_GMA1   0x1631 /* (P16) Gamma corrected output at 16(4)code in 10(8)bit linear input 0x15 Current                 */
#define HI_702_GMA2   0x1632 /* (P16) Gamma corrected output at 32(8)code in 10(8)bit linear input 0x1F Current                 */
#define HI_702_GMA3   0x1633 /* (P16) Gamma corrected output at 64(16)code in 10(8)bit linear input 0x30 Current                */
#define HI_702_GMA4   0x1634 /* (P16) Gamma corrected output at 128(32)code in 10(8)bit linear input 0x49 Current               */
#define HI_702_GMA5   0x1635 /* (P16) Gamma corrected output at 192(48)code in 10(8)bit t linear input 0x5D Current             */
#define HI_702_GMA6   0x1636 /* (P16) Gamma corrected output at 256(64)code in 10(8)bit linear input 0x6F Current               */
#define HI_702_GMA7   0x1637 /* (P16) Gamma corrected output at 320(80)code in 10(8)bit linear input 0x7F Current               */
#define HI_702_GMA8   0x1638 /* (P16) Gamma corrected output at 384(96)code in 10(8)bit linear input 0x8E Current               */
#define HI_702_GMA9   0x1639 /* (P16) Gamma corrected output at 448(112)code in 10(8)bit linear input 0x9B Current              */
#define HI_702_GMA10  0x163A /* (P16) Gamma corrected output at 512(128)code in 10(8)bit linear input 0xA8 Current              */
#define HI_702_GMA11  0x163B /* (P16) Gamma corrected output at 640(160)code in 10(8)bit linear input 0xC1 Current              */
#define HI_702_GMA12  0x163C /* (P16) Gamma corrected output at 768(192)code in 10(8)bit linear input 0xD7 Current              */
#define HI_702_GMA13  0x163D /* (P16) Gamma corrected output at 896(224)code in 10(8)bit linear input 0xF6 Current              */
#define HI_702_GMA14  0x163E /* (P16) Gamma corrected output at 1023(255)code in 10(8)bit linear input 0xFF Current             */
/* Auto Flicker Cancellation (Page Mode = 17) */
#define HI_702_FLKMODE 0x17C0 /* (P17) Number of skipped frames in AFC operation 0x00 Current                                   */
#define HI_702_FLK200  0x17C4 /* (P17) Number of horizontal line for 1/200sec 0x3C Current                                      */
#define HI_702_FLK240  0x17C5 /* (P17) Number of horizontal line for 1/240sec 0x32 Current                                      */
#define HI_702_FLKTH1  0x17C6 /* (P17) Low threshold to detect flicker noise 0x02 Current                                       */
#define HI_702_FLKTH2  0x17C7 /* (P17) High threshold to detect flicker noise 0x20 Current                                      */
/* Image Scaling (Page Mode = 18) */
#define HI_702_ZOOMCTL   0x1810 /* (P18) Control image scaling 0x00 Next                                                        */
#define HI_702_ZOUTWIDH  0x1820 /* (P18) High byte of image width for image scaling 0x02 Next                                   */
#define HI_702_ZOUTWIDL  0x1821 /* (P18) Low byte of image width for image scaling 0x80 Next                                    */
#define HI_702_ZOUTHGTH  0x1822 /* (P18) High byte of image height for image scaling 0x01 Next                                  */
#define HI_702_ZOUTHGTL  0x1823 /* (P18) Low byte of image height for image scaling 0xE0 Next                                   */
#define HI_702_ZWINSTXH  0x1824 /* (P18) High byte of start x position for windowing after image scaling 0x00 Next              */
#define HI_702_ZWINSTXL  0x1825 /* (P18) Low byte of start x position for windowing after image scaling 0x00 Next               */
#define HI_702_ZWINSTYH  0x1826 /* (P18) High byte of start y position for windowing after image scaling 0x00 Next              */
#define HI_702_ZWINSTYL  0x1827 /* (P18) Low byte of start y position for windowing after image scaling 0x00 Next               */
#define HI_702_ZWINENXH  0x1828 /* (P18) High byte of end x position for windowing after image scaling 0x01 Next                */
#define HI_702_ZWINENXL  0x1829 /* (P18) Low byte of end x position for windowing after image scaling 0x40 Next                 */
#define HI_702_ZWINENYH  0x182A /* (P18) High byte of end y position for windowing after image scaling 0x00 Next                */
#define HI_702_ZWINENYL  0x182B /* (P18) Low byte of end y position for windowing after image scaling 0xF0 Next                 */
#define HI_702_ZVERSTEPH 0x182C /* (P18) High byte of vertical scaling step 0x10 Next                                           */
#define HI_702_ZVERSTEPL 0x182D /* (P18) Low byte of vertical scaling step 0x00 Next                                            */
#define HI_702_ZHORSTEPH 0x182E /* (P18) High byte of horizontal scaling step 0x10 Next                                         */
#define HI_702_ZHORSTEPL 0x182F /* (P18) Low byte of horizontal scaling step 0x00 Next                                          */
#define HI_702_ZFIFODLY  0x1830 /* (P18) Delay depth for image scaling 0x55 Next                                                */
/* Auto Exposure (Page Mode = 20) */
#define HI_702_AECTL1     0x2010 /* (P20) Control automatic exposure 0x0C Next                                                  */
#define HI_702_AECTL2     0x2011 /* (P20) Control automatic exposure 0x04 Next                                                  */
#define HI_702_AEFRAMECTL 0x2020 /* (P20) Frame control for automatic exposure 0x01 Current                                     */
#define HI_702_AEFINECTL1 0x2028 /* (P20) Speed option for automatic exposure 0x03 Current                                      */
#define HI_702_AEFINECTL2 0x2029 /* (P20) Speed option for automatic exposure 0xA5 Current                                      */
#define HI_702_AEFINECTL3 0x202A /* (P20) Speed option for automatic exposure 0xF0 Current                                      */
#define HI_702_AEFINECTL4 0x202B /* (P20) Speed option for automatic exposure 0x35 Current                                      */
#define HI_702_AEWGT      0x2060 /* (P20) Weight coefficient of each AE region 0x95 Current                                     */
#define HI_702_AECENHST   0x2068 /* (P20) Horizontal start address for AE center area. 0x28 Current                             */
#define HI_702_AECENHEN   0x2069 /* (P20) Horizontal stop address for AE center area. 0x78 Current                              */
#define HI_702_AECENVST   0x206A /* (P20) Vertical start address for AE center area. 0x50 Current                               */
#define HI_702_AECENVED   0x206B /* (P20) Vertical stop address for AE center area. 0xA0 Current                                */
#define HI_702_YLVL       0x2070 /* (P20) Luminance level to converge in AE 0x50 Current                                        */
#define HI_702_YTH1       0x2078 /* (P20) Threshold1 of hysteresis in AE 0x45 Current                                           */
#define HI_702_YTH2       0x2079 /* (P20) Threshold2 of hysteresis in AE 0x28 Current                                           */
#define HI_702_EXPINTH    0x2080 /* (P20) High byte of internal exposure time RO Current                                        */
#define HI_702_EXPINTM    0x2081 /* (P20) Middle byte of internal exposure time RO Current                                      */
#define HI_702_EXPINTL    0x2082 /* (P20) Low byte of internal exposure time RO Current                                         */
#define HI_702_EXPTIMEH   0x2083 /* (P20) High byte of manual exposure time 0x00 Next                                           */
#define HI_702_EXPTIMEM   0x2084 /* (P20) Middle byte of manual exposure time 0xC3 Next                                         */
#define HI_702_EXPTIMEL   0x2085 /* (P20) Low byte of manual exposure time 0x50 Next                                            */
#define HI_702_EXPMINH    0x2086 /* (P20) High byte of minimum exposure time 0x01 Current                                       */
#define HI_702_EXPMINL    0x2087 /* (P20) Low byte of minimum exposure time 0xF4 Current                                        */
#define HI_702_EXPMAXH    0x2088 /* (P20) High byte of maximum exposure time 0x02 Current                                       */
#define HI_702_EXPMAXM    0x2089 /* (P20) Middle byte of maximum exposure time 0x49 Current                                     */
#define HI_702_EXPMAXL    0x208A /* (P20) Low byte of maximum exposure time 0xF0 Current                                        */
#define HI_702_EXP100H    0x208B /* (P20) High byte of exposure time for 1/100sec 0x3A Next                                     */
#define HI_702_EXP100L    0x208C /* (P20) Low byte of exposure time for 1/100sec 0x98 Next                                      */
#define HI_702_EXP120H    0x208D /* (P20) High byte of exposure time for 1/120sec 0x30 Next                                     */
#define HI_702_EXP120L    0x208E /* (P20) Low byte of exposure time for 1/120sec 0xD4 Next                                      */
#define HI_702_EXPDPCH    0x208F /* (P20) High byte of exposure time for dead pixel concealment 0x01 Current                    */
#define HI_702_EXPDPCL    0x2090 /* (P20) Low byte of exposure time for dead pixel concealment 0x86 Current                     */
#define HI_702_EXPFIXH    0x2091 /* (P20) High byte of exposure time for fixed frame rate 0x02 Current                          */
#define HI_702_EXPFIXM    0x2092 /* (P20) Middle byte of exposure time for fixed frame rate 0xF9 Current                        */
#define HI_702_EXPFIXL    0x2093 /* (P20) Low byte of exposure time for fixed frame rate 0xB8 Current                           */
#define HI_702_EXPOUT1    0x2098 /* (P20) Threshold of exposure time to define bright condition. 0x9A Current                   */
#define HI_702_EXPOUT2    0x2099 /* (P20) Threshold of exposure time to define very bright condition. 0x45 Current              */
#define HI_702_EXPLMTH    0x209C /* (P20) High byte of exposure time for low limit. 0x01 Current                                */
#define HI_702_EXPLMTL    0x209D /* (P20) Low byte of exposure time for low limit. 0xF4 Current                                 */
#define HI_702_EXPUNITH   0x209E /* (P20) High byte of unit step of EXPLMT 0x00 Current                                         */
#define HI_702_EXPUNITL   0x209F /* (P20) Low byte of unit step of EXPLMT 0xC8 Current                                          */
#define HI_702_AG         0x20B0 /* (P20) Automatic gain (AG) 0x10 Next                                                         */
#define HI_702_AGMIN      0x20B1 /* (P20) Minimum AG in AE 0x10 Current                                                         */
#define HI_702_AGMAX      0x20B2 /* (P20) Maximum AG in AE 0x80 Current                                                         */
#define HI_702_AGLVL      0x20B3 /* (P20) Target level of AG in AE 0x10 Current                                                 */
#define HI_702_AGTH1      0x20B4 /* (P20) Minimum threshold of Band AG in AE 0x10 Current                                       */
#define HI_702_AGTH2      0x20B5 /* (P20) Maximum threshold of Band AG in AE 0x33 Current                                       */
#define HI_702_AGBTH1     0x20B6 /* (P20) Threshold1 of Adaptive AG in AE. 0x22 Current                                         */
#define HI_702_AGBTH2     0x20B7 /* (P20) Threshold2 of Adaptive AG in AE. 0x1C Current                                         */
#define HI_702_AGBTH3     0x20B8 /* (P20) Threshold3 of Adaptive AG in AE. 0x19 Current                                         */
#define HI_702_AGBTH4     0x20B9 /* (P20) Threshold4 of Adaptive AG in AE. 0x17 Current                                         */
#define HI_702_AGBTH5     0x20BA /* (P20) Threshold5 of Adaptive AG in AE. 0x15 Current                                         */
#define HI_702_AGBTH6     0x20BB /* (P20) Threshold6 of Adaptive AG in AE. 0x14 Current                                         */
#define HI_702_AGBTH7     0x20BC /* (P20) Threshold7 of Adaptive AG in AE. 0x13 Current                                         */
#define HI_702_AGBTH8     0x20BD /* (P20) Threshold8 of Adaptive AG in AE. 0x12 Current                                         */
#define HI_702_AGSKY      0x20C0 /* (P20) Threshold of AG in very bright condition 0x14 Current                                 */
#define HI_702_AGDPCON    0x20C3 /* (P20) Threshold of AG to enter into dark condition at DPC operation 0x60 Current            */
#define HI_702_AGDPCOFF   0x20C4 /* (P20) Threshold of AG to escape from dark condition at DPC operation 0x58 Current           */
#define HI_702_DGDPCON    0x20C5 /* (P20) Threshold of DG to enter into dark condition at DPC operation 0xFF Current            */
#define HI_702_DGDPCOFF   0x20C6 /* (P20) Threshold of DG to escape from dark condition at DPC operation 0xFF Current           */
#define HI_702_DGMAX      0x20C8 /* (P20) Maximum Digital gain in AE. 0xFF Current                                              */
#define HI_702_DGMIN      0x20C9 /* (P20) Minimum Digital gain in AE. 0x90 Current                                              */
#define HI_702_YAVG       0x20D3 /* (P20) Average of luminance. RO                                                              */
/* Auto White Balance (Page Mode = 20) */
#define HI_702_AWBCTL1     0x2210 /* (P22) Control automatic white balance 0x6B Next                                            */
#define HI_702_AWBCTL2     0x2211 /* (P22) Control automatic white balance 0x28 Next                                            */
#define HI_702_ULVL U      0x2230 /* (P22) chrominance level to converge in AWB 0x80 Current                                    */
#define HI_702_VLVL V      0x2231 /* (P22) chrominance level to converge in AWB 0x80 Current                                    */
#define HI_702_UVTH1       0x2238 /* (P22) U/V chrominance threshold1 in AWB 0x13 Current                                       */
#define HI_702_UVTH2       0x2239 /* (P22) U/V chrominance threshold2 in AWB 0x66 Current                                       */
#define HI_702_YRANGE      0x2240 /* (P22) Valid luminance range to detect white pixel 0x88 Current                             */
#define HI_702_CDIFF       0x2241 /* (P22) Threshold of chrominance difference to detect white pixel 0x88 Current               */
#define HI_702_CSUM        0x2242 /* (P22) Threshold of chrominance summation to detect white pixel 0x66 Current                */
#define HI_702_WHTPXLTH    0x2246 /* (P22) Threshold of number of white pixel for AWB 0x0A Current                              */
#define HI_702_RGAIN       0x2280 /* (P22) Red color gain 0x30 Next                                                             */
#define HI_702_GGAIN       0x2281 /* (P22) Green color gain 0x20 Next                                                           */
#define HI_702_BGAIN       0x2282 /* (P22) Blue color gain 0x38 Next                                                            */
#define HI_702_RMAX        0x2283 /* (P22) Maximum RGAIN in AWB 0x50 Current                                                    */
#define HI_702_RMIN        0x2284 /* (P22) Minimum RGAIN in AWB 0x20 Current                                                    */
#define HI_702_BMAX        0x2285 /* (P22) Maximum BGAIN in AWB 0x50 Current                                                    */
#define HI_702_BMIN        0x2286 /* (P22) Minimum BGAIN in AWB 0x20 Current                                                    */
#define HI_702_RMAXB       0x2287 /* (P22) Upper limit of RGAIN in bright condition 0x50 Current                                */
#define HI_702_RMINB       0x2288 /* (P22) Lower limit of RGAIN in bright condition 0x3E Current                                */
#define HI_702_BMAXB       0x2289 /* (P22) Upper limit of BGAIN in bright condition 0x2E Current                                */
#define HI_702_BMINB       0x228A /* (P22) Lower limit of BGAIN in bright condition 0x20 Current                                */
#define HI_702_RBEXPLMT    0x228B /* (P22) Threshold of exposure time to change the boundary of RGAIN / BGAIN 0x08 Current      */
#define HI_702_RDELTA      0x228D /* (P22) Increasing (Decreasing) step of RGAIN 0x04 Current                                   */
#define HI_702_BDELTA      0x228E /* (P22) Increasing (Decreasing) step of BGAIN 0x60 Current                                   */
#define HI_702_BGAINPARA1  0x228F /* (P22) Parameter1 of BGAIN 0x50 Current                                                     */
#define HI_702_BGAINPARA2  0x2290 /* (P22) Parameter2 of BGAIN 0x4C Current                                                     */
#define HI_702_BGAINPARA3  0x2291 /* (P22) Parameter3 of BGAIN 0x48 Current                                                     */
#define HI_702_BGAINPARA4  0x2292 /* (P22) Parameter4 of BGAIN 0x46 Current                                                     */
#define HI_702_BGAINPARA5  0x2293 /* (P22) Parameter5 of BGAIN 0x44 Current                                                     */
#define HI_702_BGAINPARA6  0x2294 /* (P22) Parameter6 of BGAIN 0x42 Current                                                     */
#define HI_702_BGAINPARA7  0x2295 /* (P22) Parameter7 of BGAIN 0x40 Current                                                     */
#define HI_702_BGAINPARA8  0x2296 /* (P22) Parameter8 of BGAIN 0x3E Current                                                     */
#define HI_702_BGAINPARA9  0x2297 /* (P22) Parameter9 of BGAIN 0x3C Current                                                     */
#define HI_702_BGAINPARA10 0x2298 /* (P22) Parameter10 of BGAIN 0x28 Current                                                    */
#define HI_702_BGAINPARA11 0x2299 /* (P22) Parameter11 of BGAIN 0x26 Current                                                    */
#define HI_702_BGAINPARA12 0x229A /* (P22) Parameter12 of BGAIN 0x24 Current                                                    */
#define HI_702_BGAINBND    0x229B /* (P22) Boundary of BGAIN 0x04 Current                                                       */
#define HI_702_MRGAIN      0x22B2 /* (P22) RGAIN for manual white balance 0x48 Current                                          */
#define HI_702_MBGAIN      0x22B3 /* (P22) BGAIN for manual white balance 0x40 Current                                          */
#define HI_702_UAVG        0x22D2 /* (P22) Average of U chrominance of white pixels in frame RO                                 */
#define HI_702_VAVG        0x22D3 /* (P22) Average of V chrominance of white pixels in frame RO                                 */

#define reg_read(reg, val) hi702_read_reg(HI_702_##reg, val)
#define reg_write(reg, val) hi702_write_reg(HI_702_##reg, (val))
#define reg_set(reg, val) hi702_reg_set(HI_702_##reg, (val))
#define reg_clear(reg, val) hi702_reg_clear(HI_702_##reg, (val))

/* HI_702_PWRCTL bit definition */
#define HI_702_PWRCTL_SLEEP         (1 << 0)
#define HI_702_PWRCTL_SOFT_RESET    (1 << 1)
#define HI_702_PWRCTL_OUT_DRIVE_0   (0 << 4)
#define HI_702_PWRCTL_OUT_DRIVE_1   (1 << 4)
#define HI_702_PWRCTL_OUT_DRIVE_2   (2 << 4)
#define HI_702_PWRCTL_OUT_DRIVE_3   (3 << 4)
#define HI_702_PWRCTL_PCLK_DRIVE_0  (0 << 6)
#define HI_702_PWRCTL_PCLK_DRIVE_1  (1 << 6)
#define HI_702_PWRCTL_PCLK_DRIVE_2  (2 << 6)
#define HI_702_PWRCTL_PCLK_DRIVE_3  (3 << 6)

/* HI_702_VDOCTL1 bit definition */
#define HI_702_VDOCTL1_VSYNC_TYPE1  (0 << 3)
#define HI_702_VDOCTL1_VSYNC_TYPE2  (1 << 3)
#define HI_702_VDOCTL1_SUB_VGA      (0 << 4)
#define HI_702_VDOCTL1_SUB_QVGA     (1 << 4)
#define HI_702_VDOCTL1_SUB_QQVGA    (2 << 4)

/* HI_702_VDOCTL2 bit definition */
#define HI_702_VDOCTL2_X_FLIP       (1 << 0)
#define HI_702_VDOCTL2_Y_FLIP       (1 << 1)
#define HI_702_VDOCTL2_FIXED_RATE   (1 << 2)
#define HI_702_VDOCTL2_BAD_FRM_SKIP_1 (1 << 4)
#define HI_702_VDOCTL2_BAD_FRM_SKIP_2 (2 << 4)
#define HI_702_VDOCTL2_BAD_FRM_SKIP_3 (3 << 4)
#define HI_702_VDOCTL2_WINDOWING    (1 << 7)

/* HI_702_SYNCCTL bit definition */
#define HI_702_SYNCCTL_VSYNC_POL    (1 << 5)
#define HI_702_SYNCCTL_HSYNC_POL    (1 << 4)
#define HI_702_SYNCCTL_HSYNC_CLIP   (1 << 3)
#define HI_702_SYNCCTL_HSYNC_CLK_POL (1 << 2)
#define HI_702_SYNCCTL_HSYNC_CLK_DIV_8 (3 << 0)
#define HI_702_SYNCCTL_HSYNC_CLK_DIV_4 (2 << 0)
#define HI_702_SYNCCTL_HSYNC_CLK_DIV_2 (1 << 0)
#define HI_702_SYNCCTL_HSYNC_CLK_DIV_1 (0 << 0)

/* HI_702_ISPCTL1 bit definition */
#define HI_702_ISPCTL1_UPHASE_UYVY  (0 << 0)
#define HI_702_ISPCTL1_UPHASE_VYUY  (1 << 0)
#define HI_702_ISPCTL1_YPHASE_UYVY  (0 << 1)
#define HI_702_ISPCTL1_YPHASE_YUYV  (1 << 1)
#define HI_702_ISPCTL1_ITU656       (1 << 2)
#define HI_702_ISPCTL1_YUV422       (0 << 4)
#define HI_702_ISPCTL1_RGB565       (4 << 4)
#define HI_702_ISPCTL1_RGB444       (7 << 4)

/* HI_702_ISPCTL2 bit definition */
#define HI_702_ISPCTL2_CLR_INTERPOL (0 << 0)
#define HI_702_ISPCTL2_CSPCE_CONV   (1 << 1)
#define HI_702_ISPCTL2_SKETCH       (1 << 4)
#define HI_702_ISPCTL2_EMBOSSING    (1 << 5)
#define HI_702_ISPCTL2_BYTE_SWAP    (1 << 7)

/* HI_702_ISPCTL3 bit definition */
#define HI_702_ISPCTL3_U_CONST      (0 << 0)
#define HI_702_ISPCTL3_V_CONST      (1 << 1)
#define HI_702_ISPCTL3_CLR_INV      (1 << 2)
#define HI_702_ISPCTL3_NEG          (1 << 3)
#define HI_702_ISPCTL3_Y_OFFSET     (1 << 4)
#define HI_702_ISPCTL3_AUTOBRIGHT   (1 << 5)
#define HI_702_ISPCTL3_U_OFFSET     (1 << 6)
#define HI_702_ISPCTL3_V_OFFSET     (1 << 7)

/* HI_702_SATCTL bit definition */
#define HI_702_SATCTL_MANCOLORSAT   (1 << 0)
#define HI_702_SATCTL_AUTOCOLORSAT  (1 << 1)
#define HI_702_SATCTL_SUPPRESSION_0 (0 << 2)
#define HI_702_SATCTL_SUPPRESSION_1 (1 << 2)
#define HI_702_SATCTL_SUPPRESSION_2 (2 << 2)
#define HI_702_SATCTL_SUPPRESSION_3 (3 << 2)

/* HI_702_AECTL1 bit definition */
#define HI_702_AECTL1_AE_ENABLE     (1 << 7)
#define HI_702_AECTL1_ANTIFLKR_ENABLE  (1 << 6)
#define HI_702_AECTL1_100HZ         (1 << 4)
#define HI_702_AECTL1_120HZ         (0 << 4)

/* HI_702_AWBCTL1 bit definition */
#define HI_702_AWBCTL1_AWB_ENABLE    (1 << 7)
#define HI_702_AWBCTL1_ABW_SPD_7     (7 << 4)
#define HI_702_AWBCTL1_ABW_SPD_6     (6 << 4)
#define HI_702_AWBCTL1_ABW_SPD_5     (5 << 4)
#define HI_702_AWBCTL1_ABW_SPD_4     (4 << 4)
#define HI_702_AWBCTL1_ABW_SPD_3     (3 << 4)
#define HI_702_AWBCTL1_ABW_SPD_2     (2 << 4)
#define HI_702_AWBCTL1_ABW_SPD_1     (1 << 4)
#define HI_702_AWBCTL1_ABW_SPD_0     (0 << 4)
#define HI_702_AWBCTL1_ABW_SPD_OPT   (0x0B << 0)

enum {
	HI_702_FLICKER_DISABLE = 0,
	HI_702_FLICKER_MANUAL_50,
	HI_702_FLICKER_MANUAL_60,
	HI_702_FLICKER_AUTO_DETECTION
};

#define HI_702_I2C_ADDRESS  0x30
#endif
