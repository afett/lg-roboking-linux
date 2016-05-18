/*
 *     Copyright (c) 2011 MOSTiTECH co., ltd.
 *    All right reserved. by Seungwoo Kim. <ksw@mostitech.com>
 *    
 *    Hynix camera module YACC6A1SBDBS(Hi-161) driver for NEXELL.
 */
#ifndef __HI_161_CAM_H__
#define __HI_161_CAM_H__
/*
0x03(P0) PAGEMODE
0x00 = control image size, windowing, sync, black level calibration
0x10 = control image format, image effect
0x11 = control noise reduction(D-LPF)
0x12 = control noise reduction(YC-LPF).
0x13 = control edge enhancement
0x14 = control lens shading correction.
0x15 = control color correction.
0x16 = control gamma correction.
0x17 = control auto flicker cancellation
0x18 = control image scaling
0x20 = control auto exposure
0x22 = control auto white balance.
*/

#define HI161_PAGE_MAP      0x03
/* Device ID and Image Size and Windowing and Sync (Page Mode = 0) */
#define HI_161_PWRCTL       0x0001 /* (P0) Power sleep mode 0x51 Current                                                             */
#define HI_161_DEVID        0x0004 /* (P0) Device ID1 0x8C(RO)                                                                       */
#define HI_161_PLLCTL1      0x000E /* (P0) Control PLL 0x00                                                                          */
#define HI_161_PLLCTL2      0x000F /* (P0) Control PLL 0x00                                                                          */
#define HI_161_VDOCTL1      0x0010 /* (P0) Control sub-sampling and vsync type 0x00 Next                                             */
#define HI_161_VDOCTL2      0x0011 /* (P0) Control strobe, windowing, fixed frame rate, X/Y flip and skip frame 0x90 Next            */
#define HI_161_SYNCCTL      0x0012 /* (P0) Control polarity of H/VSYNC and PCLK, and Internal clock divider 0x04 Next                */
#define HI_161_WINROWH      0x0020 /* (P0) High byte of row start address for windowing 0x00 Next                                    */
#define HI_161_WINROWL      0x0021 /* (P0) Low byte of row start address for windowing 0x05 Next                                     */
#define HI_161_WINCOLH      0x0022 /* (P0) High byte of column start address for windowing 0x00 Next                                 */
#define HI_161_WINCOLL      0x0023 /* (P0) Low byte of column start address for windowing 0x07 Next                                  */
#define HI_161_WINHGTH      0x0024 /* (P0) High byte of height for windowing 0x01 Next                                               */
#define HI_161_WINHGTL      0x0025 /* (P0) Low byte of height for windowing 0xE0 Next                                                */
#define HI_161_WINWIDH      0x0026 /* (P0) High byte of width for windowing 0x02 Next                                                */
#define HI_161_WINWIDL      0x0027 /* (P0) Low byte of width for windowing 0x80 Next                                                 */
#define HI_161_HBLANKH      0x0040 /* (P0) High byte of duration for horizontal blanking 0x01 Next                                   */
#define HI_161_HBLANKL      0x0041 /* (P0) Low byte of duration for horizontal blanking 0x50 Next                                    */
#define HI_161_VSYNCH       0x0042 /* (P0) High byte of duration for vertical synchronization(Type1) 0x00 Next                       */
#define HI_161_VSYNCL       0x0043 /* (P0) Low byte of duration for vertical synchronization(Type1) 0x14 Next                        */
#define HI_161_VSCLIP       0x0044 /* (P0) Clipping duration in vertical synchronization 0x09 Current                                */
#define HI_161_VSCTL1       0x0045 /* (P0) High 4bits of start/stop row position for vertical synchronization(Type2) 0x01 Current    */
#define HI_161_VSCTL2       0x0046 /* (P0) Low byte of start row position for vertical synchronization(Type2) 0x02 Current           */
#define HI_161_VSCTL3       0x0047 /* (P0) Low byte of stop row position for vertical synchronization(Type2) 0xE2 Current            */
/* Black level calibration (Page Mode = 0) */
#define HI_161_BLCCTL       0x0080 /* (P0) Control optical black level calibration 0x08 Next                                    */
#define HI_161_BLCTIMETHON  0x0090 /* (P0) Exposure time threshold to enable additional BLC 0x07 Next                           */
#define HI_161_BLCTIMETHOFF 0x0091 /* (P0) Exposure time threshold to disable additional BLC 0x08 Next                          */
#define HI_161_BLCAGTHH     0x0092 /* (P0) AG threshold to enable additional BLC 0x60 Next                                      */
#define HI_161_BLCAGTHL     0x0093 /* (P0) AG threshold to disable additional BLC 0x60 Next                                     */
#define HI_161_BLCDGH       0x0094 /* (P0) Digital gain threshold to enable additional BLC 0xC8 Next                            */
#define HI_161_BLCDGL       0x0095 /* (P0) Digital gain threshold to disable additional BLC 0xC0 Next                           */
#define HI_161_BLCOFSDB     0x00A0 /* (P0) Blue color1 offset for additional BLC 0x00 Current                                   */
#define HI_161_BLOFSDGB     0x00A2 /* (P0) Gb color1 offset for additional BLC 0x00 Current                                     */
#define HI_161_BLCOFSDR     0x00A4 /* (P0) Red color1 offset for additional BLC 0x00 Current                                    */
#define HI_161_BLCOFSDGR    0x00A6 /* (P0) Gr color1 offset for additional BLC 0x00 Current                                     */
#define HI_161_STRCTL       0x00C0 /* (P0) Control strobe function. (XENON Type and LED Type) 0x09 Current                      */
#define HI_161_STRWID       0x00C1 /* (P0) Strobe pulse width 0x0C Current                                                      */
#define HI_161_STRTIME      0x00C2 /* (P0) Exposure time to enable strobe function 0x43 Current                                 */
/* Image Output Format and Image Effect (Page Mode = 10) */
#define HI_161_ISPCTL1      0x1010 /* (P10)                                                                                     */
#define HI_161_ISPCTL2      0x1011 /* (P10) Control image effect and color interpolation and color space conversion 0x03 Next   */
#define HI_161_ISPCTL3      0x1012 /* (P10) Control color offset and brightness 0x00 Next                                       */
#define HI_161_ISPCTL4      0x1013 /* (P10) Control the image effect 0x00 Current                                               */
#define HI_161_ITUCTL       0x1020 /* (P10) Control the ITU656-like mode. 0x00 Current                                          */
#define HI_161_ITUSOF       0x1024 /* (P10) Start of frame data for ITU656-like 0x20 Current                                    */
#define HI_161_ITUSOL       0x1025 /* (P10) Start of line data for ITU656-like 0x10 Current                                     */
#define HI_161_ITUEOF       0x1026 /* (P10) End of frame data for ITU656-like 0x01 Current                                      */
#define HI_161_ITUEOL       0x1027 /* (P10) End of line data for ITU656-like 0x02 Current                                       */
#define HI_161_YOFS         0x1040 /* (P10) Offset of luminance 0x00 Current                                                    */
#define HI_161_UOFS         0x1042 /* (P10) Offset of U chrominance 0x00 Next                                                   */
#define HI_161_VOFS         0x1043 /* (P10) Offset of V chrominance 0x00 Next                                                   */
#define HI_161_UCON         0x1044 /* (P10) Constant of U chrominance 0x80 Next                                                 */
#define HI_161_VCON         0x1045 /* (P10) Constant of V chrominance 0x80 Next                                                 */
#define HI_161_SOLARI       0x1046 /* (P10) Solarization coefficient. 0xF0 Next                                                 */
#define HI_161_BINARY       0x1047 /* (P10) Threshold of binary effect. 0x7F Next                                               */
#define HI_161_CONTRAST     0x1048 /* (P10) Threshold of binary effect. 0x7F Next                                               */
#define HI_161_DYOFS        0x1053 /* (P10) Offset of luminance at dark condition 0x48 Next                                     */
#define HI_161_DYOFSMAX     0x1055 /* (P10) Threshold of luminance 0x30 Next                                                    */
#define HI_161_SATCTL       0x1060 /* (P10) Control color saturation 0x00 Current                                               */
#define HI_161_SATB         0x1062 /* (P10) Blue color saturation coefficient 0x80 Current                                      */
#define HI_161_SATR         0x1063 /* (P10) Red color saturation coefficient 0x80 Current                                       */
#define HI_161_AGSAT        0x1064 /* (P10) Threshold of AG in auto saturation 0x60 Current                                     */
#define HI_161_SATTIMETH    0x1066 /* (P10) Time threshold of color saturation 0xF4 Current                                     */
#define HI_161_SATOUTDEL    0x1067 /* (P10) Offset of color saturation 0x00 Current                                             */
/* D-LPF (Page Mode = 11) */
#define HI_161_DLPFCTL1     0x1110 /* (P11) Control D-LPF 0x00 Current                                                          */
#define HI_161_DLPFCTL2     0x1111 /* (P11) Control D-LPF 0x00 Current                                                          */
#define HI_161_DLPFAUTOCTL1 0x1120 /* (P11) Option for D-LPF. 0x00 Current                                                      */
#define HI_161_DLPFAUTOCTL2 0x1121 /* (P11) Option for D-LPF. 0x00 Current                                                      */
#define HI_161_LPFAGTHL     0x1126 /* (P11) Hysteresis of Low threshold between Indoor mode and Dark1 mode 0x38 Current         */
#define HI_161_LPFAGTHH     0x1127 /* (P11) Hysteresis of High threshold between Indoor mode and Dark1 mode 0x30 Current        */
#define HI_161_LPFOUTTHL    0x1128 /* (P11) Hysteresis of Low threshold between outdoor2 mode and outdoor1 mode. 0x0F Current   */
#define HI_161_LPFOUTTHH    0x1129 /* (P11) Hysteresis of High threshold between outdoor2 mode and outdoor1 mode 0x10 Current   */
#define HI_161_LPFYMEANTHL  0x112B /* (P11) Hysteresis of high threshold value between dark2 mode and dark3 mode. 0x40 Current  */
#define HI_161_LPFYMEANTHH  0x112C /* (P11) Hysteresis of low threshold value between dark2 mode and dark3 mode. 0x42 Current   */
#define HI_161_OUT2YBOUNDH  0x1130 /* (P11) High luminance boundary under outdoor2 mode. 0xBA Current                           */
#define HI_161_OUT2YBOUNDL  0x1131 /* (P11) Low luminance boundary under outdoor2 mode 0x10 Current                             */
#define HI_161_OUT2RATIO    0x1132 /* (P11) D-LPFratio under outdoor2 mode 0x80 Current                                         */
#define HI_161_OUT2THH      0x1133 /* (P11) Threshold of D-LPF in high luminance region under outdoor2 mode 0x20 Current        */
#define HI_161_OUT2THM      0x1134 /* (P11) Threshold of D-LPF in middle luminance region under outdoor2 mode 0x08 Current      */
#define HI_161_OUT2THL      0x1135 /* (P11) Threshold of D-LPF in low luminance region under outdoor2 mode 0x04 Current         */
#define HI_161_OUT1YBOUNDH  0x1136 /* (P11) High luminance boundary under outdoor1 mode. 0xB0 Current                           */
#define HI_161_OUT1YBOUNDL  0x1137 /* (P11) Low luminance boundary under outdoor1 mode 0x18 Current                             */
#define HI_161_OUT1RATIO    0x1138 /* (P11) D-LPF ratio under outdoor1 mode. 0x80 Current                                       */
#define HI_161_OUT1THH      0x1139 /* (P11) Threshold of D-LPF in high luminance region under outdoor1 mode 0x28 Current        */
#define HI_161_OUT1THM      0x113A /* (P11) Threshold of D-LPF in middle luminance region under outdoor1 mode 0x10 Current      */
#define HI_161_OUT1THL      0x113B /* (P11) Threshold of D-LPF in low luminance region under outdoor1 mode 0x08 Current         */
#define HI_161_INYBOUNDH    0x113C /* (P11) High luminance boundary under indoor mode 0xA0 Current                              */
#define HI_161_INYBOUNDL    0x113D /* (P11) Low luminance boundary under indoor mode 0x20 Current                               */
#define HI_161_INRATIO      0x113E /* (P11) D-LPF ratio under indoor mode 0x80 Current                                          */
#define HI_161_INTHH        0x113F /* (P11) Threshold of D-LPF in high luminance region under indoor mode 0x40 Current          */
#define HI_161_INTHM        0x1140 /* (P11) Threshold of D-LPF in middle luminance region under indoor mode 0x20 Current        */
#define HI_161_INTHL        0x1141 /* (P11) Threshold of D-LPF in low luminance region under indoor mode 0x10 Current           */
#define HI_161_DARK1YBNDH   0x1142 /* (P11) High luminance boundary under dark1 mode 0x98 Current                               */
#define HI_161_DARK_1YBNDL  0x1143 /* (P11) Low luminance boundary under dark1 mode 0x28 Current                                */
#define HI_161_DARK_1RATIO  0x1144 /* (P11) D-LPF ratio under dark1 mode 0x80 Current                                           */
#define HI_161_DARK_1THH    0x1145 /* (P11) Threshold of D-LPF in high luminance region under dark1 mode 0x48 Current           */
#define HI_161_DARK_1THM    0x1146 /* (P11) Threshold of D-LPF in middle luminance region under dark1 mode 0x30 Current         */
#define HI_161_DARK_1THL    0x1147 /* (P11) Threshold of D-LPF in low luminance region under dark1 mode 0x20 Current            */
#define HI_161_DARK2YBNDH   0x1148 /* (P11) High luminance boundary under dark2 mode 0x90 Current                               */
#define HI_161_DARK_2YBNDL  0x1149 /* (P11) Low luminance boundary under dark2 mode. 0x2A Current                               */
#define HI_161_DARK_2RATIO  0x114A /* (P11) D-LPFratio under dark2 mode 0x80 Current                                            */
#define HI_161_DARK_2THH    0x114B /* (P11) Threshold of D-LPF in high luminance region under dark2 mode 0x50 Current           */
#define HI_161_DARK_2THM    0x114C /* (P11) Threshold of D-LPF in middle luminance region under dark2 mode 0x38 Current         */
#define HI_161_DARK_2THL    0x114D /* (P11) Threshold of D-LPF in low luminance region under dark2 mode 0x38 Current            */
#define HI_161_DARK3YBNDH   0x114E /* (P11) High luminance boundary under dark3 mode 0x80 Current                               */
#define HI_161_DARK_3YBNDL  0x114F /* (P11) Low luminance boundary under dark3 mode. 0x30 Current                               */
#define HI_161_DARK_3RATIO  0x1150 /* (P11) D-LPF ratio under dark3 mode 0x80 Current                                           */
#define HI_161_DARK_3THH    0x1151 /* (P11) Threshold of D-LPF in high luminance region under dark3 mode 0x80 Current           */
#define HI_161_DARK_3THM    0x1152 /* (P11) Threshold of D-LPF in middle luminance region under dark3 mode 0x40 Current         */
#define HI_161_DARK_3THL    0x1153 /* (P11) Threshold of D-LPF in low luminance region under dark3 mode 0x48 Current            */
/* YC-LPF (Page Mode = 12) */
#define HI_161_YCLPFCTL1    0x1220 /* (P12) Control YC-LPF 0x12 Current                                                         */
#define HI_161_YCLPFCTL2    0x1221 /* (P12) Control YC-LPF 0x06 Current                                                         */
#define HI_161_YCLPFCTL4    0x1223 /* (P12) Control YC-LPF 0x00 Current                                                         */
#define HI_161_YCPRWTH      0x1230 /* (P12) Threshold of Texture region 0x58 Current                                            */
#define HI_161_YCUNI1TH     0x1231 /* (P12) Threshold for uniform1 region 0x16 Current                                          */
#define HI_161_YCUNI2TH     0x1232 /* (P12) Threshold for uniform2 region 0x30 Current                                          */
#define HI_161_YCUNI3TH     0x1233 /* (P12) Threshold for uniform3 region 0x3A Current                                          */
#define HI_161_YCNOR1TH     0x1234 /* (P12) Threshold for normal1 region 0x3A Current                                           */
#define HI_161_YCNOR2TH     0x1235 /* (P12) Threshold for normal2 region 0x64 Current                                           */
#define HI_161_YCNOR3TH     0x1236 /* (P12) Threshold for normal3 region 0x96 Current                                           */
#define HI_161_YCOUT2THH    0x1240 /* (P12) High luminance boundary under outdoor2 mode 0xB8 Current                            */
#define HI_161_YCOUT2THL    0x1241 /* (P12) Low luminance boundary under outdoor2 mode 0x20 Current                             */
#define HI_161_YCOUT2STDH   0x1242 /* (P12) Threshold of Y-LPF in high luminance region under outdoor2 mode 0xA7 Current        */
#define HI_161_YCOUT2STDM   0x1243 /* (P12) Threshold of Y-LPF in middle luminance region under outdoor2 mode 0xA7 Current      */
#define HI_161_YCOUT2STDL   0x1244 /* (P12) Threshold of Y-LPF in low luminance region under outdoor2 mode 0xA7 Current         */
#define HI_161_YCOUT2RAT    0x1245 /* (P12) The ratio of Y-LPF under outdoor2 mode 0x40 Current                                 */
#define HI_161_YCOUT1THH    0x1246 /* (P12) High luminance boundary under outdoor1 mode 0xB0 Current                            */
#define HI_161_YCOUT1THL    0x1247 /* (P12) Low luminance boundary under outdoor1 mode 0x20 Current                             */
#define HI_161_YCOUT1STDH   0x1248 /* (P12) Threshold of Y-LPF in high luminance region under outdoor1 mode 0xAF Current        */
#define HI_161_YCOUT1STDM   0x1249 /* (P12) Threshold of Y-LPF in middle luminance region under outdoor1 mode 0xAF Current      */
#define HI_161_YCOUT1STDL   0x124A /* (P12) Threshold of Y-LPF in low luminance region under outdoor1 mode 0xAF Current         */
#define HI_161_YCOUT1RAT    0x124B /* (P12) The ratio of Y-LPF under outdoor1 mode 0x60 Current                                 */
#define HI_161_YCINTHH      0x124C /* (P12) High luminance boundary under indoor mode 0xB0 Current                              */
#define HI_161_YCINTHL      0x124D /* (P12) Low luminance boundary under indoor mode 0x30 Current                               */
#define HI_161_YCINSTDH     0x124E /* (P12) Threshold of Y-LPF in high luminance region under indoor mode 0xB7 Current          */
#define HI_161_YCINSTDM     0x124F /* (P12) Threshold of Y-LPF in middle luminance region under indoor mode 0xB7 Current        */
#define HI_161_YCINSTDL     0x1250 /* (P12) Threshold of Y-LPF in low luminance region under indoor mode 0xB7 Current           */
#define HI_161_YCINRAT      0x1251 /* (P12) The ratio of Y-LPF under indoor mode 0x80 Current                                   */
#define HI_161_YCDARK1THH   0x1252 /* (P12) High luminance boundary under dark1 mode 0xA8 Current                               */
#define HI_161_YCDARK1THL   0x1253 /* (P12) Low luminance boundary under dark1 mode 0x30 Current                                */
#define HI_161_YCDARK1STDH  0x1254 /* (P12) Threshold of Y-LPF in high luminance region under dark1 mode 0xC7 Current           */
#define HI_161_YCDARK1STDM  0x1255 /* (P12) Threshold of Y-LPF in middle luminance region under dark1 mode 0xC7 Current         */
#define HI_161_YCDARK1STDL  0x1256 /* (P12) Threshold of Y-LPF in low luminance region under dark1 mode 0xC7 Current            */
#define HI_161_YCDARK1RAT   0x1257 /* (P12) The ratio of Y-LPF under dark1 mode 0x80 Current                                    */
#define HI_161_YCDARK2THH   0x1258 /* (P12) High luminance boundary under dark2 mode 0xA0 Current                               */
#define HI_161_YCDARK2THL   0x1259 /* (P12) Low luminance boundary under dark2 mode 0x40 Current                                */
#define HI_161_YCDARK2STDH  0x125A /* (P12) Threshold of Y-LPF in high luminance region under dark2 mode 0xDF Current           */
#define HI_161_YCDARK2STDM  0x125B /* (P12) Threshold of Y-LPF in middle luminance region under dark2 mode 0xDF Current         */
#define HI_161_YCDARK2STDL  0x125C /* (P12) Threshold of Y-LPF in low luminance region under dark2 mode 0xDF Current            */
#define HI_161_YCDARK2RAT   0x125D /* (P12) The ratio of Y-LPF under dark2 mode 0x80 Current                                    */
#define HI_161_YCDARK3THH   0x125E /* (P12) High luminance boundary under dark3 mode 0xA0 Current                               */
#define HI_161_YCDARK3THL   0x125F /* (P12) Low luminance boundary under dark3 mode 0x40 Current                                */
#define HI_161_YCDARK3STDH  0x1260 /* (P12) Threshold of Y-LPF in high luminance region under dark3 mode 0xEF Current           */
#define HI_161_YCDARK3STDM  0x1261 /* (P12) Threshold of Y-LPF in middle luminance region under dark3 mode 0xEF Current         */
#define HI_161_YCDARK3STDL  0x1262 /* (P12) Threshold of Y-LPF in low luminance region under dark3 mode 0xEF Current            */
#define HI_161_YCDARK3RAT   0x1263 /* (P12) The ratio of Y-LPF under dark3 mode 0x80 Current                                    */
/* Edge Enhancement (Page Mode = 13) */
#define HI_161_EDGECTL1      0x1310 /* (P13) Control edge enhancement 0x18 Current                                              */
#define HI_161_EDGECTL2      0x1311 /* (P13) Control edge enhancement. 0x06 Current                                             */
#define HI_161_EDGECTL3      0x1312 /* (P13) Control edge enhancement. 0x06 Current                                             */
#define HI_161_EDGECTL5      0x1314 /* (P13) Control edge enhancement. 0x00 Current                                             */
#define HI_161_EDGENGAIN     0x1320 /* (P13) Edge gain to emphasize negative edge data 0x08 Current                             */
#define HI_161_EDGEPGAIN     0x1321 /* (P13) Edge gain to emphasize positive edge data 0x08 Current                             */
#define HI_161_EDGEHCLIP1TH  0x1323 /* (P13) High clip1 threshold. 0x06 Current                                                 */
#define HI_161_EDGEHCLIP2TH  0x1324 /* (P13) High clip1 threshold. 0x0C Current                                                 */
#define HI_161_EDGELCLIPTH   0x1325 /* (P13) Low clip threshold. 0x08 Current                                                   */
#define HI_161_EDGELCLIPLMT  0x1326 /* (P13) Limit of low clip. 0x30 Current                                                    */
#define HI_161_EDGETIMETH    0x1329 /* (P13) Threshold of Exposure time for automatic edge enhancement. 0x10 Current            */
#define HI_161_EDGEAGTH      0x132A /* (P13) Threshold of AG for automatic edge enhancement. 0x30 Current                       */
#define HI_161_EDGE2DCTL1    0x1380 /* (P13) Control 2nd edge enhancement 0x00 Current                                          */
#define HI_161_EDGE2DCTL2    0x1381 /* (P13) Control 2nd edge enhancement 0x07 Current                                          */
#define HI_161_EDGE2DCTL3    0x1382 /* (P13) Control 2nd edge enhancement 0x06 Current                                          */
#define HI_161_EDGE2DCTL4    0x1383 /* (P13) Control 2nd edge enhancement 0x21 Current                                          */
#define HI_161_EDGE2DNGAIN   0x1390 /* (P13) Edge gain to emphasize negative edge data for 2nd edge enhancement 0x07 Current    */
#define HI_161_EDGE2DPGAIN   0x1391 /* (P13) Edge gain to emphasize positive edge data for 2nd edge enhancement 0x07 Current    */
#define HI_161_EDGE2DHCLIPTH 0x1393 /* (P13) High clip threshold for 2nd edge enhancement 0x30 Current                          */
#define HI_161_EDGE2DLCLIPTH 0x1394 /* (P13) Low clip threshold for 2nd edge enhancement 0x00 Current                           */
#define HI_161_EDGE2DHCLIP2TH 0x1395 /* (P13) High clip2 threshold for 2nd edge enhancement 0x1E Current                        */
#define HI_161_EDGE2DLCOUT2N  0x13A0 /* (P13) Negative threshold of low clip under outdoor2 mode 0x00 Current                   */
#define HI_161_EDGE2DLCOUT2P  0x13A1 /* (P13) Positive threshold of low clip under outdoor2 mode 0x00 Current                   */
#define HI_161_EDGE2DLCOUT1N  0x13A2 /* (P13) Negative threshold of low clip under outdoor1 mode 0x01 Current                   */
#define HI_161_EDGE2DLCOUT1P  0x13A3 /* (P13) Positive threshold of low clip under outdoor1 mode 0x02 Current                   */
#define HI_161_EDGE2DLCINN    0x13A4 /* (P13) Negative threshold of low clip under indoor mode 0x02 Current                     */
#define HI_161_EDGE2DLCINP    0x13A5 /* (P13) Positive threshold of low clip under indoor mode 0x04 Current                     */
#define HI_161_EDGE2DLCD1N    0x13A6 /* (P13) Negative threshold of low clip under dark1 mode 0x03 Current                      */
#define HI_161_EDGE2DLCD1P    0x13A7 /* (P13) Positive threshold of low clip under dark1 mode 0x06 Current                      */
#define HI_161_EDGE2DLCD2N    0x13A8 /* (P13) Negative threshold of low clip under dark2 mode 0x03 Current                      */
#define HI_161_EDGE2DLCD2P    0x13A9 /* (P13) Positive threshold of low clip under dark2 mode 0x06 Current                      */
#define HI_161_EDGE2DLCD3N    0x13AA /* (P13) Negative threshold of low clip under dark3 mode 0x04 Current                      */
#define HI_161_EDGE2DLCD3P    0x13AB /* (P13) Positive threshold of low clip under dark3 mode 0x08 Current                      */
/* Lens Shading (Page Mode = 14) */
#define HI_161_LENSCTL1       0x1410 /* (P14) Control lens shading correction 0x00 Current                                          */
#define HI_161_GXCEN          0x1420 /* (P14) Optical center onGreen X-axis in pixel array for lens shading correction 0x80 Current */
#define HI_161_GYCEN          0x1421 /* (P14) Optical center on GreenY-axis in pixel array for lens shading correction 0x80 Current */
#define HI_161_RXCEN          0x1422 /* (P14) Optical center on Red X-axis in pixel array for lens shading correction 0x80 Current  */
#define HI_161_RYCEN          0x1423 /* (P14) Optical center on Red Y-axis in pixel array for lens shading correction 0x80 Current  */
#define HI_161_BXCEN          0x1424 /* (P14) Optical center on Blue X-axis in pixel array for lens shading correction 0x80 Current */
#define HI_161_BYCEN          0x1425 /* (P14) Optical center on Blue Y-axis in pixel array for lens shading correction 0x80 Current */
#define HI_161_LAGOFF         0x1428 /* (P14) AG threshold to disable Lens Shading Correction Gain 0x60 Current                     */
#define HI_161_LAGON          0x1429 /* (P14) AG threshold to enable Lens Shading Correction Gain 0x60 Current                      */
#define HI_161_LENSRGAIN      0x1440 /* (P14) Lens Shading Correction Global Gain for Red pixel 0x00 Current                        */
#define HI_161_LENSGrGAIN     0x1441 /* (P14) Lens Shading Correction Global Gain for Green Pixel 0x00 Current                      */
#define HI_161_LENSBGAIN      0x1442 /* (P14) Lens Shading Correction Global Gain for Blue Pixel 0x00 Current                       */
#define HI_161_LENSGbGAIN     0x1443 /* (P14) Lens Shading Correction Global Gain for Green Pixel 0x00 Current                      */
/* Color Correction (Page Mode = 15) */
#define HI_161_CMCCTL         0x1510 /* (P15) Control color correction 0x0F Current                                                   */
#define HI_161_CMCOFSGH       0x1514 /* (P15) High threshold of color offset gain 0x40 Current                                        */
#define HI_161_CMCOFSGM       0x1515 /* (P15) Middle threshold of color offset gain 0x30 Current                                      */
#define HI_161_CMCOFSGL       0x1516 /* (P15) Low threshold of color offset gain 0x20 Current                                         */
#define HI_161_CMCSIGN        0x1517 /* (P15) Sign bit of color correction coefficient 0x2F Current                                   */
#define HI_161_CMC11          0x1530 /* (P15) Color correction coefficient 11 0x4C Current                                            */
#define HI_161_CMC12          0x1531 /* (P15) Color correction coefficient 12 0x0C Current                                            */
#define HI_161_CMC13          0x1532 /* (P15) Color correction coefficient 13 0x00 Current                                            */
#define HI_161_CMC21          0x1533 /* (P15) Color correction coefficient 21 0x26 Current                                            */
#define HI_161_CMC22          0x1534 /* (P15) Color correction coefficient 22 0x80 Current                                            */
#define HI_161_CMC23          0x1535 /* (P15) Color correction coefficient 23 0x1A Current                                            */
#define HI_161_CMC31          0x1536 /* (P15) Color correction coefficient 31 0x20 Current                                            */
#define HI_161_CMC32          0x1537 /* (P15) Color correction coefficient 32 0x80 Current                                            */
#define HI_161_CMC33          0x1538 /* (P15) Color correction coefficient 33 0xE0 Current                                            */
#define HI_161_CMCOFSL11      0x1540 /* (P15) Color correction offset coefficient 11 0x12 Current                                     */
#define HI_161_CMCOFSL12      0x1541 /* (P15) Color correction offset coefficient 12 0xA0 Current                                     */
#define HI_161_CMCOFSL13      0x1542 /* (P15) Color correction offset coefficient 13 0x0E Current                                     */
#define HI_161_CMCOFSL21      0x1543 /* (P15) Color correction offset coefficient 21 0x84 Current                                     */
#define HI_161_CMCOFSL22      0x1544 /* (P15) Color correction offset coefficient 22 0x08 Current                                     */
#define HI_161_CMCOFSL23      0x1545 /* (P15) Color correction offset coefficient 23 0x84 Current                                     */
#define HI_161_CMCOFSL31      0x1546 /* (P15) Color correction offset coefficient 31 0x8A Current                                     */
#define HI_161_CMCOFSL32      0x1547 /* (P15) Color correction offset coefficient 32 0x02 Current                                     */
#define HI_161_CMCOFSL33      0x1548 /* (P15) Color correction offset coefficient 33 0x08 Current                                     */
#define HI_161_CMCOFSH11      0x1550 /* (P15) Color correction offset coefficient 11 for high color temperature 0x12 Current              */     
#define HI_161_CMCOFSH12      0x1551 /* (P15) Color correction offset coefficient 12 for high color temperature 0xA0 Current              */
#define HI_161_CMCOFSH13      0x1552 /* (P15) Color correction offset coefficient 13 for high color temperature 0x0E Current              */
#define HI_161_CMCOFSH21      0x1553 /* (P15) Color correction offset coefficient 21 for high color temperature 0x84 Current              */
#define HI_161_CMCOFSH22      0x1554 /* (P15) Color correction offset coefficient 22 for high color temperature 0x08 Current              */
#define HI_161_CMCOFSH23      0x1555 /* (P15) Color correction offset coefficient 23 for high color temperature 0x84 Current              */
#define HI_161_CMCOFSH31      0x1556 /* (P15) Color correction offset coefficient 31 for high color temperature 0x8A Current              */
#define HI_161_CMCOFSH32      0x1557 /* (P15) Color correction offset coefficient 32 for high color temperature 0x02 Current              */
#define HI_161_CMCOFSH33      0x1558 /* (P15) Color correction offset coefficient 33 for high color temperature 0x08 Current              */
/* Gamma Correction (Page Mode = 16) */
#define HI_161_GMACTL        0x1610 /* (P16) Control gamma correction 0x01 Current                                                           */
#define HI_161_GGMA0         0x1630 /* (P16) Gamma corrected output of green pixel at 0(0)code in 10(8)bit linear input 0x00 Current         */
#define HI_161_GGMA1         0x1631 /* (P16) Gamma corrected output of green pixel at 16(4)code in 10(8)bit linear input 0x15 Current        */
#define HI_161_GGMA2         0x1632 /* (P16) Gamma corrected output of green pixel at 32(8)code in 10(8)bit linear input 0x1F Current        */
#define HI_161_GGMA3         0x1633 /* (P16) Gamma corrected output of green pixel at 64(16)code in 10(8)bit linear input 0x30 Current       */
#define HI_161_GGMA4         0x1634 /* (P16) Gamma corrected output of green pixel at 128(32)code in 10(8)bit linear input 0x49 Current      */
#define HI_161_GGMA5         0x1635 /* (P16) Gamma corrected output of green pixel at 192(48)code in 10(8)bit t linear input 0x5D Current    */
#define HI_161_GGMA6         0x1636 /* (P16) Gamma corrected output of green pixel at 256(64)code in 10(8)bit linear input 0x6F Current      */
#define HI_161_GGMA7         0x1637 /* (P16) Gamma corrected output of green pixel at 320(80)code in 10(8)bit linear input 0x7F Current      */
#define HI_161_GGMA8         0x1638 /* (P16) Gamma corrected output of green pixel at 384(96)code in 10(8)bit linear input 0x8E Current      */
#define HI_161_GGMA9         0x1639 /* (P16) Gamma corrected output of green pixel at 448(112)code in 10(8)bit linear input 0x9B Current     */
#define HI_161_GGMA10        0x163A /* (P16) Gamma corrected output of green pixel at 512(128)code in 10(8)bit linear input 0xA8 Current     */
#define HI_161_GGMA11        0x163B /* (P16) Gamma corrected output of green pixel at 576(144)code in 10(8)bit linear input 0xB5 Current     */
#define HI_161_GGMA12        0x163C /* (P16) Gamma corrected output of green pixel at 640(160)code in 10(8)bit linear input 0xC1 Current     */
#define HI_161_GGMA13        0x163D /* (P16) Gamma corrected output of green pixel at 704(176)code in 10(8)bit linear input 0xCC Current     */
#define HI_161_GGMA14        0x163E /* (P16) Gamma corrected output of green pixel at 768(192)code in 10(8)bit linear input 0xD7 Current     */
#define HI_161_GGMA15        0x163F /* (P16) Gamma corrected output of green pixel at 832(208)code in 10(8)bit linear input 0xE2 Current     */
#define HI_161_GGMA16        0x1640 /* (P16) Gamma corrected output of green pixel at 896(224)code in 10(8)bit linear input 0xEC Current     */
#define HI_161_GGMA17        0x1641 /* (P16) Gamma corrected output of green pixel at 960(240)code in 10(8)bit linear input 0xF6 Current     */
#define HI_161_GGMA18        0x1642 /* (P16) Gamma corrected output of green pixel at 1023(255)code in 10(8)bit linear input 0xFF Current    */
#define HI_161_RGMA0         0x1650 /* (P16) Gamma corrected output of red pixel at 0(0)code in 10(8)bit linear input 0x00 Current           */
#define HI_161_RGMA1         0x1651 /* (P16) Gamma corrected output of red pixel at 16(4)code in 10(8)bit linear input 0x15 Current          */
#define HI_161_RGMA2         0x1652 /* (P16) Gamma corrected output of red pixel at 32(8)code in 10(8)bit linear input 0x1F Current          */
#define HI_161_RGMA3         0x1653 /* (P16) Gamma corrected output of red pixel at 64(16)code in 10(8)bit linear input 0x30 Current         */
#define HI_161_RGMA4         0x1654 /* (P16) Gamma corrected output of red pixel at 128(32)code in 10(8)bit linear input 0x49 Current        */
#define HI_161_RGMA5         0x1655 /* (P16) Gamma corrected output of red pixel at 192(48)code in 10(8)bit t linear input 0x5D Current      */
#define HI_161_RGMA6         0x1656 /* (P16) Gamma corrected output of red pixel at 256(64)code in 10(8)bit linear input 0x6F Current        */
#define HI_161_RGMA7         0x1657 /* (P16) Gamma corrected output of red pixel at 320(80)code in 10(8)bit linear input 0x7F Current        */
#define HI_161_RGMA8         0x1658 /* (P16) Gamma corrected output of red pixel at 384(96)code in 10(8)bit linear input 0x8E Current        */
#define HI_161_RGMA9         0x1659 /* (P16) Gamma corrected output of red pixel at 448(112)code in 10(8)bit linear input 0x9B Current       */
#define HI_161_RGMA10        0x165A /* (P16) Gamma corrected output of red pixel at 512(128)code in 10(8)bit linear input 0xA8 Current       */
#define HI_161_RGMA11        0x165B /* (P16) Gamma corrected output of red pixel at 576(144)code in 10(8)bit linear input 0xB5 Current       */
#define HI_161_RGMA12        0x165C /* (P16) Gamma corrected output of red pixel at 640(160)code in 10(8)bit linear input 0xC1 Current       */
#define HI_161_RGMA13        0x165D /* (P16) Gamma corrected output of red pixel at 704(176)code in 10(8)bit linear input 0xCC Current       */
#define HI_161_RGMA14        0x165E /* (P16) Gamma corrected output of red pixel at 768(192)code in 10(8)bit linear input 0xD7 Current       */
#define HI_161_RGMA15        0x165F /* (P16) Gamma corrected output of red pixel at 832(208)code in 10(8)bit linear input 0xE2 Current       */
#define HI_161_RGMA16        0x1660 /* (P16) Gamma corrected output of red pixel at 896(224)code in 10(8)bit linear input 0xEC Current       */
#define HI_161_RGMA17        0x1661 /* (P16) Gamma corrected output of red pixel at 960(240)code in 10(8)bit linear input 0xF6 Current       */
#define HI_161_RGMA18        0x1662 /* (P16) Gamma corrected output of red pixel at 1023(255)code in 10(8)bit linear input 0xFF Current      */
#define HI_161_BGMA0         0x1670 /* (P16) Gamma corrected output of blue pixel at 0(0)code in 10(8)bit linear input 0x00 Current          */
#define HI_161_BGMA1         0x1671 /* (P16) Gamma corrected output of blue pixel at 16(4)code in 10(8)bit linear input 0x15 Current         */
#define HI_161_BGMA2         0x1672 /* (P16) Gamma corrected output of blue pixel at 32(8)code in 10(8)bit linear input 0x1F Current         */
#define HI_161_BGMA3         0x1673 /* (P16) Gamma corrected output of blue pixel at 64(16)code in 10(8)bit linear input 0x30 Current        */
#define HI_161_BGMA4         0x1674 /* (P16) Gamma corrected output of blue pixel at 128(32)code in 10(8)bit linear input 0x49 Current       */
#define HI_161_BGMA5         0x1675 /* (P16) Gamma corrected output of blue pixel at 192(48)code in 10(8)bit t linear input 0x5D Current     */
#define HI_161_BGMA6         0x1676 /* (P16) Gamma corrected output of blue pixel at 256(64)code in 10(8)bit linear input 0x6F Current       */
#define HI_161_BGMA7         0x1677 /* (P16) Gamma corrected output of blue pixel at 320(80)code in 10(8)bit linear input 0x7F Current       */
#define HI_161_BGMA8         0x1678 /* (P16) Gamma corrected output of blue pixel at 384(96)code in 10(8)bit linear input 0x8E Current       */
#define HI_161_BGMA9         0x1679 /* (P16) Gamma corrected output of blue pixel at 448(112)code in 10(8)bit linear input 0x9B Current      */
#define HI_161_BGMA10        0x167A /* (P16) Gamma corrected output of blue pixel at 512(128)code in 10(8)bit linear input 0xA8 Current      */
#define HI_161_BGMA11        0x167B /* (P16) Gamma corrected output of blue pixel at 576(144)code in 10(8)bit linear input 0xB5 Current      */
#define HI_161_BGMA12        0x167C /* (P16) Gamma corrected output of blue pixel at 640(160)code in 10(8)bit linear input 0xC1 Current      */
#define HI_161_BGMA13        0x167D /* (P16) Gamma corrected output of blue pixel at 704(176)code in 10(8)bit linear input 0xCC Current      */
#define HI_161_BGMA14        0x167E /* (P16) Gamma corrected output of blue pixel at 768(192)code in 10(8)bit linear input 0xD7 Current      */
#define HI_161_BGMA15        0x167F /* (P16) Gamma corrected output of blue pixel at 832(208)code in 10(8)bit linear input 0xE2 Current      */
#define HI_161_BGMA16        0x1680 /* (P16) Gamma corrected output of blue pixel at 896(224)code in 10(8)bit linear input 0xEC Current      */
#define HI_161_BGMA17        0x1681 /* (P16) Gamma corrected output of blue pixel at 960(240)code in 10(8)bit linear input 0xF6 Current      */
#define HI_161_BGMA18        0x1682 /* (P16) Gamma corrected output of blue pixel at 1023(255)code in 10(8)bit linear input 0xFF Current     */
/* Auto Flicker Cancellation (Page Mode = 17) */
#define HI_161_FLKMODE 0x17C0 /* (P17) Number of skipped frames in AFC operation 0x00 Current                                   */
#define HI_161_FLK200  0x17C4 /* (P17) Number of horizontal line for 1/200sec 0x3C Current                                      */
#define HI_161_FLK240  0x17C5 /* (P17) Number of horizontal line for 1/240sec 0x32 Current                                      */
#define HI_161_FLKTH1  0x17C6 /* (P17) Low threshold to detect flicker noise 0x02 Current                                       */
#define HI_161_FLKTH2  0x17C7 /* (P17) High threshold to detect flicker noise 0x20 Current                                      */
/* Image Scaling (Page Mode = 18) */
#define HI_161_ZOOMCTL1  0x1810 /* (P18) Control image scaling 0x00 Next                                                        */
#define HI_161_ZOOMCTL2  0x1811 /* (P18) Control image scaling 0x00 Next                                                        */
#define HI_161_ZOUTWIDH  0x1820 /* (P18) High byte of image width for image scaling 0x05 Next                                   */
#define HI_161_ZOUTWIDL  0x1821 /* (P18) Low byte of image width for image scaling 0x00 Next                                    */
#define HI_161_ZOUTHGTH  0x1822 /* (P18) High byte of image height for image scaling 0x04 Next                                  */
#define HI_161_ZOUTHGTL  0x1823 /* (P18) Low byte of image height for image scaling 0x00 Next                                   */
#define HI_161_ZWINSTXH  0x1824 /* (P18) High byte of start x position for windowing after image scaling 0x00 Next              */
#define HI_161_ZWINSTXL  0x1825 /* (P18) Low byte of start x position for windowing after image scaling 0x00 Next               */
#define HI_161_ZWINSTYH  0x1826 /* (P18) High byte of start y position for windowing after image scaling 0x00 Next              */
#define HI_161_ZWINSTYL  0x1827 /* (P18) Low byte of start y position for windowing after image scaling 0x00 Next               */
#define HI_161_ZWINENXH  0x1828 /* (P18) High byte of end x position for windowing after image scaling 0x05 Next                */
#define HI_161_ZWINENXL  0x1829 /* (P18) Low byte of end x position for windowing after image scaling 0x00 Next                 */
#define HI_161_ZWINENYH  0x182A /* (P18) High byte of end y position for windowing after image scaling 0x04 Next                */
#define HI_161_ZWINENYL  0x182B /* (P18) Low byte of end y position for windowing after image scaling 0x00 Next                 */
#define HI_161_ZVERSTEPH 0x182C /* (P18) High byte of vertical scaling step 0x10 Next                                           */
#define HI_161_ZVERSTEPL 0x182D /* (P18) Low byte of vertical scaling step 0x00 Next                                            */
#define HI_161_ZHORSTEPH 0x182E /* (P18) High byte of horizontal scaling step 0x10 Next                                         */
#define HI_161_ZHORSTEPL 0x182F /* (P18) Low byte of horizontal scaling step 0x00 Next                                          */
#define HI_161_ZFIFODLY  0x1830 /* (P18) Delay depth for image scaling 0x55 Next                                                */
/* Auto Exposure (Page Mode = 20) */
#define HI_161_AECTL1     0x2010 /* (P20) Control automatic exposure 0x0C Next                                                  */
#define HI_161_AECTL2     0x2011 /* (P20) Control automatic exposure 0x04 Next                                                  */
#define HI_161_AEFRAMECTL 0x2020 /* (P20) Frame control for automatic exposure 0x01 Current                                     */
#define HI_161_AEFINECTL1 0x2028 /* (P20) Speed option for automatic exposure 0x03 Current                                      */
#define HI_161_AEFINECTL2 0x2029 /* (P20) Speed option for automatic exposure 0xA5 Current                                      */
#define HI_161_AEFINECTL3 0x202A /* (P20) Speed option for automatic exposure 0xF0 Current                                      */
#define HI_161_AEFINECTL4 0x202B /* (P20) Speed option for automatic exposure 0x35 Current                                      */
#define HI_161_AEWGT      0x2060 /* (P20) Weight coefficient of each AE region 0x95 Current                                     */
#define HI_161_AECENHST   0x2068 /* (P20) Horizontal start address for AE center area. 0x28 Current                             */
#define HI_161_AECENHEN   0x2069 /* (P20) Horizontal stop address for AE center area. 0x78 Current                              */
#define HI_161_AECENVST   0x206A /* (P20) Vertical start address for AE center area. 0x50 Current                               */
#define HI_161_AECENVED   0x206B /* (P20) Vertical stop address for AE center area. 0xA0 Current                                */
#define HI_161_YLVL       0x2070 /* (P20) Luminance level to converge in AE 0x50 Current                                        */
#define HI_161_YTH1       0x2078 /* (P20) Threshold1 of hysteresis in AE 0x45 Current                                           */
#define HI_161_YTH2       0x2079 /* (P20) Threshold2 of hysteresis in AE 0x28 Current                                           */
#define HI_161_EXPINTH    0x2080 /* (P20) High byte of internal exposure time RO Current                                        */
#define HI_161_EXPINTM    0x2081 /* (P20) Middle byte of internal exposure time RO Current                                      */
#define HI_161_EXPINTL    0x2082 /* (P20) Low byte of internal exposure time RO Current                                         */
#define HI_161_EXPTIMEH   0x2083 /* (P20) High byte of manual exposure time 0x00 Next                                           */
#define HI_161_EXPTIMEM   0x2084 /* (P20) Middle byte of manual exposure time 0xC3 Next                                         */
#define HI_161_EXPTIMEL   0x2085 /* (P20) Low byte of manual exposure time 0x50 Next                                            */
#define HI_161_EXPMINH    0x2086 /* (P20) High byte of minimum exposure time 0x01 Current                                       */
#define HI_161_EXPMINL    0x2087 /* (P20) Low byte of minimum exposure time 0xF4 Current                                        */
#define HI_161_EXPMAXH    0x2088 /* (P20) High byte of maximum exposure time 0x02 Current                                       */
#define HI_161_EXPMAXM    0x2089 /* (P20) Middle byte of maximum exposure time 0x49 Current                                     */
#define HI_161_EXPMAXL    0x208A /* (P20) Low byte of maximum exposure time 0xF0 Current                                        */
#define HI_161_EXP100H    0x208B /* (P20) High byte of exposure time for 1/100sec 0x3A Next                                     */
#define HI_161_EXP100L    0x208C /* (P20) Low byte of exposure time for 1/100sec 0x98 Next                                      */
#define HI_161_EXP120H    0x208D /* (P20) High byte of exposure time for 1/120sec 0x30 Next                                     */
#define HI_161_EXP120L    0x208E /* (P20) Low byte of exposure time for 1/120sec 0xD4 Next                                      */
#define HI_161_EXPDPCH    0x208F /* (P20) High byte of exposure time for dead pixel concealment 0x01 Current                    */
#define HI_161_EXPDPCL    0x2090 /* (P20) Low byte of exposure time for dead pixel concealment 0x86 Current                     */
#define HI_161_EXPFIXH    0x2091 /* (P20) High byte of exposure time for fixed frame rate 0x02 Current                          */
#define HI_161_EXPFIXM    0x2092 /* (P20) Middle byte of exposure time for fixed frame rate 0xF9 Current                        */
#define HI_161_EXPFIXL    0x2093 /* (P20) Low byte of exposure time for fixed frame rate 0xB8 Current                           */
#define HI_161_EXPOUT1    0x2098 /* (P20) Threshold of exposure time to define bright condition. 0x9A Current                   */
#define HI_161_EXPOUT2    0x2099 /* (P20) Threshold of exposure time to define very bright condition. 0x45 Current              */
#define HI_161_EXPLMTH    0x209C /* (P20) High byte of exposure time for low limit. 0x01 Current                                */
#define HI_161_EXPLMTL    0x209D /* (P20) Low byte of exposure time for low limit. 0xF4 Current                                 */
#define HI_161_EXPUNITH   0x209E /* (P20) High byte of unit step of EXPLMT 0x00 Current                                         */
#define HI_161_EXPUNITL   0x209F /* (P20) Low byte of unit step of EXPLMT 0xC8 Current                                          */
#define HI_161_AG         0x20B0 /* (P20) Automatic gain (AG) 0x10 Next                                                         */
#define HI_161_AGMIN      0x20B1 /* (P20) Minimum AG in AE 0x10 Current                                                         */
#define HI_161_AGMAX      0x20B2 /* (P20) Maximum AG in AE 0x80 Current                                                         */
#define HI_161_AGLVL      0x20B3 /* (P20) Target level of AG in AE 0x10 Current                                                 */
#define HI_161_AGTH1      0x20B4 /* (P20) Minimum threshold of Band AG in AE 0x10 Current                                       */
#define HI_161_AGTH2      0x20B5 /* (P20) Maximum threshold of Band AG in AE 0x30 Current                                       */
#define HI_161_AGBTH1     0x20B6 /* (P20) Threshold1 of Adaptive AG in AE. 0x20 Current                                         */
#define HI_161_AGBTH2     0x20B7 /* (P20) Threshold2 of Adaptive AG in AE. 0x1B Current                                         */
#define HI_161_AGBTH3     0x20B8 /* (P20) Threshold3 of Adaptive AG in AE. 0x18 Current                                         */
#define HI_161_AGBTH4     0x20B9 /* (P20) Threshold4 of Adaptive AG in AE. 0x16 Current                                         */
#define HI_161_AGBTH5     0x20BA /* (P20) Threshold5 of Adaptive AG in AE. 0x15 Current                                         */
#define HI_161_AGBTH6     0x20BB /* (P20) Threshold6 of Adaptive AG in AE. 0x14 Current                                         */
#define HI_161_AGBTH7     0x20BC /* (P20) Threshold7 of Adaptive AG in AE. 0x13 Current                                         */
#define HI_161_AGBTH8     0x20BD /* (P20) Threshold8 of Adaptive AG in AE. 0x12 Current                                         */
#define HI_161_AGBTH9     0x20BE /* (P20) Threshold9 of Adaptive AG in AE. 0x12 Current                                         */
#define HI_161_AGBTH10    0x20BF /* (P20) Threshold10 of Adaptive AG in AE. 0x12 Current                                        */
#define HI_161_AGBTH11    0x20C0 /* (P20) Threshold11 of Adaptive AG in AE. 0x12 Current                                        */
#define HI_161_AGBTH12    0x20C1 /* (P20) Threshold12 of Adaptive AG in AE. 0x12 Current                                        */
#define HI_161_AGLVLL     0x20C2 /* (P20) AGLVLL is target of AG to converge on abrupt transition.(Dark -> Indoor) 0x10 Current */
#define HI_161_AGSKY      0x20C7 /* (P20) Threshold of AG in very bright condition 0x14 Current                                 */
#define HI_161_DGMAX      0x20C8 /* (P20) Maximum Digital gain in AE. 0xFF Current                                              */
#define HI_161_DGMIN      0x20C9 /* (P20) Minimum Digital gain in AE. 0x80 Current                                              */
#define HI_161_YAVG       0x20D3 /* (P20) Average of luminance. RO                                                              */
/* Auto White Balance (Page Mode = 20) */
#define HI_161_AWBCTL1     0x2210 /* (P22) Control automatic white balance 0x6B Next                                            */
#define HI_161_AWBCTL2     0x2211 /* (P22) Control automatic white balance 0x28 Next                                            */
#define HI_161_AWBMAP      0x2219 /* (P22) Control automatic white balance 0x00 Next                                            */
#define HI_161_AWBCTL3     0x2225 /* (P22) Control automatic white balance 0x3C Next                                            */
#define HI_161_AWBCTL4     0x222E /* (P22) Control automatic white balance 0x7A Next                                            */
#define HI_161_AWBCTL5     0x222F /* (P22) Control automatic white balance 0x08 Next                                            */
#define HI_161_ULVL        0x2230 /* (P22) U chrominance level to converge in AWB 0x80 Current                                  */
#define HI_161_VLVL        0x2231 /* (P22) V chrominance level to converge in AWB 0x80 Current                                  */
#define HI_161_WHTSLOPE1   0x2234 /* (P22) White region 2 domain slope 0x18 Current                                             */
#define HI_161_WHTSLOPE2   0x2235 /* (P22) White region 4 domain slope 0x0B Current                                             */
#define HI_161_CDIFFBND1   0x2236 /* (P22) Cdiff band of 2 domain slope of white region 0x2A Current                            */
#define HI_161_CDIFFBND2   0x2237 /* (P22) Cdiff band of 4 domain slope of white region 0x3A Current                            */
#define HI_161_UVTH1       0x2238 /* (P22) U/V chrominance threshold1 in AWB 0x13 Current                                       */
#define HI_161_UVTH2       0x2239 /* (P22) U/V chrominance threshold2 in AWB 0x66 Current                                       */
#define HI_161_YRANGE      0x2240 /* (P22) Valid luminance range to detect white pixel 0xF3 Current                             */
#define HI_161_CDIFF       0x2241 /* (P22) Threshold of chrominance difference to detect white pixel 0x35 Current               */
#define HI_161_CSUM2       0x2242 /* (P22) Threshold of chrominance summation to detect white pixel 0x34 Current                */
#define HI_161_YRANGETOT   0x2243 /* (P22) Valid luminance range to detect white pixel in transiert condition 0xF3 Current                           */
#define HI_161_CDIFFTOT    0x2244 /* (P22) Threshold of chrominance difference to detect white pixel in transiert condtion 0x58 Current              */
#define HI_161_CSUM2TOT    0x2245 /* (P22) Threshold of chrominance summation of 2 domain to detect white pixel in transiert condition. 0x44 Current */
#define HI_161_WHTPXLTH    0x2246 /* (P22) Threshold of number of white pixel for AWB 0x0A Current                                                   */
#define HI_161_YRANGEDAK   0x2247 /* (P22) Valid luminance range to detect white pixel in dark condition 0xF3 Current                                */
#define HI_161_CDIFFDAK    0x2248 /* (P22) Threshold of chrominance difference to detect white pixel in dark condtion 0x35 Current                   */
#define HI_161_CSUM2DAK    0x2249 /* (P22) Threshold of chrominance summation of 2 domain to detect white pixel in dark condition. 0x34 Current      */
#define HI_161_YRANGEOUT   0x224A /* (P22) Valid luminance range to detect white pixel in outdoor condition 0xF8 Current                             */
#define HI_161_RGAIN       0x2280 /* (P22) Red color gain 0x30 Next                                                             */
#define HI_161_GGAIN       0x2281 /* (P22) Green color gain 0x20 Next                                                           */
#define HI_161_BGAIN       0x2282 /* (P22) Blue color gain 0x38 Next                                                            */
#define HI_161_RMAX        0x2283 /* (P22) Maximum RGAIN in AWB 0x50 Current                                                    */
#define HI_161_RMIN        0x2284 /* (P22) Minimum RGAIN in AWB 0x20 Current                                                    */
#define HI_161_BMAX        0x2285 /* (P22) Maximum BGAIN in AWB 0x50 Current                                                    */
#define HI_161_BMIN        0x2286 /* (P22) Minimum BGAIN in AWB 0x20 Current                                                    */
#define HI_161_RMAXM       0x2287 /* (P22) Upper limit of RGAIN in bright condition 0x50 Current                                */
#define HI_161_RMINM       0x2288 /* (P22) Lower limit of RGAIN in bright condition 0x3E Current                                */
#define HI_161_BMAXM       0x2289 /* (P22) Upper limit of BGAIN in bright condition 0x2E Current                                */
#define HI_161_BMINM       0x228A /* (P22) Lower limit of BGAIN in bright condition 0x20 Current                                */
#define HI_161_RMAXB       0x228B /* (P22) Lower limit of BGAIN in bright condition 0x20 Current                                */
#define HI_161_RMINB       0x228C /* (P22) Lower limit of BGAIN in bright condition 0x20 Current                                */
#define HI_161_BMAXB       0x228D /* (P22) Lower limit of BGAIN in bright condition 0x20 Current                                */
#define HI_161_BMINB       0x228E /* (P22) Lower limit of BGAIN in bright condition 0x20 Current                                */
#define HI_161_BGAINPARA1  0x228F /* (P22) Parameter1 of BGAIN 0x50 Current                                                     */
#define HI_161_BGAINPARA2  0x2290 /* (P22) Parameter2 of BGAIN 0x4C Current                                                     */
#define HI_161_BGAINPARA3  0x2291 /* (P22) Parameter3 of BGAIN 0x48 Current                                                     */
#define HI_161_BGAINPARA4  0x2292 /* (P22) Parameter4 of BGAIN 0x46 Current                                                     */
#define HI_161_BGAINPARA5  0x2293 /* (P22) Parameter5 of BGAIN 0x44 Current                                                     */
#define HI_161_BGAINPARA6  0x2294 /* (P22) Parameter6 of BGAIN 0x42 Current                                                     */
#define HI_161_BGAINPARA7  0x2295 /* (P22) Parameter7 of BGAIN 0x40 Current                                                     */
#define HI_161_BGAINPARA8  0x2296 /* (P22) Parameter8 of BGAIN 0x3E Current                                                     */
#define HI_161_BGAINPARA9  0x2297 /* (P22) Parameter9 of BGAIN 0x3C Current                                                     */
#define HI_161_BGAINPARA10 0x2298 /* (P22) Parameter10 of BGAIN 0x28 Current                                                    */
#define HI_161_BGAINPARA11 0x2299 /* (P22) Parameter11 of BGAIN 0x26 Current                                                    */
#define HI_161_BGAINPARA12 0x229A /* (P22) Parameter12 of BGAIN 0x24 Current                                                    */
#define HI_161_BGAINBND1   0x229B /* (P22) Boundary of BGAIN 0x04 Current                                                       */
#define HI_161_BGAINBND2   0x229C /* (P22) Boundary of BGAIN 0x44 Current                                                       */
#define HI_161_RAINTH1     0x229D /* (P22) Threshold1 of RGAIN 0x40 Current                                                     */
#define HI_161_RAINTH2     0x229E /* (P22) Threshold2 of RGAIN 0x30 Current                                                     */
#define HI_161_RAINTH3     0x229F /* (P22) Threshold3 of RGAIN 0x20 Current                                                     */
#define HI_161_RDELTA1     0x22A0 /* (P22) The Increment delta value1 of red gain limit boundary 0x04 Current                   */
#define HI_161_BDELTA1     0x22A1 /* (P22) The Increment delta value1of blue gain limit boundary 0x60 Current                   */
#define HI_161_RDELTA2     0x22A2 /* (P22) The Increment delta value2 of red gain limit boundary 0x22 Current                   */
#define HI_161_BDELTA2     0x22A3 /* (P22) The Increment delta value2 of blue gain limit boundary 0x42 Current                  */
#define HI_161_AWBEXPLMT1  0x22A4 /* (P22) Threshold1 of red and blue limit boundary automatically 0x10 Current                 */
#define HI_161_AWBEXPLMT2  0x22A5 /* (P22) Threshold2 of red and blue limit boundary automatically 0x40 Current                 */
#define HI_161_AWBEXPLMT3  0x22A6 /* (P22) Threshold3 of red and blue limit boundary automatically 0x80 Current                 */ 
#define HI_161_HTMPTHOFF   0x22AD /* (P22) Gain threshold to disnable High temp WB offset. 0x38 Current                         */
#define HI_161_HTMPTHON    0x22AE /* (P22) Gain threshold to enable High temp WB offset. 0x30 Current                           */
#define HI_161_MRGAIN      0x22B2 /* (P22) RGAIN for manual white balance 0x48 Current                                          */
#define HI_161_MBGAIN      0x22B3 /* (P22) BGAIN for manual white balance 0x40 Current                                          */
#define HI_161_HTMPOFS     0x22B6 /* (P22) Hight temp WB target offset 0x00 Current                                             */
#define HI_161_LTMPOFS     0x22B7 /* (P22) Low   temp WB target offset 0x00 Current                                             */
#define HI_161_LTMPTHMIN   0x22BC /* (P22) Gain threshold to complete calculation of Low temp WB offset. 0x30 Current           */
#define HI_161_LTMPTHMAX   0x22BD /* (P22) Gain threshold to apply calaulation of Low temp WB offset. 0x38 Current              */
#define HI_161_UAVG        0x22D2 /* (P22) Average of U chrominance of white pixels in frame RO                                 */
#define HI_161_VAVG        0x22D3 /* (P22) Average of V chrominance of white pixels in frame RO                                 */

#define reg_read(reg, val) hi161_read_reg(HI_161_##reg, val)
#define reg_write(reg, val) hi161_write_reg(HI_161_##reg, (val))
#define reg_set(reg, val) hi161_reg_set(HI_161_##reg, (val))
#define reg_clear(reg, val) hi161_reg_clear(HI_161_##reg, (val))

/* HI_161_PWRCTL bit definition */
#define HI_161_PWRCTL_SLEEP         (1 << 0)
#define HI_161_PWRCTL_SOFT_RESET    (1 << 1)  
#define HI_161_PWRCTL_OUT_DRIVE_0   (0 << 4)  
#define HI_161_PWRCTL_OUT_DRIVE_1   (1 << 4)
#define HI_161_PWRCTL_OUT_DRIVE_2   (2 << 4)
#define HI_161_PWRCTL_OUT_DRIVE_3   (3 << 4)
#define HI_161_PWRCTL_PCLK_DRIVE_0  (0 << 6)
#define HI_161_PWRCTL_PCLK_DRIVE_1  (1 << 6)
#define HI_161_PWRCTL_PCLK_DRIVE_2  (2 << 6)
#define HI_161_PWRCTL_PCLK_DRIVE_3  (3 << 6)
#define HI_161_PWRCTL_PCLK_DRIVE_MX (1 << 3)

/* HI_161_VDOCTL1 bit definition */
#define HI_161_VDOCTL1_VSYNC_TYPE1  (0 << 3)
#define HI_161_VDOCTL1_VSYNC_TYPE2  (1 << 3)
#define HI_161_VDOCTL1_SUB_VGA      (0 << 4)
#define HI_161_VDOCTL1_SUB_QVGA     (1 << 4)
#define HI_161_VDOCTL1_SUB_QQVGA    (2 << 4)

/* HI_161_VDOCTL2 bit definition */
#define HI_161_VDOCTL2_X_FLIP       (1 << 0)
#define HI_161_VDOCTL2_Y_FLIP       (1 << 1)
#define HI_161_VDOCTL2_FIXED_RATE   (1 << 2)
#define HI_161_VDOCTL2_BAD_FRM_SKIP_1 (1 << 4)
#define HI_161_VDOCTL2_BAD_FRM_SKIP_2 (2 << 4)
#define HI_161_VDOCTL2_BAD_FRM_SKIP_3 (3 << 4)
#define HI_161_VDOCTL2_WINDOWING    (1 << 7)

/* HI_161_SYNCCTL bit definition */
#define HI_161_SYNCCTL_VSYNC_POL    (1 << 5)
#define HI_161_SYNCCTL_HSYNC_POL    (1 << 4)
#define HI_161_SYNCCTL_HSYNC_CLIP   (1 << 3)
#define HI_161_SYNCCTL_HSYNC_CLK_POL (1 << 2)
#define HI_161_SYNCCTL_HSYNC_CLK_DIV_8 (3 << 0)
#define HI_161_SYNCCTL_HSYNC_CLK_DIV_4 (2 << 0)
#define HI_161_SYNCCTL_HSYNC_CLK_DIV_2 (1 << 0)
#define HI_161_SYNCCTL_HSYNC_CLK_DIV_1 (0 << 0)

/* HI_161_ISPCTL1 bit definition */
#define HI_161_ISPCTL1_UPHASE_UYVY  (0 << 0)
#define HI_161_ISPCTL1_UPHASE_VYUY  (1 << 0)
#define HI_161_ISPCTL1_YPHASE_UYVY  (0 << 1)
#define HI_161_ISPCTL1_YPHASE_YUYV  (1 << 1)
#define HI_161_ISPCTL1_ITU656       (1 << 2)
#define HI_161_ISPCTL1_YUV422       (0 << 4)
#define HI_161_ISPCTL1_RGB565       (4 << 4)
#define HI_161_ISPCTL1_RGB444       (7 << 4)

/* HI_161_ISPCTL2 bit definition */
#define HI_161_ISPCTL2_CLR_INTERPOL (0 << 0)
#define HI_161_ISPCTL2_CSPCE_CONV   (1 << 1)
#define HI_161_ISPCTL2_SKETCH       (1 << 4)
#define HI_161_ISPCTL2_EMBOSSING    (1 << 5)
#define HI_161_ISPCTL2_BYTE_SWAP    (1 << 7)

/* HI_161_ISPCTL3 bit definition */
#define HI_161_ISPCTL3_U_CONST      (0 << 0)
#define HI_161_ISPCTL3_V_CONST      (1 << 1)
#define HI_161_ISPCTL3_CLR_INV      (1 << 2)
#define HI_161_ISPCTL3_NEG          (1 << 3)
#define HI_161_ISPCTL3_Y_OFFSET     (1 << 4)
#define HI_161_ISPCTL3_AUTOBRIGHT   (1 << 5)
#define HI_161_ISPCTL3_U_OFFSET     (1 << 6)
#define HI_161_ISPCTL3_V_OFFSET     (1 << 7)

/* HI_161_SATCTL bit definition */
#define HI_161_SATCTL_MANCOLORSAT   (1 << 0)
#define HI_161_SATCTL_AUTOCOLORSAT  (1 << 1)
#define HI_161_SATCTL_SUPPRESSION_0 (0 << 2)
#define HI_161_SATCTL_SUPPRESSION_1 (1 << 2)
#define HI_161_SATCTL_SUPPRESSION_2 (2 << 2)
#define HI_161_SATCTL_SUPPRESSION_3 (3 << 2)

/* HI_161_AECTL1 bit definition */
#define HI_161_AECTL1_AE_ENABLE     (1 << 7)
#define HI_161_AECTL1_ANTIFLKR_ENABLE  (1 << 6)
#define HI_161_AECTL1_100HZ         (1 << 4)
#define HI_161_AECTL1_120HZ         (0 << 4)

/* HI_161_AWBCTL1 bit definition */
#define HI_161_AWBCTL1_AWB_ENABLE    (1 << 7)
#define HI_161_AWBCTL1_ABW_SPD_7     (7 << 4)
#define HI_161_AWBCTL1_ABW_SPD_6     (6 << 4)
#define HI_161_AWBCTL1_ABW_SPD_5     (5 << 4)
#define HI_161_AWBCTL1_ABW_SPD_4     (4 << 4)
#define HI_161_AWBCTL1_ABW_SPD_3     (3 << 4)
#define HI_161_AWBCTL1_ABW_SPD_2     (2 << 4)
#define HI_161_AWBCTL1_ABW_SPD_1     (1 << 4)
#define HI_161_AWBCTL1_ABW_SPD_0     (0 << 4)
#define HI_161_AWBCTL1_ABW_SPD_OPT   (0x0B << 0)           

enum {
	HI_161_FLICKER_DISABLE = 0,
	HI_161_FLICKER_MANUAL_50,
	HI_161_FLICKER_MANUAL_60,
	HI_161_FLICKER_AUTO_DETECTION
};

#define HI_161_I2C_ADDRESS  0x20
#endif
