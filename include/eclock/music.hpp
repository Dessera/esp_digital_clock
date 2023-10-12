#pragma once

// 堆音乐的史山，以后会改的

#define LOW1 262
#define LOW1J 277
#define LOW2 294
#define LOW2J 311
#define LOW3 330
#define LOW4 349
#define LOW4J 370
#define LOW5 392
#define LOW5J 415
#define LOW6 440
#define LOW6J 466
#define LOW7 494

#define MID1 523
#define MID1J 554
#define MID2 587
#define MID2J 622
#define MID3 659
#define MID4 698
#define MID4J 740
#define MID5 784
#define MID5J 831
#define MID6 880
#define MID6J 932
#define MID7 988

#define HIGH1 1046
#define HIGH1J 1109
#define HIGH2 1175
#define HIGH2J 1245
#define HIGH3 1318
#define HIGH4 1397
#define HIGH4J 1480
#define HIGH5 1568
#define HIGH5J 1661
#define HIGH6 1760
#define HIGH6J 1865
#define HIGH7 1976

// make definition above as 2 arrays (j and not j)
// not j and low (replace with number)
constexpr int low[7] = {LOW1, LOW2, LOW3, LOW4, LOW5, LOW6, LOW7};
// j and low (replace low3j with 0)
constexpr int lowj[7] = {LOW1J, LOW2J, 0, LOW4J, LOW5J, LOW6J, LOW7};
// not j and mid (replace with number)
constexpr int mid[7] = {MID1, MID2, MID3, MID4, MID5, MID6, MID7};
// j and mid (replace mid3j with 0)
constexpr int midj[7] = {MID1J, MID2J, 0, MID4J, MID5J, MID6J, MID7};
// not j and high (replace with number)
constexpr int high[7] = {HIGH1, HIGH2, HIGH3, HIGH4, HIGH5, HIGH6, HIGH7};
// j and high (replace high3j with 0)
constexpr int highj[7] = {HIGH1J, HIGH2J, 0, HIGH4J, HIGH5J, HIGH6J, HIGH7};

constexpr int music[500] = {
    6,  0,   11,  0,   120, 13,  0,   50,  0,   6,  11,  12,  11,  120, 12,
    13, 0,   15,  140, 14,  120, 12,  11,  7,   60, 7,   11,  12,  120, 13,
    0,  3,   0,   6,   0,   11,  0,   120, 13,  0,  50,  0,   6,   11,  12,
    11, 120, 12,  13,  0,   15,  140, 14,  120, 13, 14,  140, 15,  150, 15,
    16, 16,  17,  0,   6,   0,   6,   0,   6,   0,  6,   0,   1,   0,   1,
    0,  1,   0,   1,   0,   4,   0,   4,   0,   4,  0,   4,   0,   3,   0,
    3,  0,   3,   0,   3,   0,   6,   0,   6,   0,  6,   0,   6,   0,   1,
    0,  1,   0,   1,   0,   1,   0,   4,   0,   4,  0,   4,   0,   4,   0,
    3,  0,   3,   0,   3,   0,   3,   0,   16,  0,  21,  0,   220, 23,  0,
    15, 16,  21,  22,  21,  220, 22,  23,  0,   25, 240, 24,  220, 22,  21,
    17, 160, 17,  21,  22,  220, 23,  0,   13,  0,  16,  0,   21,  0,   220,
    23, 0,   15,  16,  21,  22,  21,  220, 22,  23, 0,   25,  240, 24,  220,
    23, 24,  240, 25,  250, 25,  26,  26,  27,  0,  130, 140, 6,   1,   3,
    1,  6,   1,   3,   3,   1,   3,   5,   3,   1,  3,   5,   3,   4,   6,
    4,  6,   4,   6,   4,   6,   3,   7,   5,   7,  5,   0,   3,   0,   6,
    1,  3,   1,   6,   1,   3,   3,   1,   3,   5,  3,   1,   3,   5,   3,
    4,  6,   4,   6,   4,   6,   4,   6,   3,   7,  5,   7,   5,   0,   3,
    0,  3,   7,   50,  7,   3,   7,   5,   7,   3,  7,   50,  7,   3,   7,
    5,  7,   100};

int select_music(int i);