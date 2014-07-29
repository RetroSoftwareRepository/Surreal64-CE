/******************************************************************************\
* Authors:  Iconoclast                                                         *
* Release:  2013.11.26                                                         *
* License:  CC0 Public Domain Dedication                                       *
*                                                                              *
* To the extent possible under law, the author(s) have dedicated all copyright *
* and related and neighboring rights to this software to the public domain     *
* worldwide. This software is distributed without any warranty.                *
*                                                                              *
* You should have received a copy of the CC0 Public Domain Dedication along    *
* with this software.                                                          *
* If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.             *
\******************************************************************************/
#ifndef _DIVROM_H
#define _DIVROM_H

static int DivIn = 0; /* buffered numerator of division read from vector file */
static int DivOut = 0; /* global division result set by VRCP/VRCPL/VRSQ/VRSQH */
#if (0)
static int MovIn; /* We do not emulate this register (obsolete, for VMOV). */
#endif

static int DPH;
/*
 * Boolean flag:  Double-precision high was the last vector divide op?
 *
 * if (lastDivideOp == VRCP, VRCPL, VRSQ, VRSQL)
 *     DPH = false; // single-precision or double-precision low, not high
 * else if (lastDivideOp == VRCPH, VRSQH)
 *     DPH = true; // double-precision high
 * else if (lastDivideOp == VMOV, VNOP)
 *     DPH = DPH; // no change, divide-group ops but not real divides
 */

/*
 * 11-bit vector divide result look-up table
 * Thanks to MooglyGuy @ MESS for organizing.
 */
static const unsigned short div_ROM[1024] = {
    0xFFFF,
    0xFF00,
    0xFE01,
    0xFD04,
    0xFC07,
    0xFB0C,
    0xFA11,
    0xF918,
    0xF81F,
    0xF727,
    0xF631,
    0xF53B,
    0xF446,
    0xF352,
    0xF25F,
    0xF16D,
    0xF07C,
    0xEF8B,
    0xEE9C,
    0xEDAE,
    0xECC0,
    0xEBD3,
    0xEAE8,
    0xE9FD,
    0xE913,
    0xE829,
    0xE741,
    0xE65A,
    0xE573,
    0xE48D,
    0xE3A9,
    0xE2C5,
    0xE1E1,
    0xE0FF,
    0xE01E,
    0xDF3D,
    0xDE5D,
    0xDD7E,
    0xDCA0,
    0xDBC2,
    0xDAE6,
    0xDA0A,
    0xD92F,
    0xD854,
    0xD77B,
    0xD6A2,
    0xD5CA,
    0xD4F3,
    0xD41D,
    0xD347,
    0xD272,
    0xD19E,
    0xD0CB,
    0xCFF8,
    0xCF26,
    0xCE55,
    0xCD85,
    0xCCB5,
    0xCBE6,
    0xCB18,
    0xCA4B,
    0xC97E,
    0xC8B2,
    0xC7E7,
    0xC71C,
    0xC652,
    0xC589,
    0xC4C0,
    0xC3F8,
    0xC331,
    0xC26B,
    0xC1A5,
    0xC0E0,
    0xC01C,
    0xBF58,
    0xBE95,
    0xBDD2,
    0xBD10,
    0xBC4F,
    0xBB8F,
    0xBACF,
    0xBA10,
    0xB951,
    0xB894,
    0xB7D6,
    0xB71A,
    0xB65E,
    0xB5A2,
    0xB4E8,
    0xB42E,
    0xB374,
    0xB2BB,
    0xB203,
    0xB14B,
    0xB094,
    0xAFDE,
    0xAF28,
    0xAE73,
    0xADBE,
    0xAD0A,
    0xAC57,
    0xABA4,
    0xAAF1,
    0xAA40,
    0xA98E,
    0xA8DE,
    0xA82E,
    0xA77E,
    0xA6D0,
    0xA621,
    0xA574,
    0xA4C6,
    0xA41A,
    0xA36E,
    0xA2C2,
    0xA217,
    0xA16D,
    0xA0C3,
    0xA01A,
    0x9F71,
    0x9EC8,
    0x9E21,
    0x9D79,
    0x9CD3,
    0x9C2D,
    0x9B87,
    0x9AE2,
    0x9A3D,
    0x9999,
    0x98F6,
    0x9852,
    0x97B0,
    0x970E,
    0x966C,
    0x95CB,
    0x952B,
    0x948B,
    0x93EB,
    0x934C,
    0x92AD,
    0x920F,
    0x9172,
    0x90D4,
    0x9038,
    0x8F9C,
    0x8F00,
    0x8E65,
    0x8DCA,
    0x8D30,
    0x8C96,
    0x8BFC,
    0x8B64,
    0x8ACB,
    0x8A33,
    0x899C,
    0x8904,
    0x886E,
    0x87D8,
    0x8742,
    0x86AD,
    0x8618,
    0x8583,
    0x84F0,
    0x845C,
    0x83C9,
    0x8336,
    0x82A4,
    0x8212,
    0x8181,
    0x80F0,
    0x8060,
    0x7FD0,
    0x7F40,
    0x7EB1,
    0x7E22,
    0x7D93,
    0x7D05,
    0x7C78,
    0x7BEB,
    0x7B5E,
    0x7AD2,
    0x7A46,
    0x79BA,
    0x792F,
    0x78A4,
    0x781A,
    0x7790,
    0x7706,
    0x767D,
    0x75F5,
    0x756C,
    0x74E4,
    0x745D,
    0x73D5,
    0x734F,
    0x72C8,
    0x7242,
    0x71BC,
    0x7137,
    0x70B2,
    0x702E,
    0x6FA9,
    0x6F26,
    0x6EA2,
    0x6E1F,
    0x6D9C,
    0x6D1A,
    0x6C98,
    0x6C16,
    0x6B95,
    0x6B14,
    0x6A94,
    0x6A13,
    0x6993,
    0x6914,
    0x6895,
    0x6816,
    0x6798,
    0x6719,
    0x669C,
    0x661E,
    0x65A1,
    0x6524,
    0x64A8,
    0x642C,
    0x63B0,
    0x6335,
    0x62BA,
    0x623F,
    0x61C5,
    0x614B,
    0x60D1,
    0x6058,
    0x5FDF,
    0x5F66,
    0x5EED,
    0x5E75,
    0x5DFD,
    0x5D86,
    0x5D0F,
    0x5C98,
    0x5C22,
    0x5BAB,
    0x5B35,
    0x5AC0,
    0x5A4B,
    0x59D6,
    0x5961,
    0x58ED,
    0x5879,
    0x5805,
    0x5791,
    0x571E,
    0x56AC,
    0x5639,
    0x55C7,
    0x5555,
    0x54E3,
    0x5472,
    0x5401,
    0x5390,
    0x5320,
    0x52AF,
    0x5240,
    0x51D0,
    0x5161,
    0x50F2,
    0x5083,
    0x5015,
    0x4FA6,
    0x4F38,
    0x4ECB,
    0x4E5E,
    0x4DF1,
    0x4D84,
    0x4D17,
    0x4CAB,
    0x4C3F,
    0x4BD3,
    0x4B68,
    0x4AFD,
    0x4A92,
    0x4A27,
    0x49BD,
    0x4953,
    0x48E9,
    0x4880,
    0x4817,
    0x47AE,
    0x4745,
    0x46DC,
    0x4674,
    0x460C,
    0x45A5,
    0x453D,
    0x44D6,
    0x446F,
    0x4408,
    0x43A2,
    0x433C,
    0x42D6,
    0x4270,
    0x420B,
    0x41A6,
    0x4141,
    0x40DC,
    0x4078,
    0x4014,
    0x3FB0,
    0x3F4C,
    0x3EE8,
    0x3E85,
    0x3E22,
    0x3DC0,
    0x3D5D,
    0x3CFB,
    0x3C99,
    0x3C37,
    0x3BD6,
    0x3B74,
    0x3B13,
    0x3AB2,
    0x3A52,
    0x39F1,
    0x3991,
    0x3931,
    0x38D2,
    0x3872,
    0x3813,
    0x37B4,
    0x3755,
    0x36F7,
    0x3698,
    0x363A,
    0x35DC,
    0x357F,
    0x3521,
    0x34C4,
    0x3467,
    0x340A,
    0x33AE,
    0x3351,
    0x32F5,
    0x3299,
    0x323E,
    0x31E2,
    0x3187,
    0x312C,
    0x30D1,
    0x3076,
    0x301C,
    0x2FC2,
    0x2F68,
    0x2F0E,
    0x2EB4,
    0x2E5B,
    0x2E02,
    0x2DA9,
    0x2D50,
    0x2CF8,
    0x2C9F,
    0x2C47,
    0x2BEF,
    0x2B97,
    0x2B40,
    0x2AE8,
    0x2A91,
    0x2A3A,
    0x29E4,
    0x298D,
    0x2937,
    0x28E0,
    0x288B,
    0x2835,
    0x27DF,
    0x278A,
    0x2735,
    0x26E0,
    0x268B,
    0x2636,
    0x25E2,
    0x258D,
    0x2539,
    0x24E5,
    0x2492,
    0x243E,
    0x23EB,
    0x2398,
    0x2345,
    0x22F2,
    0x22A0,
    0x224D,
    0x21FB,
    0x21A9,
    0x2157,
    0x2105,
    0x20B4,
    0x2063,
    0x2012,
    0x1FC1,
    0x1F70,
    0x1F1F,
    0x1ECF,
    0x1E7F,
    0x1E2E,
    0x1DDF,
    0x1D8F,
    0x1D3F,
    0x1CF0,
    0x1CA1,
    0x1C52,
    0x1C03,
    0x1BB4,
    0x1B66,
    0x1B17,
    0x1AC9,
    0x1A7B,
    0x1A2D,
    0x19E0,
    0x1992,
    0x1945,
    0x18F8,
    0x18AB,
    0x185E,
    0x1811,
    0x17C4,
    0x1778,
    0x172C,
    0x16E0,
    0x1694,
    0x1648,
    0x15FD,
    0x15B1,
    0x1566,
    0x151B,
    0x14D0,
    0x1485,
    0x143B,
    0x13F0,
    0x13A6,
    0x135C,
    0x1312,
    0x12C8,
    0x127F,
    0x1235,
    0x11EC,
    0x11A3,
    0x1159,
    0x1111,
    0x10C8,
    0x107F,
    0x1037,
    0x0FEF,
    0x0FA6,
    0x0F5E,
    0x0F17,
    0x0ECF,
    0x0E87,
    0x0E40,
    0x0DF9,
    0x0DB2,
    0x0D6B,
    0x0D24,
    0x0CDD,
    0x0C97,
    0x0C50,
    0x0C0A,
    0x0BC4,
    0x0B7E,
    0x0B38,
    0x0AF2,
    0x0AAD,
    0x0A68,
    0x0A22,
    0x09DD,
    0x0998,
    0x0953,
    0x090F,
    0x08CA,
    0x0886,
    0x0842,
    0x07FD,
    0x07B9,
    0x0776,
    0x0732,
    0x06EE,
    0x06AB,
    0x0668,
    0x0624,
    0x05E1,
    0x059E,
    0x055C,
    0x0519,
    0x04D6,
    0x0494,
    0x0452,
    0x0410,
    0x03CE,
    0x038C,
    0x034A,
    0x0309,
    0x02C7,
    0x0286,
    0x0245,
    0x0204,
    0x01C3,
    0x0182,
    0x0141,
    0x0101,
    0x00C0,
    0x0080,
    0x0040,
    0x6A09,
    0xFFFF,
    0x6955,
    0xFF00,
    0x68A1,
    0xFE02,
    0x67EF,
    0xFD06,
    0x673E,
    0xFC0B,
    0x668D,
    0xFB12,
    0x65DE,
    0xFA1A,
    0x6530,
    0xF923,
    0x6482,
    0xF82E,
    0x63D6,
    0xF73B,
    0x632B,
    0xF648,
    0x6280,
    0xF557,
    0x61D7,
    0xF467,
    0x612E,
    0xF379,
    0x6087,
    0xF28C,
    0x5FE0,
    0xF1A0,
    0x5F3A,
    0xF0B6,
    0x5E95,
    0xEFCD,
    0x5DF1,
    0xEEE5,
    0x5D4E,
    0xEDFF,
    0x5CAC,
    0xED19,
    0x5C0B,
    0xEC35,
    0x5B6B,
    0xEB52,
    0x5ACB,
    0xEA71,
    0x5A2C,
    0xE990,
    0x598F,
    0xE8B1,
    0x58F2,
    0xE7D3,
    0x5855,
    0xE6F6,
    0x57BA,
    0xE61B,
    0x5720,
    0xE540,
    0x5686,
    0xE467,
    0x55ED,
    0xE38E,
    0x5555,
    0xE2B7,
    0x54BE,
    0xE1E1,
    0x5427,
    0xE10D,
    0x5391,
    0xE039,
    0x52FC,
    0xDF66,
    0x5268,
    0xDE94,
    0x51D5,
    0xDDC4,
    0x5142,
    0xDCF4,
    0x50B0,
    0xDC26,
    0x501F,
    0xDB59,
    0x4F8E,
    0xDA8C,
    0x4EFE,
    0xD9C1,
    0x4E6F,
    0xD8F7,
    0x4DE1,
    0xD82D,
    0x4D53,
    0xD765,
    0x4CC6,
    0xD69E,
    0x4C3A,
    0xD5D7,
    0x4BAF,
    0xD512,
    0x4B24,
    0xD44E,
    0x4A9A,
    0xD38A,
    0x4A10,
    0xD2C8,
    0x4987,
    0xD206,
    0x48FF,
    0xD146,
    0x4878,
    0xD086,
    0x47F1,
    0xCFC7,
    0x476B,
    0xCF0A,
    0x46E5,
    0xCE4D,
    0x4660,
    0xCD91,
    0x45DC,
    0xCCD6,
    0x4558,
    0xCC1B,
    0x44D5,
    0xCB62,
    0x4453,
    0xCAA9,
    0x43D1,
    0xC9F2,
    0x434F,
    0xC93B,
    0x42CF,
    0xC885,
    0x424F,
    0xC7D0,
    0x41CF,
    0xC71C,
    0x4151,
    0xC669,
    0x40D2,
    0xC5B6,
    0x4055,
    0xC504,
    0x3FD8,
    0xC453,
    0x3F5B,
    0xC3A3,
    0x3EDF,
    0xC2F4,
    0x3E64,
    0xC245,
    0x3DE9,
    0xC198,
    0x3D6E,
    0xC0EB,
    0x3CF5,
    0xC03F,
    0x3C7C,
    0xBF93,
    0x3C03,
    0xBEE9,
    0x3B8B,
    0xBE3F,
    0x3B13,
    0xBD96,
    0x3A9C,
    0xBCED,
    0x3A26,
    0xBC46,
    0x39B0,
    0xBB9F,
    0x393A,
    0xBAF8,
    0x38C5,
    0xBA53,
    0x3851,
    0xB9AE,
    0x37DD,
    0xB90A,
    0x3769,
    0xB867,
    0x36F6,
    0xB7C5,
    0x3684,
    0xB723,
    0x3612,
    0xB681,
    0x35A0,
    0xB5E1,
    0x352F,
    0xB541,
    0x34BF,
    0xB4A2,
    0x344F,
    0xB404,
    0x33DF,
    0xB366,
    0x3370,
    0xB2C9,
    0x3302,
    0xB22C,
    0x3293,
    0xB191,
    0x3226,
    0xB0F5,
    0x31B9,
    0xB05B,
    0x314C,
    0xAFC1,
    0x30DF,
    0xAF28,
    0x3074,
    0xAE8F,
    0x3008,
    0xADF7,
    0x2F9D,
    0xAD60,
    0x2F33,
    0xACC9,
    0x2EC8,
    0xAC33,
    0x2E5F,
    0xAB9E,
    0x2DF6,
    0xAB09,
    0x2D8D,
    0xAA75,
    0x2D24,
    0xA9E1,
    0x2CBC,
    0xA94E,
    0x2C55,
    0xA8BC,
    0x2BEE,
    0xA82A,
    0x2B87,
    0xA799,
    0x2B21,
    0xA708,
    0x2ABB,
    0xA678,
    0x2A55,
    0xA5E8,
    0x29F0,
    0xA559,
    0x298B,
    0xA4CB,
    0x2927,
    0xA43D,
    0x28C3,
    0xA3B0,
    0x2860,
    0xA323,
    0x27FD,
    0xA297,
    0x279A,
    0xA20B,
    0x2738,
    0xA180,
    0x26D6,
    0xA0F6,
    0x2674,
    0xA06C,
    0x2613,
    0x9FE2,
    0x25B2,
    0x9F59,
    0x2552,
    0x9ED1,
    0x24F2,
    0x9E49,
    0x2492,
    0x9DC2,
    0x2432,
    0x9D3B,
    0x23D3,
    0x9CB4,
    0x2375,
    0x9C2F,
    0x2317,
    0x9BA9,
    0x22B9,
    0x9B25,
    0x225B,
    0x9AA0,
    0x21FE,
    0x9A1C,
    0x21A1,
    0x9999,
    0x2145,
    0x9916,
    0x20E8,
    0x9894,
    0x208D,
    0x9812,
    0x2031,
    0x9791,
    0x1FD6,
    0x9710,
    0x1F7B,
    0x968F,
    0x1F21,
    0x960F,
    0x1EC7,
    0x9590,
    0x1E6D,
    0x9511,
    0x1E13,
    0x9492,
    0x1DBA,
    0x9414,
    0x1D61,
    0x9397,
    0x1D09,
    0x931A,
    0x1CB1,
    0x929D,
    0x1C59,
    0x9221,
    0x1C01,
    0x91A5,
    0x1BAA,
    0x9129,
    0x1B53,
    0x90AF,
    0x1AFC,
    0x9034,
    0x1AA6,
    0x8FBA,
    0x1A50,
    0x8F40,
    0x19FA,
    0x8EC7,
    0x19A5,
    0x8E4F,
    0x1950,
    0x8DD6,
    0x18FB,
    0x8D5E,
    0x18A7,
    0x8CE7,
    0x1853,
    0x8C70,
    0x17FF,
    0x8BF9,
    0x17AB,
    0x8B83,
    0x1758,
    0x8B0D,
    0x1705,
    0x8A98,
    0x16B2,
    0x8A23,
    0x1660,
    0x89AE,
    0x160D,
    0x893A,
    0x15BC,
    0x88C6,
    0x156A,
    0x8853,
    0x1519,
    0x87E0,
    0x14C8,
    0x876D,
    0x1477,
    0x86FB,
    0x1426,
    0x8689,
    0x13D6,
    0x8618,
    0x1386,
    0x85A7,
    0x1337,
    0x8536,
    0x12E7,
    0x84C6,
    0x1298,
    0x8456,
    0x1249,
    0x83E7,
    0x11FB,
    0x8377,
    0x11AC,
    0x8309,
    0x115E,
    0x829A,
    0x1111,
    0x822C,
    0x10C3,
    0x81BF,
    0x1076,
    0x8151,
    0x1029,
    0x80E4,
    0x0FDC,
    0x8078,
    0x0F8F,
    0x800C,
    0x0F43,
    0x7FA0,
    0x0EF7,
    0x7F34,
    0x0EAB,
    0x7EC9,
    0x0E60,
    0x7E5E,
    0x0E15,
    0x7DF4,
    0x0DCA,
    0x7D8A,
    0x0D7F,
    0x7D20,
    0x0D34,
    0x7CB6,
    0x0CEA,
    0x7C4D,
    0x0CA0,
    0x7BE5,
    0x0C56,
    0x7B7C,
    0x0C0C,
    0x7B14,
    0x0BC3,
    0x7AAC,
    0x0B7A,
    0x7A45,
    0x0B31,
    0x79DE,
    0x0AE8,
    0x7977,
    0x0AA0,
    0x7911,
    0x0A58,
    0x78AB,
    0x0A10,
    0x7845,
    0x09C8,
    0x77DF,
    0x0981,
    0x777A,
    0x0939,
    0x7715,
    0x08F2,
    0x76B1,
    0x08AB,
    0x764D,
    0x0865,
    0x75E9,
    0x081E,
    0x7585,
    0x07D8,
    0x7522,
    0x0792,
    0x74BF,
    0x074D,
    0x745D,
    0x0707,
    0x73FA,
    0x06C2,
    0x7398,
    0x067D,
    0x7337,
    0x0638,
    0x72D5,
    0x05F3,
    0x7274,
    0x05AF,
    0x7213,
    0x056A,
    0x71B3,
    0x0526,
    0x7152,
    0x04E2,
    0x70F2,
    0x049F,
    0x7093,
    0x045B,
    0x7033,
    0x0418,
    0x6FD4,
    0x03D5,
    0x6F76,
    0x0392,
    0x6F17,
    0x0350,
    0x6EB9,
    0x030D,
    0x6E5B,
    0x02CB,
    0x6DFD,
    0x0289,
    0x6DA0,
    0x0247,
    0x6D43,
    0x0206,
    0x6CE6,
    0x01C4,
    0x6C8A,
    0x0183,
    0x6C2D,
    0x0142,
    0x6BD1,
    0x0101,
    0x6B76,
    0x00C0,
    0x6B1A,
    0x0080,
    0x6ABF,
    0x0040,
    0x6A64
};

enum {
    SP_DIV_SQRT_NO,
    SP_DIV_SQRT_YES
};
enum {
    SP_DIV_PRECISION_SINGLE = 0,
    SP_DIV_PRECISION_DOUBLE = 1,
    SP_DIV_PRECISION_CURRENT
};

INLINE static void do_div(int data, int sqrt, int precision)
{
    int32_t addr;
    int fetch;
    int shift;

    if (precision == SP_DIV_PRECISION_SINGLE)
        data = (data < 0) ? -data : +data;
    if (precision == SP_DIV_PRECISION_DOUBLE && data < 0)
        data = (data >= -32768) ? -data : ~data;

/*
 * Note, from the code just above, that data cannot be negative.
 * (data >= 0) is unconditionally forced by the above algorithm.
 */
    addr = data;
    if (data == 0x00000000)
    {
        shift = (precision == SP_DIV_PRECISION_SINGLE) ? 16 : 0;
        addr = addr << shift;
    }
    else
        for (shift = 0; addr >= 0x00000000; addr <<= 1, shift++);
    addr = (addr >> 22) & 0x000001FF;

    if (sqrt == SP_DIV_SQRT_YES)
    {
        addr &= 0x000001FE;
        addr |= 0x00000200 | (shift & 1);
    }
    fetch = div_ROM[addr];
    shift ^= 31; /* flipping shift direction from left- to right- */
    shift >>= (sqrt == SP_DIV_SQRT_YES);
    DivOut = (0x40000000 | (fetch << 14)) >> shift;
    if (DivIn == 0) /* corner case:  overflow via division by zero */
        DivOut = 0x7FFFFFFF;
    else if (DivIn == -32768) /* corner case:  signed underflow barrier */
        DivOut = 0xFFFF0000;
    else
        DivOut ^= (DivIn < 0) ? ~0 : 0;
    return;
}
#endif
