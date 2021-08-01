
/////////////////////////////////////////////////////////////////////////////
//
// Palette.cpp - Palette Functions
//	 NyteOwl, Sep 2013
//
/////////////////////////////////////////////////////////////////////////////

#pragma unmanaged

#include "inc/stdafx.h"
#include "inc/mapx.h"


/////////////////////////////////////////////////////////////////////////////
// Macros
/////////////////////////////////////////////////////////////////////////////

#define HASHTABLESIZE 1031

#define BITMASK32 0x0F0F0F


/////////////////////////////////////////////////////////////////////////////
// Data Types
/////////////////////////////////////////////////////////////////////////////

typedef struct t_ncs_pal_node_s
{

    struct t_ncs_pal_node_s *next;
    int data;
    int count;

} PAL_NODE, *PPALNODE;


/////////////////////////////////////////////////////////////////////////////
// Data
/////////////////////////////////////////////////////////////////////////////

static PAL_NODE **m_hash_table = NULL;

// Palette from Jedi Knight
static RGBColor m_pal_jedi[256] = {
    {255, 0, 255, 0},
    {0, 255, 0, 0},
    {0, 222, 0, 0},
    {0, 186, 0, 0},
    {0, 149, 0, 0},
    {0, 101, 0, 0},
    {255, 0, 0, 0},
    {222, 0, 0, 0},
    {186, 0, 0, 0},
    {149, 0, 0, 0},
    {101, 0, 0, 0},
    {250, 255, 0, 0},
    {230, 194, 0, 0},
    {202, 129, 0, 0},
    {170, 68, 0, 0},
    {137, 0, 0, 0},
    {255, 255, 255, 0},
    {234, 238, 255, 0},
    {214, 230, 255, 0},
    {194, 214, 255, 0},
    {170, 202, 255, 0},
    {255, 198, 0, 0},
    {255, 190, 0, 0},
    {255, 182, 0, 0},
    {255, 170, 0, 0},
    {255, 149, 0, 0},
    {255, 133, 0, 0},
    {255, 113, 0, 0},
    {255, 93, 0, 0},
    {255, 72, 0, 0},
    {255, 40, 0, 0},
    {0, 0, 255, 0},
    {255, 255, 255, 0},
    {250, 250, 250, 0},
    {246, 246, 246, 0},
    {238, 238, 238, 0},
    {230, 230, 230, 0},
    {226, 226, 226, 0},
    {222, 222, 222, 0},
    {214, 214, 214, 0},
    {210, 210, 210, 0},
    {206, 206, 206, 0},
    {198, 198, 198, 0},
    {194, 194, 194, 0},
    {186, 186, 186, 0},
    {182, 182, 182, 0},
    {174, 174, 174, 0},
    {170, 170, 170, 0},
    {161, 161, 161, 0},
    {153, 153, 153, 0},
    {149, 149, 149, 0},
    {141, 141, 141, 0},
    {129, 129, 129, 0},
    {121, 121, 121, 0},
    {113, 113, 113, 0},
    {105, 105, 105, 0},
    {97, 97, 97, 0},
    {89, 89, 89, 0},
    {76, 76, 76, 0},
    {68, 68, 68, 0},
    {56, 56, 56, 0},
    {40, 40, 40, 0},
    {24, 24, 24, 0},
    {0, 0, 0, 0},
    {214, 218, 234, 0},
    {206, 214, 230, 0},
    {206, 206, 222, 0},
    {198, 206, 222, 0},
    {194, 198, 214, 0},
    {190, 194, 210, 0},
    {182, 190, 206, 0},
    {182, 182, 202, 0},
    {174, 178, 194, 0},
    {170, 174, 194, 0},
    {165, 170, 186, 0},
    {157, 161, 182, 0},
    {153, 157, 174, 0},
    {149, 149, 170, 0},
    {145, 149, 165, 0},
    {137, 141, 157, 0},
    {133, 137, 153, 0},
    {129, 129, 149, 0},
    {121, 125, 141, 0},
    {117, 117, 137, 0},
    {109, 113, 129, 0},
    {105, 105, 121, 0},
    {97, 101, 117, 0},
    {93, 93, 109, 0},
    {89, 89, 101, 0},
    {76, 80, 97, 0},
    {72, 72, 89, 0},
    {68, 68, 76, 0},
    {56, 60, 72, 0},
    {48, 48, 60, 0},
    {40, 40, 48, 0},
    {32, 32, 32, 0},
    {255, 222, 206, 0},
    {238, 202, 178, 0},
    {222, 178, 149, 0},
    {206, 157, 129, 0},
    {190, 133, 105, 0},
    {170, 113, 80, 0},
    {149, 93, 60, 0},
    {129, 72, 40, 0},
    {255, 255, 0, 0},
    {238, 214, 0, 0},
    {218, 178, 0, 0},
    {198, 141, 0, 0},
    {182, 105, 0, 0},
    {157, 68, 0, 0},
    {133, 32, 0, 0},
    {109, 0, 0, 0},
    {234, 255, 194, 0},
    {222, 246, 170, 0},
    {214, 234, 145, 0},
    {206, 222, 117, 0},
    {194, 214, 93, 0},
    {190, 202, 68, 0},
    {182, 190, 32, 0},
    {178, 182, 0, 0},
    {218, 141, 68, 0},
    {206, 129, 56, 0},
    {198, 117, 48, 0},
    {186, 105, 32, 0},
    {178, 97, 24, 0},
    {165, 89, 24, 0},
    {153, 76, 0, 0},
    {145, 68, 0, 0},
    {255, 0, 0, 0},
    {238, 0, 0, 0},
    {218, 0, 0, 0},
    {198, 0, 0, 0},
    {178, 0, 0, 0},
    {153, 0, 0, 0},
    {129, 0, 0, 0},
    {97, 0, 0, 0},
    {165, 194, 218, 0},
    {137, 165, 198, 0},
    {109, 137, 182, 0},
    {80, 109, 161, 0},
    {56, 76, 137, 0},
    {32, 48, 113, 0},
    {0, 24, 89, 0},
    {0, 0, 56, 0},
    {214, 153, 113, 0},
    {206, 149, 105, 0},
    {202, 141, 101, 0},
    {194, 133, 93, 0},
    {186, 129, 89, 0},
    {182, 121, 80, 0},
    {170, 113, 72, 0},
    {165, 109, 68, 0},
    {153, 101, 60, 0},
    {149, 97, 56, 0},
    {141, 89, 48, 0},
    {129, 80, 40, 0},
    {121, 76, 40, 0},
    {109, 68, 32, 0},
    {101, 60, 24, 0},
    {93, 56, 24, 0},
    {255, 238, 206, 0},
    {246, 226, 186, 0},
    {234, 214, 170, 0},
    {226, 206, 157, 0},
    {214, 194, 141, 0},
    {206, 182, 125, 0},
    {194, 170, 113, 0},
    {182, 157, 97, 0},
    {174, 145, 80, 0},
    {161, 133, 68, 0},
    {149, 121, 56, 0},
    {137, 109, 40, 0},
    {121, 97, 32, 0},
    {109, 80, 24, 0},
    {93, 68, 0, 0},
    {76, 56, 0, 0},
    {170, 238, 145, 0},
    {153, 222, 125, 0},
    {141, 206, 109, 0},
    {125, 190, 97, 0},
    {113, 174, 80, 0},
    {101, 153, 68, 0},
    {89, 133, 56, 0},
    {72, 113, 40, 0},
    {255, 194, 255, 0},
    {234, 165, 238, 0},
    {214, 137, 222, 0},
    {194, 109, 206, 0},
    {170, 80, 190, 0},
    {149, 56, 170, 0},
    {121, 24, 149, 0},
    {97, 0, 133, 0},
    {182, 198, 186, 0},
    {170, 190, 178, 0},
    {165, 182, 170, 0},
    {153, 170, 161, 0},
    {149, 161, 149, 0},
    {137, 149, 145, 0},
    {129, 145, 133, 0},
    {117, 133, 125, 0},
    {109, 121, 113, 0},
    {101, 109, 101, 0},
    {89, 97, 93, 0},
    {76, 80, 76, 0},
    {60, 68, 68, 0},
    {48, 56, 48, 0},
    {24, 32, 24, 0},
    {0, 0, 0, 0},
    {230, 186, 101, 0},
    {222, 174, 93, 0},
    {214, 170, 80, 0},
    {210, 157, 76, 0},
    {202, 149, 68, 0},
    {194, 137, 60, 0},
    {186, 125, 56, 0},
    {178, 117, 48, 0},
    {170, 105, 40, 0},
    {161, 97, 32, 0},
    {149, 89, 24, 0},
    {141, 76, 24, 0},
    {133, 68, 0, 0},
    {121, 56, 0, 0},
    {109, 48, 0, 0},
    {101, 40, 0, 0},
    {137, 56, 24, 0},
    {121, 48, 24, 0},
    {109, 40, 24, 0},
    {93, 32, 0, 0},
    {76, 24, 0, 0},
    {60, 24, 0, 0},
    {32, 0, 0, 0},
    {0, 0, 0, 0},
    {105, 129, 105, 0},
    {97, 121, 97, 0},
    {93, 113, 93, 0},
    {80, 105, 80, 0},
    {76, 97, 76, 0},
    {68, 89, 68, 0},
    {60, 76, 60, 0},
    {56, 72, 56, 0},
    {210, 206, 198, 0},
    {194, 182, 170, 0},
    {182, 161, 145, 0},
    {165, 141, 117, 0},
    {149, 117, 93, 0},
    {129, 97, 68, 0},
    {109, 72, 40, 0},
    {93, 56, 24, 0},
    {194, 194, 0, 0},
    {182, 182, 0, 0},
    {170, 170, 0, 0},
    {157, 157, 0, 0},
    {141, 141, 0, 0},
    {129, 129, 0, 0},
    {113, 113, 0, 0},
    {255, 255, 255, 0},
};

// Fire Palette
static RGBColor m_pal_fire[] = {
    {255, 0, 255, 0},   // 0
    {0, 0, 0, 0},       // 1
    {2, 0, 0, 0},       // 2
    {4, 1, 0, 0},       // 3
    {6, 1, 0, 0},       // 4
    {9, 2, 0, 0},       // 5
    {11, 2, 0, 0},      // 6
    {13, 3, 0, 0},      // 7
    {16, 3, 0, 0},      // 8
    {18, 4, 0, 0},      // 9
    {20, 4, 0, 0},      // 10
    {22, 5, 0, 0},      // 11
    {25, 5, 0, 0},      // 12
    {27, 6, 0, 0},      // 13
    {29, 6, 0, 0},      // 14
    {32, 7, 0, 0},      // 15
    {34, 7, 0, 0},      // 16
    {36, 8, 0, 0},      // 17
    {39, 8, 0, 0},      // 18
    {41, 9, 0, 0},      // 19
    {43, 10, 0, 0},     // 20
    {45, 10, 0, 0},     // 21
    {48, 11, 0, 0},     // 22
    {50, 11, 0, 0},     // 23
    {52, 12, 0, 0},     // 24
    {55, 12, 0, 0},     // 25
    {57, 13, 0, 0},     // 26
    {59, 13, 0, 0},     // 27
    {62, 14, 0, 0},     // 28
    {64, 14, 0, 0},     // 29
    {66, 15, 0, 0},     // 30
    {68, 15, 0, 0},     // 31
    {71, 16, 0, 0},     // 32
    {73, 16, 0, 0},     // 33
    {75, 17, 0, 0},     // 34
    {78, 17, 0, 0},     // 35
    {80, 18, 0, 0},     // 36
    {82, 18, 0, 0},     // 37
    {85, 19, 0, 0},     // 38
    {87, 20, 0, 0},     // 39
    {89, 20, 0, 0},     // 40
    {91, 21, 0, 0},     // 41
    {94, 21, 0, 0},     // 42
    {96, 22, 0, 0},     // 43
    {98, 22, 0, 0},     // 44
    {101, 23, 0, 0},    // 45
    {103, 23, 0, 0},    // 46
    {105, 24, 0, 0},    // 47
    {108, 24, 0, 0},    // 48
    {110, 25, 0, 0},    // 49
    {112, 25, 0, 0},    // 50
    {114, 26, 0, 0},    // 51
    {117, 26, 0, 0},    // 52
    {119, 27, 0, 0},    // 53
    {121, 27, 0, 0},    // 54
    {124, 28, 0, 0},    // 55
    {126, 28, 0, 0},    // 56
    {128, 29, 0, 0},    // 57
    {131, 30, 0, 0},    // 58
    {133, 30, 0, 0},    // 59
    {135, 31, 0, 0},    // 60
    {137, 31, 0, 0},    // 61
    {140, 32, 0, 0},    // 62
    {142, 32, 0, 0},    // 63
    {144, 33, 0, 0},    // 64
    {147, 33, 0, 0},    // 65
    {149, 34, 0, 0},    // 66
    {151, 34, 0, 0},    // 67
    {154, 35, 0, 0},    // 68
    {156, 35, 0, 0},    // 69
    {158, 36, 0, 0},    // 70
    {160, 36, 0, 0},    // 71
    {163, 37, 0, 0},    // 72
    {165, 37, 0, 0},    // 73
    {167, 38, 0, 0},    // 74
    {170, 39, 0, 0},    // 75
    {172, 39, 0, 0},    // 76
    {174, 40, 0, 0},    // 77
    {177, 40, 0, 0},    // 78
    {179, 41, 0, 0},    // 79
    {181, 41, 0, 0},    // 80
    {183, 42, 0, 0},    // 81
    {186, 42, 0, 0},    // 82
    {188, 43, 0, 0},    // 83
    {190, 43, 0, 0},    // 84
    {193, 44, 0, 0},    // 85
    {195, 44, 0, 0},    // 86
    {197, 45, 0, 0},    // 87
    {200, 45, 0, 0},    // 88
    {202, 46, 0, 0},    // 89
    {204, 46, 0, 0},    // 90
    {206, 47, 0, 0},    // 91
    {209, 47, 0, 0},    // 92
    {211, 48, 0, 0},    // 93
    {213, 49, 0, 0},    // 94
    {216, 49, 0, 0},    // 95
    {218, 50, 0, 0},    // 96
    {220, 50, 0, 0},    // 97
    {223, 51, 0, 0},    // 98
    {225, 51, 0, 0},    // 99
    {227, 52, 0, 0},    // 100
    {229, 52, 0, 0},    // 101
    {232, 53, 0, 0},    // 102
    {234, 53, 0, 0},    // 103
    {236, 54, 0, 0},    // 104
    {239, 54, 0, 0},    // 105
    {241, 55, 0, 0},    // 106
    {243, 55, 0, 0},    // 107
    {246, 56, 0, 0},    // 108
    {248, 56, 0, 0},    // 109
    {250, 57, 0, 0},    // 110
    {252, 57, 0, 0},    // 111
    {252, 58, 0, 0},    // 112
    {252, 60, 0, 0},    // 113
    {252, 62, 0, 0},    // 114
    {252, 64, 0, 0},    // 115
    {252, 65, 0, 0},    // 116
    {252, 67, 0, 0},    // 117
    {252, 69, 0, 0},    // 118
    {252, 71, 0, 0},    // 119
    {252, 72, 0, 0},    // 120
    {252, 74, 0, 0},    // 121
    {252, 76, 0, 0},    // 122
    {252, 78, 0, 0},    // 123
    {252, 79, 0, 0},    // 124
    {252, 81, 0, 0},    // 125
    {252, 83, 0, 0},    // 126
    {252, 85, 0, 0},    // 127
    {252, 87, 0, 0},    // 128
    {252, 88, 0, 0},    // 129
    {252, 90, 0, 0},    // 130
    {252, 92, 0, 0},    // 131
    {252, 94, 0, 0},    // 132
    {252, 95, 0, 0},    // 133
    {252, 97, 0, 0},    // 134
    {252, 99, 0, 0},    // 135
    {252, 101, 0, 0},   // 136
    {252, 102, 0, 0},   // 137
    {252, 104, 0, 0},   // 138
    {252, 106, 0, 0},   // 139
    {252, 108, 0, 0},   // 140
    {252, 110, 0, 0},   // 141
    {252, 111, 0, 0},   // 142
    {252, 113, 0, 0},   // 143
    {252, 115, 0, 0},   // 144
    {252, 117, 0, 0},   // 145
    {252, 118, 0, 0},   // 146
    {252, 120, 0, 0},   // 147
    {252, 122, 0, 0},   // 148
    {253, 124, 0, 0},   // 149
    {253, 125, 0, 0},   // 150
    {253, 127, 0, 0},   // 151
    {253, 129, 0, 0},   // 152
    {253, 131, 0, 0},   // 153
    {253, 133, 0, 0},   // 154
    {253, 134, 0, 0},   // 155
    {253, 136, 0, 0},   // 156
    {253, 138, 0, 0},   // 157
    {253, 140, 0, 0},   // 158
    {253, 141, 0, 0},   // 159
    {253, 143, 0, 0},   // 160
    {253, 145, 0, 0},   // 161
    {253, 147, 0, 0},   // 162
    {253, 148, 0, 0},   // 163
    {253, 150, 0, 0},   // 164
    {253, 152, 0, 0},   // 165
    {253, 154, 0, 0},   // 166
    {253, 156, 0, 0},   // 167
    {253, 157, 0, 0},   // 168
    {253, 159, 0, 0},   // 169
    {253, 161, 0, 0},   // 170
    {253, 163, 0, 0},   // 171
    {253, 164, 0, 0},   // 172
    {253, 166, 0, 0},   // 173
    {253, 168, 0, 0},   // 174
    {253, 170, 0, 0},   // 175
    {253, 171, 0, 0},   // 176
    {253, 173, 0, 0},   // 177
    {253, 175, 0, 0},   // 178
    {253, 177, 0, 0},   // 179
    {253, 178, 0, 0},   // 180
    {253, 180, 0, 0},   // 181
    {253, 182, 0, 0},   // 182
    {253, 184, 0, 0},   // 183
    {253, 186, 0, 0},   // 184
    {253, 187, 0, 0},   // 185
    {254, 189, 0, 0},   // 186
    {254, 191, 0, 0},   // 187
    {254, 193, 0, 0},   // 188
    {254, 194, 0, 0},   // 189
    {254, 196, 0, 0},   // 190
    {254, 198, 0, 0},   // 191
    {254, 200, 0, 0},   // 192
    {254, 201, 0, 0},   // 193
    {254, 203, 0, 0},   // 194
    {254, 205, 0, 0},   // 195
    {254, 207, 0, 0},   // 196
    {254, 209, 0, 0},   // 197
    {254, 210, 0, 0},   // 198
    {254, 212, 0, 0},   // 199
    {254, 214, 0, 0},   // 200
    {254, 216, 0, 0},   // 201
    {254, 217, 0, 0},   // 202
    {254, 219, 0, 0},   // 203
    {254, 221, 0, 0},   // 204
    {254, 223, 0, 0},   // 205
    {254, 224, 0, 0},   // 206
    {254, 226, 0, 0},   // 207
    {254, 228, 0, 0},   // 208
    {254, 230, 0, 0},   // 209
    {254, 232, 0, 0},   // 210
    {254, 233, 0, 0},   // 211
    {254, 235, 0, 0},   // 212
    {254, 237, 0, 0},   // 213
    {254, 239, 0, 0},   // 214
    {254, 240, 0, 0},   // 215
    {254, 242, 0, 0},   // 216
    {254, 244, 0, 0},   // 217
    {254, 246, 0, 0},   // 218
    {254, 247, 0, 0},   // 219
    {254, 249, 0, 0},   // 220
    {254, 251, 0, 0},   // 221
    {254, 253, 0, 0},   // 222
    {255, 255, 0, 0},   // 223
    {255, 255, 7, 0},   // 224
    {255, 255, 15, 0},  // 225
    {255, 255, 23, 0},  // 226
    {255, 255, 31, 0},  // 227
    {255, 255, 39, 0},  // 228
    {255, 255, 47, 0},  // 229
    {255, 255, 55, 0},  // 230
    {255, 255, 63, 0},  // 231
    {255, 255, 71, 0},  // 232
    {255, 255, 79, 0},  // 233
    {255, 255, 87, 0},  // 234
    {255, 255, 95, 0},  // 235
    {255, 255, 103, 0}, // 236
    {255, 255, 111, 0}, // 237
    {255, 255, 119, 0}, // 238
    {255, 255, 127, 0}, // 239
    {255, 255, 135, 0}, // 240
    {255, 255, 143, 0}, // 241
    {255, 255, 151, 0}, // 242
    {255, 255, 159, 0}, // 243
    {255, 255, 167, 0}, // 244
    {255, 255, 175, 0}, // 245
    {255, 255, 183, 0}, // 246
    {255, 255, 191, 0}, // 247
    {255, 255, 199, 0}, // 248
    {255, 255, 207, 0}, // 249
    {255, 255, 215, 0}, // 250
    {255, 255, 223, 0}, // 251
    {255, 255, 231, 0}, // 252
    {255, 255, 239, 0}, // 253
    {255, 255, 247, 0}, // 254
    {255, 255, 255, 0}, // 255
};

// VGA Palette
static RGBColor m_pal_vga[] = {
    {0, 0, 0, 0},
    {0, 0, 170, 0},
    {0, 170, 0, 0},
    {0, 170, 170, 0},
    {170, 0, 0, 0},
    {170, 0, 170, 0},
    {170, 85, 0, 0},
    {170, 170, 170, 0},
    {85, 85, 85, 0},
    {85, 85, 255, 0},
    {85, 255, 85, 0},
    {85, 255, 255, 0},
    {255, 85, 85, 0},
    {255, 85, 255, 0},
    {255, 255, 85, 0},
    {255, 255, 255, 0},
    {0, 0, 0, 0},
    {16, 16, 16, 0},
    {32, 32, 32, 0},
    {53, 53, 53, 0},
    {69, 69, 69, 0},
    {85, 85, 85, 0},
    {101, 101, 101, 0},
    {117, 117, 117, 0},
    {138, 138, 138, 0},
    {154, 154, 154, 0},
    {170, 170, 170, 0},
    {186, 186, 186, 0},
    {202, 202, 202, 0},
    {223, 223, 223, 0},
    {239, 239, 239, 0},
    {255, 255, 255, 0},
    {0, 0, 255, 0},
    {65, 0, 255, 0},
    {130, 0, 255, 0},
    {190, 0, 255, 0},
    {255, 0, 255, 0},
    {255, 0, 190, 0},
    {255, 0, 130, 0},
    {255, 0, 65, 0},
    {255, 0, 0, 0},
    {255, 65, 0, 0},
    {255, 130, 0, 0},
    {250, 190, 0, 0},
    {255, 255, 0, 0},
    {190, 255, 0, 0},
    {130, 255, 0, 0},
    {65, 255, 0, 0},
    {0, 255, 0, 0},
    {0, 255, 65, 0},
    {0, 255, 130, 0},
    {0, 255, 190, 0},
    {0, 255, 255, 0},
    {0, 190, 255, 0},
    {0, 130, 255, 0},
    {0, 65, 255, 0},
    {130, 130, 255, 0},
    {158, 130, 255, 0},
    {190, 130, 255, 0},
    {223, 130, 255, 0},
    {255, 130, 255, 0},
    {255, 130, 223, 0},
    {255, 130, 190, 0},
    {255, 130, 158, 0},
    {255, 130, 130, 0},
    {255, 158, 130, 0},
    {255, 190, 130, 0},
    {255, 223, 130, 0},
    {255, 255, 130, 0},
    {223, 255, 130, 0},
    {190, 255, 130, 0},
    {158, 255, 130, 0},
    {130, 255, 130, 0},
    {130, 255, 158, 0},
    {130, 255, 190, 0},
    {130, 255, 223, 0},
    {130, 255, 255, 0},
    {130, 223, 255, 0},
    {130, 190, 255, 0},
    {130, 158, 255, 0},
    {186, 186, 255, 0},
    {202, 186, 255, 0},
    {223, 186, 255, 0},
    {239, 186, 255, 0},
    {255, 186, 255, 0},
    {255, 186, 239, 0},
    {255, 186, 223, 0},
    {255, 186, 202, 0},
    {255, 186, 186, 0},
    {255, 202, 186, 0},
    {255, 186, 255, 0},
    {255, 239, 255, 0},
    {255, 255, 186, 0},
    {239, 255, 186, 0},
    {223, 255, 186, 0},
    {202, 255, 186, 0},
    {186, 255, 186, 0},
    {186, 255, 202, 0},
    {186, 255, 223, 0},
    {186, 255, 239, 0},
    {186, 255, 255, 0},
    {186, 239, 255, 0},
    {186, 223, 255, 0},
    {186, 202, 255, 0},
    {0, 0, 113, 0},
    {28, 0, 113, 0},
    {57, 0, 113, 0},
    {85, 0, 113, 0},
    {113, 0, 113, 0},
    {113, 0, 85, 0},
    {113, 0, 57, 0},
    {113, 0, 28, 0},
    {113, 0, 0, 0},
    {113, 28, 0, 0},
    {113, 57, 0, 0},
    {113, 85, 0, 0},
    {113, 113, 0, 0},
    {85, 113, 0, 0},
    {57, 113, 0, 0},
    {28, 113, 0, 0},
    {0, 113, 0, 0},
    {0, 113, 28, 0},
    {0, 113, 57, 0},
    {0, 113, 85, 0},
    {0, 113, 113, 0},
    {0, 85, 113, 0},
    {0, 57, 113, 0},
    {0, 28, 113, 0},
    {57, 57, 113, 0},
    {69, 57, 113, 0},
    {85, 57, 113, 0},
    {97, 57, 113, 0},
    {113, 57, 113, 0},
    {113, 57, 97, 0},
    {113, 57, 85, 0},
    {113, 57, 69, 0},
    {113, 57, 57, 0},
    {113, 69, 57, 0},
    {113, 85, 57, 0},
    {113, 97, 57, 0},
    {113, 113, 57, 0},
    {97, 113, 57, 0},
    {85, 113, 57, 0},
    {69, 113, 57, 0},
    {57, 113, 57, 0},
    {57, 113, 69, 0},
    {57, 113, 85, 0},
    {57, 113, 97, 0},
    {57, 113, 113, 0},
    {57, 97, 113, 0},
    {57, 85, 113, 0},
    {57, 69, 113, 0},
    {81, 81, 113, 0},
    {89, 81, 113, 0},
    {97, 81, 113, 0},
    {105, 81, 113, 0},
    {113, 81, 113, 0},
    {113, 81, 105, 0},
    {113, 87, 97, 0},
    {113, 81, 89, 0},
    {113, 81, 81, 0},
    {113, 89, 81, 0},
    {113, 97, 81, 0},
    {113, 105, 81, 0},
    {113, 113, 81, 0},
    {105, 113, 81, 0},
    {97, 113, 81, 0},
    {89, 113, 81, 0},
    {81, 113, 81, 0},
    {81, 113, 89, 0},
    {81, 113, 97, 0},
    {81, 113, 105, 0},
    {81, 113, 113, 0},
    {81, 105, 113, 0},
    {81, 97, 113, 0},
    {81, 89, 113, 0},
    {0, 0, 65, 0},
    {16, 0, 65, 0},
    {32, 0, 65, 0},
    {49, 0, 65, 0},
    {65, 0, 65, 0},
    {65, 0, 49, 0},
    {65, 0, 32, 0},
    {65, 0, 16, 0},
    {65, 0, 0, 0},
    {65, 16, 0, 0},
    {65, 32, 0, 0},
    {65, 49, 0, 0},
    {65, 65, 0, 0},
    {49, 65, 0, 0},
    {32, 65, 0, 0},
    {16, 65, 0, 0},
    {0, 65, 0, 0},
    {0, 65, 16, 0},
    {0, 65, 32, 0},
    {0, 65, 49, 0},
    {0, 65, 65, 0},
    {0, 49, 65, 0},
    {0, 32, 65, 0},
    {0, 16, 65, 0},
    {32, 32, 65, 0},
    {40, 32, 65, 0},
    {49, 32, 65, 0},
    {57, 32, 65, 0},
    {65, 32, 65, 0},
    {65, 32, 57, 0},
    {65, 32, 49, 0},
    {65, 32, 40, 0},
    {65, 32, 32, 0},
    {65, 40, 32, 0},
    {65, 49, 32, 0},
    {65, 57, 32, 0},
    {65, 65, 32, 0},
    {57, 65, 32, 0},
    {49, 65, 32, 0},
    {40, 65, 32, 0},
    {32, 65, 32, 0},
    {32, 65, 40, 0},
    {32, 65, 49, 0},
    {32, 65, 57, 0},
    {32, 65, 65, 0},
    {32, 57, 65, 0},
    {32, 49, 65, 0},
    {32, 40, 65, 0},
    {45, 45, 65, 0},
    {49, 45, 65, 0},
    {53, 45, 65, 0},
    {61, 45, 65, 0},
    {65, 45, 65, 0},
    {65, 45, 61, 0},
    {65, 45, 53, 0},
    {65, 45, 49, 0},
    {65, 45, 45, 0},
    {65, 49, 45, 0},
    {65, 53, 45, 0},
    {65, 61, 45, 0},
    {65, 65, 45, 0},
    {61, 65, 45, 0},
    {53, 65, 45, 0},
    {49, 65, 45, 0},
    {45, 65, 45, 0},
    {45, 65, 49, 0},
    {45, 65, 53, 0},
    {45, 65, 61, 0},
    {45, 65, 65, 0},
    {45, 61, 65, 0},
    {45, 53, 65, 0},
    {45, 49, 65, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};


/////////////////////////////////////////////////////////////////////////////
// Copy a Palette Range
/////////////////////////////////////////////////////////////////////////////

void CopyPalette(PRGBCOLOR pDst, PRGBCOLOR pSrc, int iColors)
{
    CheckPtr(pDst, "CopyPalette (dst)");
    CheckPtr(pSrc, "CopyPalette (src)");

    if (iColors < 1)
        return;

    if (pSrc != pDst)
    {

        CopyBuffer(pSrc, pDst, iColors * sizeof(RGBColor));
    }
}

/////////////////////////////////////////////////////////////////////////////
// Fill a Palette Range
/////////////////////////////////////////////////////////////////////////////

void FillPalette(PRGBCOLOR pDst, RGBColor clr, int iColors)
{
    CheckPtr(pDst, "FillPalette (dst)");

    for (int i = 0; i < iColors; i++)
        pDst[i] = clr;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize 332 Palette
/////////////////////////////////////////////////////////////////////////////

void InitPalette332(PRGBCOLOR pal)
{
    CheckPtr(pal, "InitPalette332 (pal)");

    for (int c = 0; c < 254; c++)
    {

        pal[c].r = ((c >> 5) & 7) * 255 / 7;
        pal[c].g = ((c >> 2) & 7) * 255 / 7;
        pal[c].b = (c & 3) * 255 / 3;
    }

    // Set black and white colors
    pal[254].r = pal[254].g = pal[254].b = 0;
    pal[255].r = pal[255].g = pal[255].b = 0xFF;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize Jedi Palette
/////////////////////////////////////////////////////////////////////////////

void InitPaletteJedi(PRGBCOLOR pal)
{
    CheckPtr(pal, "InitPaletteJedi (pal)");

    CopyPalette(pal, m_pal_jedi, 256);
}

/////////////////////////////////////////////////////////////////////////////
// Initialize Fire Palette
/////////////////////////////////////////////////////////////////////////////

void InitPaletteFire(PRGBCOLOR pal)
{
    CheckPtr(pal, "InitPaletteFire (pal)");

    CopyPalette(pal, m_pal_fire, 256);
}

/////////////////////////////////////////////////////////////////////////////
// Initialize VGA Palette
/////////////////////////////////////////////////////////////////////////////

void InitPaletteVGA(PRGBCOLOR pal)
{
    CheckPtr(pal, "InitPaletteJedi (pal)");

    CopyPalette(pal, m_pal_vga, 256);
}

/////////////////////////////////////////////////////////////////////////////
// Initialize Monotone Palette
/////////////////////////////////////////////////////////////////////////////

static void gamma_correct(RGBColor src, PRGBCOLOR dst, float gamma)
{
    float range = 255.0f;
    gamma = 1 / gamma;
    dst->r = pow(src.r / range, gamma);
    dst->g = pow(src.g / range, gamma);
    dst->b = pow(src.b / range, gamma);
}

void InitPaletteMono(PRGBCOLOR pal, RGBColor color, float gamma)
{
    CheckPtr(pal, "InitPaletteMono (pal)");
    // Create linear mono palette
    pal[0].r = 0;
    pal[0].g = 0;
    pal[0].b = 0;
    pal[255] = color;
    RGBPalette tmp;
    LerpPaletteRange(pal[0], 0, color, 255, &tmp);
    // Do gamma correction
    if (gamma != 1)
    {
        for (int i = 0; i < 256; i++)
        {

            gamma_correct(tmp.color[i], &(pal[i]), gamma);
        }
    }
    else
    {
        CopyBuffer(tmp.color, pal, 256 * sizeof(RGBColor));
    }
}


/////////////////////////////////////////////////////////////////////////////
// Hashing Function
/////////////////////////////////////////////////////////////////////////////

static inline int hash(int data)
{
    return (data % HASHTABLESIZE);
}

/////////////////////////////////////////////////////////////////////////////
// Sort Linked List in Descending Order
/////////////////////////////////////////////////////////////////////////////

static PAL_NODE *sort_list(PAL_NODE *list)
{

    PAL_NODE *newlist;
    PAL_NODE *tmp;
    PAL_NODE *tmp0;
    PAL_NODE *listbak;

    // Allocate new list
    newlist = (PAL_NODE *)calloc(1, sizeof(PAL_NODE));

    // Backup of list
    listbak = list;

    // Pointer to first element
    list = list->next;

    // Walk the list
    while (list)
    {

        // Get pointer to head of new list
        tmp = newlist;

        // Walk new list
        while (tmp->next)
        {

            // Break at first lower node
            if (tmp->next->count < list->count)
                break;

            // Next node in new list
            tmp = tmp->next;
        }

        // Allocate new node & copy from old list
        tmp0 = (PAL_NODE *)calloc(1, sizeof(PAL_NODE));
        tmp0->count = list->count;
        tmp0->data = list->data;
        tmp0->next = tmp->next;
        tmp->next = tmp0;

        // Next node in old list
        list = list->next;
    }

    // Destroy old list
    while (listbak)
    {

        tmp = listbak;
        listbak = listbak->next;
        free(tmp);
    }

    // Return new list
    return newlist;
}

/////////////////////////////////////////////////////////////////////////////
// Insert a New Node in a Linked List
/////////////////////////////////////////////////////////////////////////////

static PAL_NODE *insert_node(int data)
{

    PAL_NODE *p;
    PAL_NODE *p0;
    int bucket;
    int i = 0;

    // Determine which bucket to use
    bucket = hash(data);

    // Get the linked list for this bucket
    p = m_hash_table[bucket];

    // Walk the linked list
    while (p->next)
    {

        // If node already exists, stop here
        if (p->next->data == data)
        {

            i = 1;
            break;
        }

        // Move to next node
        p = p->next;
    }

    // Node with the same data was found
    if (i)
    {

        // Save ptr to the node
        p0 = p->next;

        // Bump the node's usage count
        p0->count++;
    }

    // Need to create new node
    else
    {

        // Allocate new node
        p0 = (PAL_NODE *)calloc(1, sizeof(PAL_NODE));

        // Initialize new node
        p0->data = data;
        p0->count = 1;
        p0->next = NULL;

        // Link new node into list
        p->next = p0;
    }

    // Return pointer to affected node
    return p0;
}

/////////////////////////////////////////////////////////////////////////////
// Delete Specified Node
/////////////////////////////////////////////////////////////////////////////

static void delete_node(int data)
{

    PAL_NODE *p0;
    PAL_NODE *p;
    int bucket;

    // Indicate node not found yet
    p0 = NULL;

    // Get bucket containing node
    bucket = hash(data);

    // Get head of bucket's linked list
    p = m_hash_table[bucket];

    // Try to locate node in linked list
    while ((p) && (p->data != data))
    {

        p0 = p;
        p = p->next;
    }

    // Oops!  Node not found.
    if (!p)
        return;

    // Node p was not the head node
    if (p0)
    {

        // Usage count > 1, bump count but keep node
        if (p->count > 1)
            p->count--;

        // Usage count zero, remove node
        else
        {

            p0->next = p->next;
            free(p);
        }
    }

    // Node p was the head node
    else
    {

        // Usage count > 1, bump count but keep node
        if (p->count > 1)
            p->count--;

        // Usage count zero, remove node
        else
        {

            m_hash_table[bucket] = p->next;
            free(p);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// Locate a Node
/////////////////////////////////////////////////////////////////////////////

static PAL_NODE *find_node(int data)
{

    int bucket;
    PAL_NODE *p;

    // Locate the bucket containing the node
    bucket = hash(data);

    // Get the linked list for the bucket
    p = m_hash_table[bucket];

    // Search the linked list for the node
    while ((p) && (p->data != data))
        p = p->next;

    // Return node pointer (NULL for not found)
    return p;
}

/////////////////////////////////////////////////////////////////////////////
// Delete All Nodes in a Linked List
/////////////////////////////////////////////////////////////////////////////

static void delete_list(PAL_NODE *list)
{

    PAL_NODE *tmp;

    while (list)
    {

        tmp = list;
        list = list->next;
        free(tmp);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Compare Two Colors
/////////////////////////////////////////////////////////////////////////////

static int compare_cols(int num1, int num2)
{
    return (abs(((num1 >> 16) & 0x0F) - ((num2 >> 16) & 0x0F)) 
         + abs(((num1 >> 8) & 0x0F) - ((num2 >> 8) & 0x0F)) 
         + abs((num1 & 0x0F) - (num2 & 0x0F)));
}


/////////////////////////////////////////////////////////////////////////////
// Initialize Optimal Palette
/////////////////////////////////////////////////////////////////////////////
//
// Calculates a suitable palette for color reducing the specified truecolor
// image. If the rsvdcols parameter is not NULL, it contains an array of
// 256 flags specifying which colors the palette is allowed to use.
//
/////////////////////////////////////////////////////////////////////////////

static int get_red(int color)
{
    return (color >> 16) & 0xFF;
}

static int get_grn(int color)
{
    return (color >> 8) & 0xFF;
}

static int get_blu(int color)
{
    return (color) & 0xFF; 
}

int InitPaletteOptimal(PRGBCOLOR pPal, LPIMAGE img, char *rsvd)
{
    CheckImage(img);
    CheckPtr(pPal, "InitPaletteOptimal (pal)");

    PAL_NODE *help_table[HASHTABLESIZE];

    int *common, *common2;
    int imgsize, i, max;
    int x, y;
    int j, tmp, big2 = 0, big, best = 0, best2 = 0, rsvdcnt = 0, start;

    char tmprsvd[256];

    if (!rsvd)
    {
        for (i = 0; i < 256; i++)
            tmprsvd[i] = FALSE;

        rsvd = tmprsvd;
    }

    m_hash_table = (PAL_NODE **)calloc(HASHTABLESIZE, sizeof(PAL_NODE *));

    for (i = 0; i < HASHTABLESIZE; i++)
        m_hash_table[i] = (PAL_NODE *)calloc(1, sizeof(PAL_NODE));

    imgsize = img->h * img->w;

    for (y = 0; y < img->h; y++)
    {

        for (x = 0; x < img->w; x++)
        {

            tmp = *(int*)(&(img->line[y][x]));
            insert_node((tmp >> 4) & BITMASK32);
            // insert_node(tmp & BITMASK32);
        }
    }

    // Sort m_hash_table's lists with the biggest count first
    for (i = 0; i < HASHTABLESIZE; i++)
        m_hash_table[i] = sort_list(m_hash_table[i]);

    // Merge the most common values to 'common' table. Note that to search
    // for the biggest 'count' it's enough to check first entry in each list
    // in m_hash_table because the lists are already sorted by the 'count' value.
    // We'll fill in first 512 values because if we fill directly palette here
    // it could miss some rare but very different color (I had it this way, but
    // it gave poor results for some images).

    // Several palette entries can be saved by filling the r,g,b values in
    // advance and in this routine skipping over them.

    for (i = 0; i < HASHTABLESIZE; i++)
    {
        help_table[i] = m_hash_table[i]->next;
    }

    // First items are helpers with no values so we can skip over them
    for (i = 0; i < 256; i++)
    {
        if (rsvd[i])
            rsvdcnt++;
    }

    common = (int *)calloc((512 + rsvdcnt), sizeof(int));
    common2 = (int *)calloc((512 + rsvdcnt), sizeof(int));

    // Store the reserved entries in common to be able to count with them
    for (i = 0, j = 0; i < 256; i++)
    {
        if (rsvd[i])
        {
            RGBColor color = pPal[i];
            ARGB argb;
            argb.r = color.r;
            argb.g = color.g;
            argb.b = color.b;
            argb.a = 0;
            tmp = *(int*)(&argb);
            common[j] = (tmp >> 4) & BITMASK32;
            // common[j] = tmp & BITMASK32;
            j++;
        }
    }

    for (i = rsvdcnt, max = rsvdcnt + 512; i < max; i++)
    {

        int curmax = 0, big = 0, j;

        common[i] = 0;

        for (j = 0; j < HASHTABLESIZE; j++)
        {
            if ((help_table[j]) && (help_table[j]->count > curmax))
            {
                curmax = help_table[j]->count;
                big = j;
            }
        }

        if (!help_table[big])
        {
            max = i;
            break;
        }

        common[i] = help_table[big]->data;
        help_table[big] = help_table[big]->next;
    }

    // Compare each value we'll possibly use to replace with all already used
    // colors (up to START) to find the most different ones. Lot of colors are
    // just so much similar (difference smaller than some small value (here 3)
    // than it's useless to continue comparing, it speeds the process a lot too.

    start = 256 - (256 - rsvdcnt) / 10;
    best = max;

    for (i = 0; i < rsvdcnt; i++)
    {
        for (j = rsvdcnt; j < rsvdcnt + 100; j++)
        { // +100 should be enough

            if ((tmp = compare_cols(common[i], common[j])) == 0)
            {

                memcpy(&common[j], &common[j + 1], (max - j) * 4);
                j--;
                max--;
            }
        }
    }

    if ((rsvdcnt > 150) && (best > max + 50))
        start = rsvdcnt;

    for (i = start; i < max; i++)
    {

        for (j = 0, big = 10000; j < start; j++)
        {

            tmp = compare_cols(common[i], common[j]);

            if (tmp < big)
            {

                big = tmp;
                best = i;

                if (tmp < 3)
                    break;
            }
        }

        // Place the smallest difference in there
        common2[i] = big;

        // Find the biggest difference color to be used for color START
        // (save one run in the next loop).

        if (big > big2)
        {

            big2 = big;
            best2 = best;
        }
    }

    common[start] = common[best2];

    // Replace other 'common' >START && <256 with very different colors
    for (i = start + 1; i < 256; i++)
    {

        // Look if the last added value (i-1) doesn't change the differences
        // (e.g. image is full of green shadows and several red pixels. If we
        // are happy (and in this case we must be) to 4 bit per component we use
        // fiter them to lot less so in first 512 'common' is also the red
        // color.  But we already placed the red color to 'common' table so to
        // get there another shade of red is now of lot less importance.

        for (j = i + 1; j < max; j++)
        {

            tmp = compare_cols(common[j], common[i]);

            if (tmp < common2[j])
                common2[j] = tmp;
        }

        // Find the biggest 'common[1]' which is the color the most different
        // from these already used.

        for (j = i + 1, big = 0; j < max; j++)
        {

            if (common2[j] > big)
            {

                big = common2[j];
                best = j;
            }
        }

        // Place it into 'common' and set it's difference to 0 to prevent
        // it from being included more times.

        common[i] = common[best];
        common2[best] = 0;
    }

    // Max is at most 512 but could be lot less (e.g. image is all white). If
    // the image uses more than 256 colors we must use only the 256 (the palette
    // size). We then remap our reduced color values back to 8 bits per RGB
    // component (the Windows DAC resolution).

    if (max > 256)
        max = 256;
    
    for (i = 0, j = rsvdcnt; i < max; i++)
    {
        if (!rsvd[i])
        {

            tmp = (common[j] << 4);
            pPal[i].r = get_red(tmp);
            pPal[i].g = get_grn(tmp);
            pPal[i].b = get_blu(tmp);
            j++;
        }
    }

    // Free all dynamically allocated memory
    for (i = 0; i < HASHTABLESIZE; i++)
        delete_list(m_hash_table[i]);

    free(m_hash_table);
    m_hash_table = NULL;

    free(common);
    free(common2);

    return 0;
}
