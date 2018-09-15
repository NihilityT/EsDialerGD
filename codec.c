//
// Created by ChileungL on 23/05/2018.
//

/* 接口内容编解码
 *
 * 通信内容用了个令人excited的方式进行编码，预先打一个足够大的表即可解决。只开了2048字节，够用了。
 * 需要编解码的时候把通信内容与下面这个大大的东西逐个xor，即可得到加密或解密的结果。
 */

#include "md5.h"
#include "codec.h"

u_char magic_box[2048] = {
        0x33, 0xE2, 0x73, 0x9B, 0x7D, 0x6E, 0x34, 0x52, 0x08, 0x75, 0x27, 0x97, 0x10, 0x1D, 0xF7, 0xB1, 0x02, 0xB2,
        0xF5, 0xC1, 0x99, 0x3A, 0x43, 0x96, 0x23, 0xC5, 0x35, 0x32, 0x06, 0x05, 0x6C, 0xF1, 0x04, 0x53, 0x5B, 0x7E,
        0xCB, 0xF8, 0xEF, 0x09, 0x87, 0xDC, 0xCE, 0x2F, 0x5D, 0x28, 0x3F, 0x77, 0x0F, 0x84, 0xDB, 0xF3, 0xDB, 0xA7,
        0xD8, 0xB2, 0x72, 0x10, 0x1D, 0x93, 0x66, 0x72, 0x69, 0xFF, 0x9F, 0xE1, 0x62, 0x27, 0x78, 0x58, 0x33, 0xBD,
        0xE8, 0x20, 0x27, 0x90, 0xD3, 0x31, 0xA7, 0xEF, 0x05, 0xC2, 0x97, 0x69, 0x9D, 0xA7, 0x0E, 0x53, 0xF6, 0xCC,
        0xBC, 0x04, 0x97, 0x29, 0xFC, 0xF4, 0xF7, 0x7E, 0x90, 0x49, 0xF2, 0x03, 0xA9, 0x9D, 0x11, 0x04, 0x75, 0x48,
        0xC1, 0xC8, 0xC3, 0x70, 0xF1, 0x00, 0x35, 0xA0, 0x66, 0xDF, 0x97, 0x73, 0x8E, 0x02, 0xF0, 0xBD, 0x83, 0x02,
        0xEC, 0xF5, 0x2A, 0x64, 0x61, 0x70, 0xBA, 0x66, 0x29, 0x4A, 0xEF, 0x2B, 0x3B, 0x32, 0x2F, 0x49, 0x02, 0xE4,
        0xD3, 0x7A, 0x65, 0x94, 0x87, 0xEF, 0x18, 0xF9, 0x4E, 0x4D, 0xE4, 0x18, 0xB4, 0x2A, 0x1C, 0x66, 0x33, 0x02,
        0x1F, 0xB4, 0xBD, 0xAF, 0x09, 0x72, 0x11, 0xCE, 0x76, 0x01, 0x81, 0xB2, 0xF8, 0x21, 0x22, 0x9C, 0xE5, 0x29,
        0xF6, 0x89, 0x42, 0xFB, 0x8D, 0xA5, 0x09, 0xA3, 0x58, 0xAA, 0x64, 0x78, 0x74, 0xB2, 0x12, 0x69, 0x7E, 0xA1,
        0xC5, 0x78, 0x72, 0xB8, 0x22, 0xCE, 0x3D, 0xBB, 0xD4, 0x17, 0x5C, 0x9D, 0x31, 0x20, 0xF8, 0x94, 0x2B, 0x75,
        0xBC, 0x8D, 0x81, 0xA0, 0x97, 0xC2, 0xCF, 0xEF, 0x82, 0xA8, 0x7C, 0xCE, 0xEB, 0xF7, 0xDE, 0xBD, 0xC0, 0xFB,
        0x8B, 0x41, 0xA4, 0x2A, 0xC7, 0xC1, 0x91, 0x9E, 0x92, 0xD6, 0xBF, 0xEE, 0x94, 0x89, 0x03, 0xB3, 0x6A, 0xE4,
        0x57, 0x13, 0x33, 0xC0, 0xF1, 0x3D, 0x6D, 0xF8, 0x86, 0x25, 0xE3, 0x70, 0xBD, 0x2C, 0x50, 0x79, 0x3A, 0xDC,
        0xAE, 0x8E, 0x86, 0x81, 0xF8, 0x09, 0x93, 0x7F, 0xF5, 0xBD, 0x99, 0xE5, 0x5D, 0x83, 0xAA, 0xAA, 0xCE, 0x5E,
        0xBB, 0xDC, 0x7C, 0x97, 0xF7, 0x68, 0x44, 0x8B, 0xB3, 0x8C, 0x0A, 0x7C, 0xBE, 0x9D, 0x83, 0x31, 0x8A, 0x48,
        0x99, 0xC7, 0x53, 0x48, 0x8C, 0x6A, 0x9A, 0x11, 0x99, 0x78, 0x04, 0xCD, 0xC8, 0xB6, 0x13, 0xE2, 0xFB, 0xE5,
        0xCC, 0x46, 0x62, 0x5D, 0x91, 0xA4, 0x61, 0xD4, 0x4C, 0xF3, 0xCA, 0x90, 0x7D, 0x96, 0x7B, 0x96, 0x26, 0xA6,
        0x24, 0xF3, 0x0D, 0xBF, 0xC5, 0xBD, 0xEC, 0x09, 0xAE, 0xCE, 0xBB, 0x7E, 0x58, 0xC0, 0x4E, 0x9B, 0xCA, 0xA6,
        0xC5, 0xF3, 0xF9, 0x37, 0x73, 0x7D, 0x6D, 0x2E, 0xA1, 0x0F, 0xC6, 0x64, 0x42, 0xF4, 0x52, 0x7E, 0x9B, 0xFB,
        0x1A, 0x79, 0x45, 0x1D, 0x36, 0x54, 0x2D, 0xF8, 0x81, 0xB2, 0x57, 0x56, 0xF2, 0x13, 0x3B, 0xE6, 0xD1, 0x5F,
        0x2D, 0x53, 0xBE, 0xAA, 0xE2, 0x8C, 0x34, 0x78, 0xF9, 0x50, 0x33, 0x95, 0x6B, 0xF5, 0xB0, 0xEE, 0x7B, 0xA0,
        0xAD, 0x7C, 0xC5, 0xA9, 0x86, 0x88, 0x51, 0x09, 0x31, 0xC2, 0x5A, 0x86, 0x09, 0x84, 0x47, 0x2C, 0xFC, 0x14,
        0x5F, 0x30, 0x3A, 0xCB, 0xC5, 0x53, 0x8E, 0x01, 0x93, 0xE6, 0x21, 0xC4, 0xFF, 0x55, 0x46, 0xE4, 0xA7, 0x5A,
        0xBF, 0xB0, 0x77, 0xAB, 0x16, 0xCB, 0x7E, 0x5C, 0x6B, 0x2A, 0xCE, 0xB5, 0x77, 0xB2, 0x8D, 0x13, 0xB8, 0xCF,
        0x15, 0x10, 0x08, 0xBC, 0x4B, 0x58, 0xC2, 0xA1, 0x74, 0x09, 0x54, 0x64, 0x9E, 0x76, 0xCE, 0xE0, 0x54, 0x11,
        0x65, 0xF9, 0x08, 0xC0, 0x6F, 0x67, 0xFE, 0xE6, 0x77, 0xF7, 0x70, 0xE3, 0xA7, 0x32, 0x9A, 0x2C, 0xAE, 0xC0,
        0x81, 0x7F, 0x04, 0x92, 0x9D, 0x08, 0xE9, 0x55, 0x0F, 0xE0, 0xDD, 0x9E, 0x69, 0xE6, 0x17, 0x10, 0x60, 0x2E,
        0xB1, 0xC7, 0xF9, 0x0A, 0x3A, 0xC6, 0x93, 0xAF, 0x1D, 0x56, 0x40, 0x08, 0x59, 0xD4, 0xEF, 0x82, 0xD3, 0x33,
        0x7E, 0x41, 0xA5, 0xC4, 0xD5, 0xFA, 0xDB, 0xEC, 0x14, 0xAB, 0x89, 0xB0, 0x4D, 0xEF, 0xD6, 0x4D, 0xD3, 0x5A,
        0x51, 0x77, 0x72, 0x20, 0x44, 0x61, 0x2A, 0x77, 0x07, 0x22, 0xA6, 0x99, 0xFB, 0x94, 0xB7, 0x56, 0xB9, 0x81,
        0x49, 0x78, 0x84, 0x91, 0xB2, 0x19, 0x69, 0xF5, 0x81, 0xA4, 0xE2, 0x40, 0x70, 0x3C, 0x6B, 0xCE, 0x8A, 0xBE,
        0x46, 0xFB, 0xC3, 0x20, 0xDA, 0x7E, 0x27, 0x94, 0x79, 0xBB, 0x82, 0x6E, 0x0E, 0xF6, 0x2A, 0x30, 0xFD, 0x60,
        0x35, 0x66, 0xC4, 0xE6, 0xEF, 0xAF, 0x13, 0x83, 0x4A, 0x13, 0x46, 0x02, 0xE6, 0x31, 0xFC, 0xB5, 0x5A, 0xDF,
        0xD5, 0x65, 0x1F, 0x59, 0xCE, 0x9B, 0x8A, 0xEE, 0x52, 0x4B, 0xD7, 0x10, 0x7C, 0xB6, 0x82, 0x57, 0x1A, 0xBF,
        0x81, 0x34, 0x7E, 0xE7, 0xC8, 0xD4, 0x76, 0x92, 0xD5, 0x9C, 0x5A, 0xAC, 0x60, 0x70, 0xAC, 0xA4, 0x20, 0x8D,
        0xEE, 0x2F, 0x52, 0xD7, 0x73, 0x3F, 0x03, 0xB8, 0xC1, 0x99, 0x05, 0x7F, 0x04, 0x6E, 0x84, 0xEB, 0xD6, 0xD9,
        0x55, 0x96, 0x4D, 0x28, 0xAE, 0x3D, 0xFF, 0xAF, 0x2F, 0x0A, 0x59, 0xFC, 0x1F, 0x63, 0x0C, 0xB0, 0xD4, 0xD1,
        0x9E, 0x25, 0x4B, 0xA6, 0xA4, 0x79, 0x67, 0x45, 0x99, 0x33, 0x96, 0x64, 0x8B, 0x30, 0xE1, 0x18, 0xB9, 0x59,
        0x41, 0x4C, 0xFA, 0x20, 0x31, 0x93, 0x4C, 0xFD, 0xED, 0xFD, 0xB1, 0x94, 0x8C, 0x8A, 0x37, 0xA6, 0x50, 0x42,
        0x34, 0x82, 0x26, 0xFB, 0xE9, 0x24, 0x0E, 0x89, 0x59, 0x1B, 0xB2, 0x6A, 0x53, 0xF5, 0x44, 0xFF, 0x7E, 0x00,
        0xFD, 0xF4, 0x79, 0xA9, 0xBD, 0xB4, 0x9C, 0x21, 0x0B, 0xB0, 0x21, 0x67, 0xDB, 0xE0, 0x09, 0x32, 0x6C, 0x6F,
        0xC9, 0x0B, 0x69, 0xF3, 0xE9, 0xDF, 0x98, 0xAD, 0x4E, 0x33, 0x45, 0x02, 0x83, 0xB6, 0x21, 0x0B, 0x9A, 0xD9,
        0x58, 0xB4, 0x88, 0xA8, 0x4C, 0xF7, 0x02, 0x9B, 0x6E, 0xAD, 0x8B, 0x85, 0xA1, 0x26, 0xEA, 0x19, 0x4F, 0xE9,
        0x7C, 0x00, 0x66, 0x04, 0x44, 0xBE, 0x34, 0x4C, 0xE0, 0xAB, 0x79, 0xA9, 0xCC, 0x9D, 0xED, 0x57, 0xB7, 0x29,
        0x29, 0xAE, 0x0E, 0x19, 0xF4, 0x2F, 0xE4, 0x60, 0x31, 0x75, 0x84, 0x0C, 0x4E, 0x34, 0x6E, 0x8F, 0x1F, 0x92,
        0x78, 0x15, 0x23, 0xBB, 0xE8, 0xEC, 0xA0, 0x23, 0x60, 0x29, 0xC3, 0x2C, 0xFB, 0x8A, 0x74, 0xA1, 0xAD, 0xF3,
        0x46, 0x17, 0x90, 0x20, 0xD7, 0x62, 0x36, 0x38, 0x92, 0xB5, 0xC9, 0xF2, 0x75, 0xB0, 0x7F, 0x84, 0x8D, 0x0F,
        0x05, 0xC7, 0xC3, 0xD1, 0x2D, 0xE8, 0x00, 0x98, 0x29, 0xA2, 0x72, 0x86, 0x4E, 0x2E, 0xBA, 0x3C, 0x80, 0x53,
        0xB9, 0x27, 0xED, 0xE5, 0xFE, 0x85, 0xC5, 0x0F, 0xCF, 0xB2, 0x7A, 0x4B, 0xE8, 0x2C, 0x7D, 0xC8, 0xE2, 0xFA,
        0x51, 0x19, 0xDE, 0x74, 0x17, 0x2D, 0x86, 0x41, 0x4A, 0xD2, 0xF1, 0x7D, 0x32, 0x24, 0x1A, 0x86, 0xEC, 0xFF,
        0x08, 0x2B, 0x87, 0x11, 0x6D, 0x81, 0x6F, 0xCD, 0xA9, 0xD3, 0x3A, 0xB5, 0x83, 0xA5, 0x16, 0x67, 0xD7, 0x68,
        0xDD, 0xE0, 0xB8, 0x99, 0xA8, 0x7E, 0x41, 0x6D, 0xCC, 0xCB, 0x75, 0x2B, 0x33, 0xAD, 0x82, 0x54, 0x0B, 0xEC,
        0x1A, 0x17, 0xC7, 0x49, 0x0D, 0xBD, 0x4C, 0x88, 0x6C, 0xBB, 0x4C, 0x27, 0xC2, 0xED, 0xFF, 0x1D, 0x1A, 0xA9,
        0xEE, 0xAD, 0xB1, 0x82, 0x45, 0x51, 0xA5, 0x3F, 0xCB, 0xE7, 0xFA, 0xA8, 0x6D, 0x98, 0x7C, 0x0E, 0x87, 0xE6,
        0x74, 0xF1, 0x31, 0x1E, 0xF3, 0xC3, 0xDD, 0x6C, 0xFD, 0x8E, 0x44, 0x3F, 0x7A, 0xAE, 0x45, 0x8A, 0x1B, 0x85,
        0xB7, 0xB8, 0xCC, 0xCC, 0xD6, 0x04, 0xDD, 0xBF, 0x12, 0xCF, 0x73, 0x33, 0x49, 0xEB, 0x7B, 0x13, 0x38, 0x94,
        0x21, 0x24, 0x31, 0x63, 0x2A, 0x31, 0x0F, 0x41, 0x50, 0xBE, 0x82, 0xA8, 0x14, 0x60, 0xB3, 0x2E, 0x2C, 0x0B,
        0x8E, 0x91, 0xF4, 0x1D, 0x1A, 0x66, 0x37, 0xCE, 0xA5, 0x9D, 0xC3, 0x4F, 0xEB, 0xCF, 0xBC, 0xA9, 0x8B, 0x63,
        0x96, 0xA2, 0x98, 0x37, 0x96, 0x43, 0x37, 0x78, 0x56, 0xB5, 0xDA, 0xA8, 0xD9, 0xE0, 0x8E, 0xB3, 0xA3, 0x5C,
        0xA2, 0xBC, 0x9D, 0x96, 0x53, 0xDC, 0x00, 0xCD, 0x58, 0x84, 0x49, 0x0D, 0xB4, 0x6F, 0xE3, 0x1F, 0x4C, 0x78,
        0xEB, 0xD8, 0xD7, 0x40, 0xB4, 0x2A, 0x14, 0x6C, 0x33, 0x08, 0x3C, 0x95, 0x3D, 0xAE, 0xB2, 0x13, 0x25, 0xA7,
        0x14, 0x0B, 0xC3, 0x98, 0x28, 0xE3, 0xDF, 0x12, 0x27, 0x14, 0x8E, 0xC6, 0x25, 0xB7, 0x1D, 0x71, 0x2C, 0xDB,
        0xDF, 0x89, 0x8E, 0x64, 0x88, 0x1E, 0x99, 0xF9, 0x2B, 0x7C, 0xDF, 0xBD, 0x9F, 0xD5, 0x85, 0xA2, 0x5C, 0xFF,
        0xA4, 0xE4, 0x8F, 0xD5, 0x60, 0xFC, 0xFD, 0x41, 0x55, 0x13, 0x62, 0xA2, 0xB4, 0xA1, 0x9B, 0xF6, 0x92, 0x59,
        0x80, 0x51, 0x55, 0xAB, 0xC4, 0xA3, 0x2A, 0xB2, 0xA3, 0x61, 0x54, 0x4B, 0xD7, 0x52, 0x17, 0x25, 0xC8, 0xBA,
        0xC0, 0x7B, 0x01, 0x8F, 0xF9, 0xD0, 0xA8, 0x82, 0x01, 0x9D, 0x23, 0xC3, 0xB0, 0xD5, 0xEB, 0xF1, 0xE4, 0x5D,
        0x2D, 0xEB, 0x6B, 0x8C, 0xBB, 0xEC, 0x12, 0xA2, 0x2D, 0xE0, 0x7A, 0xFF, 0x4C, 0xB4, 0x59, 0x4D, 0x12, 0x96,
        0x8B, 0x15, 0xB5, 0x90, 0xA2, 0x23, 0x8A, 0x87, 0xCD, 0x7D, 0xCC, 0x43, 0x17, 0x4C, 0x7E, 0xC1, 0xD7, 0xD7,
        0x0F, 0x99, 0x36, 0xE6, 0x1B, 0x59, 0x5B, 0x22, 0xA9, 0x1D, 0x01, 0xC8, 0x12, 0x6D, 0xEC, 0x39, 0x41, 0x52,
        0x6D, 0xC9, 0xF4, 0x7A, 0xCF, 0x22, 0x6B, 0x8A, 0x50, 0xFB, 0x50, 0x00, 0xD6, 0x64, 0x04, 0xFC, 0xA6, 0x2E,
        0xBB, 0xB4, 0x58, 0xB5, 0x6C, 0x5E, 0x9C, 0x00, 0x67, 0xD6, 0x1E, 0x57, 0x82, 0x90, 0xC8, 0x09, 0x41, 0x9A,
        0xFB, 0xA6, 0x86, 0x6E, 0x50, 0x49, 0xEF, 0x7D, 0xEF, 0xF8, 0xF8, 0x55, 0x7F, 0x17, 0x2A, 0xF9, 0x59, 0x0A,
        0xD8, 0xB5, 0xA2, 0x52, 0x6A, 0x1B, 0x58, 0xCD, 0x48, 0xFB, 0x60, 0x52, 0x93, 0x10, 0x2A, 0x17, 0xDA, 0x79,
        0xC0, 0x87, 0x7E, 0x59, 0x9C, 0xF6, 0xF6, 0x71, 0x30, 0x3D, 0xF3, 0xE9, 0x68, 0x38, 0x98, 0x17, 0x4C, 0xD3,
        0x66, 0xAC, 0xCF, 0xB8, 0xD8, 0x59, 0x41, 0xFA, 0xD5, 0x59, 0x48, 0xEB, 0xBA, 0x3D, 0x6C, 0x81, 0x09, 0x56,
        0xED, 0x34, 0xCE, 0x2A, 0x53, 0x67, 0xA6, 0xC1, 0x03, 0xC3, 0x5E, 0xAC, 0x9D, 0x89, 0x28, 0x08, 0x25, 0xCD,
        0x73, 0x89, 0xFF, 0xAB, 0xFC, 0xDC, 0xF5, 0xFE, 0xAC, 0x18, 0x18, 0xF0, 0x54, 0xAC, 0x70, 0x4E, 0x2A, 0x54,
        0xBB, 0x9D, 0x5C, 0xE1, 0xDB, 0x24, 0x1A, 0xA9, 0xBA, 0x9A, 0x10, 0x09, 0xC7, 0x60, 0x90, 0x49, 0x23, 0x77,
        0x4F, 0x23, 0xEB, 0xD3, 0x1D, 0x03, 0xB6, 0xCF, 0xF4, 0x94, 0x79, 0x5D, 0x88, 0x37, 0x56, 0x87, 0xD3, 0x98,
        0xBD, 0xB9, 0x05, 0x88, 0xA0, 0x73, 0x23, 0x4E, 0x4E, 0x3F, 0x07, 0x65, 0x24, 0xE6, 0x66, 0xFC, 0xDD, 0x89,
        0x4D, 0x9E, 0x09, 0xEF, 0xC6, 0x5B, 0x83, 0x97, 0x56, 0xE8, 0x1C, 0x15, 0xC2, 0x6D, 0x68, 0xAE, 0xBA, 0x81,
        0x96, 0xA0, 0x6E, 0xF8, 0xAB, 0x8D, 0x84, 0x13, 0x0D, 0xBB, 0xFA, 0xA7, 0x3B, 0xAF, 0x10, 0x58, 0xB2, 0xB7,
        0x0E, 0x65, 0x7A, 0x13, 0x09, 0x2B, 0xAA, 0x78, 0xBF, 0xB2, 0x88, 0xFA, 0xB6, 0x27, 0xC8, 0x85, 0x56, 0x47,
        0x5D, 0x72, 0x59, 0x4F, 0x0F, 0x54, 0x6A, 0x15, 0x62, 0xBD, 0x23, 0x1F, 0x52, 0x6A, 0x2C, 0x30, 0xA5, 0xE8,
        0x86, 0x8C, 0x90, 0xA6, 0xE3, 0x14, 0x48, 0x41, 0xD5, 0x35, 0x9B, 0xE4, 0x46, 0x82, 0x08, 0x11, 0xAA, 0x89,
        0x27, 0x84, 0xE6, 0x56, 0x17, 0x40, 0xA7, 0x72, 0x33, 0xDF, 0xD6, 0x55, 0x5F, 0x45, 0x25, 0xE9, 0x04, 0x9B,
        0x25, 0xA3, 0xD0, 0x80, 0xF0, 0xFB, 0xF2, 0x02, 0x96, 0xD9, 0xC2, 0x4E, 0xB5, 0x70, 0x5A, 0x51, 0x93, 0x40,
        0xD2, 0xE9, 0x7C, 0x20, 0x19, 0x46, 0xEF, 0x02, 0xFF, 0x7F, 0xD2, 0x71, 0x58, 0x75, 0x83, 0xCD, 0xB3, 0xEA,
        0xC1, 0x3E, 0xB3, 0x37, 0xE3, 0x28, 0x3D, 0xC2, 0xB5, 0x31, 0x20, 0x59, 0x71, 0xBD, 0x0D, 0x87, 0x92, 0x6D,
        0x06, 0xEB, 0x02, 0x35, 0xCC, 0x0D, 0x11, 0xDE, 0xAE, 0x89, 0x78, 0x34, 0x74, 0x2D, 0x61, 0x01, 0xD8, 0x15,
        0xEF, 0xA2, 0x29, 0xA6, 0xA4, 0x59, 0x95, 0xAF, 0xC9, 0xF0, 0x9A, 0x19, 0x91, 0x59, 0x76, 0xCC, 0xCE, 0x79,
        0xF0, 0xC8, 0x5F, 0xE3, 0x9B, 0x3D, 0x4C, 0x1D, 0xFA, 0x2A, 0xF5, 0xE3, 0x07, 0xEC, 0xE0, 0x04, 0x0A, 0x23,
        0x6C, 0xA8, 0xFE, 0x11, 0x86, 0x06, 0x6B, 0xB9, 0x44, 0x1E, 0x4C, 0x9B, 0x79, 0xC1, 0x8E, 0xEA, 0xB9, 0xDC,
        0x87, 0x52, 0x26, 0x54, 0x6D, 0x3E, 0x70, 0x4F, 0xCC, 0x3B, 0xE2, 0x9B, 0x10, 0x29, 0x01, 0x0B, 0x00, 0x66,
        0xA9, 0x70, 0x4E, 0x34, 0x34, 0xA1, 0xFC, 0xEA, 0xC3, 0x2C, 0xD3, 0xC3, 0x2E, 0xF9, 0x36, 0x76, 0x14, 0xEB,
        0xDD, 0xFF, 0x26, 0xDB, 0x07, 0x00, 0xC3, 0x26, 0xC7, 0xCD, 0x92, 0x60, 0x6C, 0x84, 0xFD, 0xEA, 0x5C, 0x2D,
        0xC2, 0x6B, 0x57, 0xE8, 0x73, 0xB0, 0x06, 0x4A, 0x9F, 0x53, 0x3F, 0xCA, 0xAD, 0xE6, 0x0B, 0xC8, 0xA3, 0xE8,
        0x25, 0x12, 0xD2, 0x10, 0xAE, 0x9E, 0xF8, 0x28, 0xF5, 0x94, 0xF9, 0xF4, 0xEC, 0x13, 0x3D, 0x60, 0x1C, 0xA4,
        0x8F, 0x70, 0xF2, 0xDA, 0x84, 0x67, 0x11, 0x6A, 0x17, 0x6B, 0xCA, 0xB7, 0x1E, 0xCC, 0xF5, 0xD3, 0x15, 0x3E,
        0xC6, 0x3B, 0x42, 0x9A, 0x98, 0x7A, 0x9A, 0xC6, 0x20, 0xED, 0xE8, 0xA8, 0xAA, 0xF1, 0x07, 0x15, 0x72, 0x7D,
        0x63, 0xE2, 0x86, 0xD2, 0x39, 0xC9, 0x91, 0xE9, 0x71, 0xDE, 0xD7, 0x1A, 0x87, 0xAA, 0xFA, 0x77, 0x03, 0x5C,
        0x77, 0xCE, 0x40, 0x19, 0xB1, 0xDB, 0x5C, 0x67, 0x4F, 0xD3, 0xAE, 0xC4, 0xB7, 0x2D, 0x69, 0xF2, 0xD5, 0x12,
        0xE8, 0x15, 0xB6, 0x7F, 0xF0, 0x7F, 0x1C, 0x71, 0xE1, 0x96, 0x8F, 0x2B, 0xBA, 0x13, 0xD3, 0x31, 0x6C, 0x67,
        0x25, 0x08, 0xC9, 0x08, 0xF9, 0x09, 0x4A, 0xF6, 0x6F, 0x03, 0xF1, 0xCD, 0x71, 0xBC, 0x40, 0xAA, 0x7C, 0x2A,
        0x59, 0x9E, 0x46, 0xBE, 0x9E, 0xFC, 0xF1, 0x44, 0x90, 0xED, 0x56, 0xDB, 0x02, 0x25, 0xBC, 0xF0, 0x20, 0xF0,
        0xC0, 0xF3, 0x35, 0x32, 0x67, 0xB6, 0x15, 0xA4, 0x00, 0x16, 0x8C, 0xE2, 0xF0, 0x0D, 0x59, 0xB5, 0x95, 0x08,
        0xE5, 0x23, 0x24, 0x46, 0x0F, 0xC9, 0xDB, 0xA3, 0xFE, 0xBA, 0xA5, 0x92, 0x53, 0x2B, 0xF7, 0x62, 0xA8, 0xB2,
        0x56, 0x33, 0xB4, 0xBD, 0xAF, 0xBA, 0x6D, 0x2A, 0x6A, 0xCE, 0xEE, 0x47, 0x94, 0x32, 0xC7, 0xB3, 0xD2, 0x83,
        0x5E, 0x8D, 0x3D, 0xC3, 0xE8, 0x2C, 0xFE, 0x4C, 0x69, 0x16, 0xC8, 0xC9, 0xE1, 0x16, 0xFA, 0xBB, 0x83, 0x76,
        0xE0, 0x09, 0x8C, 0x1A, 0x64, 0xD6, 0xEE, 0xDD, 0x39, 0x90, 0xC0, 0xEE, 0x5A, 0x07, 0x3D, 0x9C, 0x48, 0x0B,
        0x1A, 0x93, 0x10, 0x15, 0x09, 0xAC, 0x9B, 0x08, 0xAA, 0x3B, 0xFD, 0x44, 0x14, 0x3B, 0x0D, 0x5D, 0x32, 0xB4,
        0xE5, 0xEB, 0x63, 0x55, 0x23, 0xF3, 0x78, 0xEE, 0xB0, 0x77, 0xAD, 0x63, 0x2F, 0xE1,
};

void fuck(u_char *data, size_t len) {
    if (len > sizeof(magic_box))
        return;

    for (size_t i = 0; i < len; i++)
        data[i] ^= magic_box[i];
}

void bin2hex(char *hex, const u_char *bin, size_t bin_len) {
    for (size_t i = 0; i < bin_len; i++)
        sprintf(hex + (i << 1), "%02X", bin[i]);
}

void hex2bin(u_char *bin, const char *hex) {
    for (size_t i = 0; hex[i << 1] && hex[i << 1 | 1]; i++)
        sscanf(hex + (i << 1), "%02X", bin + i);
}

void payload_encode(char *data, char *md5_hex, const u_char *buf) {
    size_t buf_len = strlen((const char *) buf);
    fuck((u_char *) buf, buf_len);
    bin2hex(data, buf, buf_len);

    char md5_bin[HASHSIZE] = {0};
    md5(data, (buf_len << 1), md5_bin);
    bin2hex(md5_hex, (const u_char *) md5_bin, sizeof(md5_bin));
}

void payload_decode(u_char *data, const u_char *buf, size_t buf_len) {
    hex2bin(data, (const char *) buf);
    fuck(data, buf_len >> 1);
}
