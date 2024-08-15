#include "include/hashkey.h"
#include "include/piece.h"
#include "lib/logc/log.h"
#include <stdint.h>
#include <stdlib.h>

static const uint8_t SQ64TO120[64] = {
  21, 22, 23, 24, 25, 26, 27, 28, 31, 32, 33, 34, 35, 36, 37, 38,
  41, 42, 43, 44, 45, 46, 47, 48, 51, 52, 53, 54, 55, 56, 57, 58,
  61, 62, 63, 64, 65, 66, 67, 68, 71, 72, 73, 74, 75, 76, 77, 78,
  81, 82, 83, 84, 85, 86, 87, 88, 91, 92, 93, 94, 95, 96, 97, 98,
};

static const uint16_t HASH_BLACK_TO_MOVE = 11421;
static const uint64_t HASH_WKCA          = 24534;
static const uint64_t HASH_WQCA          = 22088;
static const uint64_t HASH_BKCA          = 22615;
static const uint64_t HASH_BQCA          = 8784;
static const uint64_t HASHTABLE[12][64]  = {
  {3447,  238,   7276,  8227,  31107, 28420, 9950,  28835, 19167, 23624, 4057,
    18767, 1138,  27886, 2837,  8819,  25521, 6669,  23971, 4052,  2741,  16203,
    10311, 20236, 9123,  4806,  21639, 3954,  9332,  16199, 19938, 7073,  21264,
    12598, 25254, 18406, 5557,  18006, 24382, 28865, 17100, 28590, 21476, 7565,
    30615, 651,   22272, 21993, 14841, 22582, 11994, 6678,  1193,  32310, 17317,
    26573, 15211, 26312, 26426, 23121, 31095, 5950,  8271,  272},
  {11836, 8916,  31024, 11048, 26771, 15236, 15583, 15559, 11070, 27870, 1558,
    26778, 20539, 30638, 29370, 27971, 21636, 24348, 18616, 18034, 32515, 26536,
    24551, 7622,  29431, 14371, 28995, 1548,  32381, 28051, 22422, 12049, 10602,
    23852, 25372, 21150, 15670, 10619, 22628, 30729, 28709, 7790,  19343, 12717,
    1001,  2549,  26181, 14506, 10227, 1021,  6273,  17544, 24964, 1289,  8484,
    22626, 25638, 3437,  27576, 7886,  11976, 26205, 7153,  27521},
  {30397, 3414,  21177, 5619,  3894,  1425,  14616, 6343,  11104, 14622, 31583,
    25378, 4450,  16690, 31807, 1894,  17234, 30053, 23321, 18295, 25572, 24793,
    10717, 28291, 6327,  20827, 7197,  19289, 5877,  16302, 6934,  20835, 27099,
    28586, 15880, 17247, 22249, 805,   9868,  32294, 15042, 31123, 6349,  21842,
    32093, 5997,  26977, 24555, 3029,  20638, 18301, 26001, 2039,  10716, 5902,
    30193, 6999,  10800, 31803, 19221, 11094, 660,   10232, 7245},
  {9526,  13124, 21237, 30691, 23548, 7794,  24356, 18858, 25067, 16298, 7937,
    18481, 26085, 31160, 8389,  1194,  18548, 30342, 30007, 32653, 12356, 18493,
    31212, 31834, 4639,  15020, 4387,  18291, 25139, 12314, 5790,  28022, 25476,
    26801, 24925, 18036, 22076, 10514, 25580, 17810, 10367, 4205,  23934, 7756,
    12869, 28577, 13337, 1197,  21108, 21630, 1571,  14385, 6097,  19058, 19990,
    13289, 1078,  3857,  30598, 30914, 4247,  32287, 24143, 14236},
  {29913, 24648, 709,   10827, 1536,  30494, 21512, 22703, 218,   3415,  16993,
    4970,  10172, 24267, 24306, 8815,  6257,  17330, 28236, 3899,  19954, 29439,
    10153, 15002, 19787, 24558, 32108, 20082, 30194, 6164,  29316, 6401,  1222,
    22375, 6609,  8137,  7769,  14405, 2073,  18978, 16124, 31671, 26200, 15165,
    22556, 2721,  8878,  16894, 11841, 27096, 30094, 7940,  32162, 12625, 1915,
    18554, 16967, 29404, 29240, 8647,  16521, 16754, 12515, 22702},
  {1424,  14487, 30019, 29383, 4144,  1911,  28252, 30775, 24214, 20766, 9407,
    22337, 17044, 19331, 32736, 11838, 32511, 8746,  1262,  20888, 14099, 16902,
    11,    15676, 15572, 9372,  28162, 20412, 10943, 10672, 18429, 15051, 16308,
    21305, 21846, 12069, 7720,  24570, 17818, 32208, 31818, 17877, 5179,  24112,
    13322, 8149,  10456, 12027, 17105, 10586, 4443,  21455, 4243,  17759, 9133,
    27823, 31735, 15019, 29165, 22763, 6528,  415,   9127,  24972},
  {24573, 1326,  6002,  13838, 32205, 1703,  1240,  18656, 11580, 27128, 15599,
    31534, 7004,  12153, 14597, 18803, 31745, 22907, 19115, 26558, 25106, 18509,
    9680,  18352, 4273,  21326, 11406, 27350, 28007, 7697,  26073, 24223, 30523,
    7826,  14287, 1888,  7102,  5334,  29284, 18998, 24212, 15224, 1446,  27207,
    19272, 11729, 20868, 31574, 8360,  8112,  29860, 9846,  32632, 18771, 23742,
    8375,  32483, 16058, 6973,  15960, 11289, 24502, 5624,  960},
  {17073, 5743,  3721,  4426,  26071, 16929, 12933, 15272, 12460, 8189,  21144,
    12575, 21829, 28670, 27430, 16192, 16869, 30646, 8134,  26244, 2223,  149,
    16098, 3012,  8307,  7602,  22421, 12396, 5212,  12175, 12555, 21309, 4735,
    21616, 18672, 14726, 4216,  25153, 24166, 3979,  21470, 22965, 13213, 23142,
    18523, 13608, 8253,  19675, 21328, 23512, 16356, 26983, 14901, 17462, 17863,
    16508, 9401,  3143,  2725,  32697, 30550, 32485, 24995, 12494},
  {11521, 11798, 10798, 28779, 19590, 7747,  28380, 17311, 5533,  16750, 31750,
    25909, 6887,  15928, 8416,  22633, 4796,  955,   1497,  6774,  13107, 22624,
    2273,  1939,  26708, 3046,  17287, 18204, 14247, 10865, 1793,  31058, 15746,
    17313, 23990, 6761,  21626, 29483, 2706,  9383,  10202, 25527, 30207, 8051,
    8030,  26016, 11799, 28088, 27656, 16959, 17947, 29031, 30992, 15200, 5320,
    21831, 18755, 8858,  28058, 287,   25549, 20071, 7495,  9102},
  {16841, 23937, 10927, 22801, 11438, 28859, 20068, 18962, 12674, 8501,  31956,
    10991, 20995, 15690, 7258,  6932,  6135,  15869, 28469, 1627,  10427, 25618,
    5399,  2089,  22683, 16871, 20151, 17802, 12695, 29171, 7516,  1751,  18098,
    14998, 3290,  30683, 4925,  14816, 18455, 5473,  12143, 7054,  21224, 14793,
    4192,  28332, 32019, 25371, 13788, 3224,  12823, 22423, 4951,  19730, 9766,
    16637, 248,   1304,  16958, 10005, 32568, 15958, 13041, 1282},
  {6281,  420,   15491, 3344,  26980, 21148, 30264, 30517, 21324, 8013,  8921,
    16787, 15382, 1221,  30266, 6930,  15170, 7185,  28168, 9082,  866,   28036,
    18027, 15544, 1131,  22328, 9282,  14348, 13355, 5427,  731,   2360,  8892,
    32004, 21500, 8025,  16777, 2056,  3165,  854,   21563, 11975, 30725, 25015,
    2712,  17210, 4956,  11125, 2185,  28317, 17964, 9801,  27361, 26545, 10555,
    28825, 9689,  9420,  24969, 894,   27846, 10033, 17130, 3520},
  {9915,  28359, 17464, 12317, 14470, 2600,  9130,  4350,  19087, 4742,  16147,
    233,   25309, 2788,  4803,  13265, 32078, 19859, 32536, 9427,  13741, 8634,
    15536, 8154,  28459, 27923, 4286,  20937, 19945, 11332, 13765, 28687, 31355,
    29940, 32450, 21156, 8386,  20016, 5472,  7545,  285,   7472,  30951, 18200,
    7796,  6753,  2290,  12363, 8877,  25851, 28385, 1354,  13257, 15796, 31967,
    2154,  19430, 12390, 29407, 7595,  21509, 18155, 5133,  26093}};

uint64_t hashkey_position(Position* position) {
    uint64_t hash = 0;
    uint8_t  sq;
    Piece    piece;

    if (position->active_color == BLACK)
    {
        hash ^= HASH_BLACK_TO_MOVE;
    }
    if (position->casteling_rights & WKCA)
    {
        hash ^= HASH_WKCA;
    }
    if (position->casteling_rights & WQCA)
    {
        hash ^= HASH_WQCA;
    }
    if (position->casteling_rights & BKCA)
    {
        hash ^= HASH_BKCA;
    }
    if (position->casteling_rights & BQCA)
    {
        hash ^= HASH_BQCA;
    }

    for (uint8_t i = 0; i < 64; i++)
    {
        sq = SQ64TO120[i];

        if (POS_IS_SQ_INVALID(position, sq))
        {
            log_error("Found an invalid square on the board!");
            exit(1);
        }

        if (POS_IS_SQ_EMPTY(position, sq))
        {
            continue;
        }

        piece = POS_PIECE(position, sq);
        hash ^= HASHTABLE[PIECE_TO_HASH_ID(piece)][i];
    }

    return hash;
}
