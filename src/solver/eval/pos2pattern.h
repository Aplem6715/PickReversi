#ifndef POS2PATTERN_H
#define POS2PATTERN_H

#include "pattern.h"

namespace eval
{
    struct PosToPattern
    {
        int numPattern;
        struct
        {
            unsigned short uid;
            unsigned short idx;
        } pattern[8];
    };

    // 各座標と対応するパターンとその３進インデックス
    static const PosToPattern kPos2Pattern[] = {
        /*A1*/ {6, {{kPatternDiag8_2, kPow3_0}, {kPatternEdgeX_1, kPow3_1}, {kPatternEdgeX_4, kPow3_8}, {kPatternConer_1, kPow3_0}, {kPatternBox10_1, kPow3_0}, {kPatternBox10_8, kPow3_0}}},
        /*B1*/ {6, {{kPatternLine2_4, kPow3_0}, {kPatternDiag7_2, kPow3_0}, {kPatternEdgeX_1, kPow3_2}, {kPatternConer_1, kPow3_1}, {kPatternBox10_1, kPow3_1}, {kPatternBox10_8, kPow3_5}}},
        /*C1*/ {5, {{kPatternLine3_4, kPow3_0}, {kPatternDiag6_2, kPow3_0}, {kPatternEdgeX_1, kPow3_3}, {kPatternConer_1, kPow3_2}, {kPatternBox10_1, kPow3_2}}},
        /*D1*/ {6, {{kPatternLine4_4, kPow3_0}, {kPatternDiag4_1, kPow3_0}, {kPatternDiag5_2, kPow3_0}, {kPatternEdgeX_1, kPow3_4}, {kPatternBox10_1, kPow3_3}, {kPatternBox10_2, kPow3_4}}},
        /*E1*/ {6, {{kPatternLine4_2, kPow3_0}, {kPatternDiag4_2, kPow3_0}, {kPatternDiag5_1, kPow3_0}, {kPatternEdgeX_1, kPow3_5}, {kPatternBox10_1, kPow3_4}, {kPatternBox10_2, kPow3_3}}},
        /*F1*/ {5, {{kPatternLine3_2, kPow3_0}, {kPatternDiag6_1, kPow3_0}, {kPatternEdgeX_1, kPow3_6}, {kPatternConer_2, kPow3_6}, {kPatternBox10_2, kPow3_2}}},
        /*G1*/ {6, {{kPatternLine2_2, kPow3_0}, {kPatternDiag7_1, kPow3_0}, {kPatternEdgeX_1, kPow3_7}, {kPatternConer_2, kPow3_3}, {kPatternBox10_2, kPow3_1}, {kPatternBox10_3, kPow3_5}}},
        /*H1*/ {6, {{kPatternDiag8_1, kPow3_0}, {kPatternEdgeX_1, kPow3_8}, {kPatternEdgeX_2, kPow3_1}, {kPatternConer_2, kPow3_0}, {kPatternBox10_2, kPow3_0}, {kPatternBox10_3, kPow3_0}}},
        //
        /*A2*/ {6, {{kPatternLine2_1, kPow3_0}, {kPatternDiag7_4, kPow3_0}, {kPatternEdgeX_4, kPow3_7}, {kPatternConer_1, kPow3_3}, {kPatternBox10_1, kPow3_5}, {kPatternBox10_8, kPow3_1}}},
        /*B2*/ {8, {{kPatternLine2_1, kPow3_1}, {kPatternLine2_4, kPow3_1}, {kPatternDiag8_2, kPow3_1}, {kPatternEdgeX_1, kPow3_0}, {kPatternEdgeX_4, kPow3_9}, {kPatternConer_1, kPow3_4}, {kPatternBox10_1, kPow3_6}, {kPatternBox10_8, kPow3_6}}},
        /*C2*/ {6, {{kPatternLine2_1, kPow3_2}, {kPatternLine3_4, kPow3_1}, {kPatternDiag4_1, kPow3_1}, {kPatternDiag7_2, kPow3_1}, {kPatternConer_1, kPow3_5}, {kPatternBox10_1, kPow3_7}}},
        /*D2*/ {6, {{kPatternLine2_1, kPow3_3}, {kPatternLine4_4, kPow3_1}, {kPatternDiag5_1, kPow3_1}, {kPatternDiag6_2, kPow3_1}, {kPatternBox10_1, kPow3_8}, {kPatternBox10_2, kPow3_9}}},
        /*E2*/ {6, {{kPatternLine2_1, kPow3_4}, {kPatternLine4_2, kPow3_1}, {kPatternDiag5_2, kPow3_1}, {kPatternDiag6_1, kPow3_1}, {kPatternBox10_1, kPow3_9}, {kPatternBox10_2, kPow3_8}}},
        /*F2*/ {6, {{kPatternLine2_1, kPow3_5}, {kPatternLine3_2, kPow3_1}, {kPatternDiag4_2, kPow3_1}, {kPatternDiag7_1, kPow3_1}, {kPatternConer_2, kPow3_7}, {kPatternBox10_2, kPow3_7}}},
        /*G2*/ {8, {{kPatternLine2_1, kPow3_6}, {kPatternLine2_2, kPow3_1}, {kPatternDiag8_1, kPow3_1}, {kPatternEdgeX_1, kPow3_9}, {kPatternEdgeX_2, kPow3_0}, {kPatternConer_2, kPow3_4}, {kPatternBox10_2, kPow3_6}, {kPatternBox10_3, kPow3_6}}},
        /*H2*/ {6, {{kPatternLine2_1, kPow3_7}, {kPatternDiag7_3, kPow3_0}, {kPatternEdgeX_2, kPow3_2}, {kPatternConer_2, kPow3_1}, {kPatternBox10_2, kPow3_5}, {kPatternBox10_3, kPow3_1}}},
        //
        /*A3*/ {5, {{kPatternLine3_1, kPow3_0}, {kPatternDiag6_4, kPow3_0}, {kPatternEdgeX_4, kPow3_6}, {kPatternConer_1, kPow3_6}, {kPatternBox10_8, kPow3_2}}},
        /*B3*/ {6, {{kPatternLine3_1, kPow3_1}, {kPatternLine2_4, kPow3_2}, {kPatternDiag4_1, kPow3_2}, {kPatternDiag7_4, kPow3_1}, {kPatternConer_1, kPow3_7}, {kPatternBox10_8, kPow3_7}}},
        /*C3*/ {5, {{kPatternLine3_1, kPow3_2}, {kPatternLine3_4, kPow3_2}, {kPatternDiag5_1, kPow3_2}, {kPatternDiag8_2, kPow3_2}, {kPatternConer_1, kPow3_8}}},
        /*D3*/ {4, {{kPatternLine3_1, kPow3_3}, {kPatternLine4_4, kPow3_2}, {kPatternDiag6_1, kPow3_2}, {kPatternDiag7_2, kPow3_2}}},
        /*E3*/ {4, {{kPatternLine3_1, kPow3_4}, {kPatternLine4_2, kPow3_2}, {kPatternDiag6_2, kPow3_2}, {kPatternDiag7_1, kPow3_2}}},
        /*F3*/ {5, {{kPatternLine3_1, kPow3_5}, {kPatternLine3_2, kPow3_2}, {kPatternDiag5_2, kPow3_2}, {kPatternDiag8_1, kPow3_2}, {kPatternConer_2, kPow3_8}}},
        /*G3*/ {6, {{kPatternLine3_1, kPow3_6}, {kPatternLine2_2, kPow3_2}, {kPatternDiag4_2, kPow3_2}, {kPatternDiag7_3, kPow3_1}, {kPatternConer_2, kPow3_5}, {kPatternBox10_3, kPow3_7}}},
        /*H3*/ {5, {{kPatternLine3_1, kPow3_7}, {kPatternDiag6_3, kPow3_0}, {kPatternEdgeX_2, kPow3_3}, {kPatternConer_2, kPow3_2}, {kPatternBox10_3, kPow3_2}}},
        //
        /*A4*/ {6, {{kPatternLine4_1, kPow3_0}, {kPatternDiag4_1, kPow3_3}, {kPatternDiag5_4, kPow3_0}, {kPatternEdgeX_4, kPow3_5}, {kPatternBox10_7, kPow3_4}, {kPatternBox10_8, kPow3_3}}},
        /*B4*/ {6, {{kPatternLine4_1, kPow3_1}, {kPatternLine2_4, kPow3_3}, {kPatternDiag5_1, kPow3_3}, {kPatternDiag6_4, kPow3_1}, {kPatternBox10_7, kPow3_9}, {kPatternBox10_8, kPow3_8}}},
        /*C4*/ {4, {{kPatternLine4_1, kPow3_2}, {kPatternLine3_4, kPow3_3}, {kPatternDiag6_1, kPow3_3}, {kPatternDiag7_4, kPow3_2}}},
        /*D4*/ {4, {{kPatternLine4_1, kPow3_3}, {kPatternLine4_4, kPow3_3}, {kPatternDiag7_1, kPow3_3}, {kPatternDiag8_2, kPow3_3}}},
        /*E4*/ {4, {{kPatternLine4_1, kPow3_4}, {kPatternLine4_2, kPow3_3}, {kPatternDiag7_2, kPow3_3}, {kPatternDiag8_1, kPow3_3}}},
        /*F4*/ {4, {{kPatternLine4_1, kPow3_5}, {kPatternLine3_2, kPow3_3}, {kPatternDiag6_2, kPow3_3}, {kPatternDiag7_3, kPow3_2}}},
        /*G4*/ {6, {{kPatternLine4_1, kPow3_6}, {kPatternLine2_2, kPow3_3}, {kPatternDiag5_2, kPow3_3}, {kPatternDiag6_3, kPow3_1}, {kPatternBox10_3, kPow3_8}, {kPatternBox10_4, kPow3_9}}},
        /*H4*/ {6, {{kPatternLine4_1, kPow3_7}, {kPatternDiag4_2, kPow3_3}, {kPatternDiag5_3, kPow3_0}, {kPatternEdgeX_2, kPow3_4}, {kPatternBox10_3, kPow3_3}, {kPatternBox10_4, kPow3_4}}},
        //
        /*A5*/ {6, {{kPatternLine4_3, kPow3_0}, {kPatternDiag4_4, kPow3_0}, {kPatternDiag5_1, kPow3_4}, {kPatternEdgeX_4, kPow3_4}, {kPatternBox10_7, kPow3_3}, {kPatternBox10_8, kPow3_4}}},
        /*B5*/ {6, {{kPatternLine4_3, kPow3_1}, {kPatternLine2_4, kPow3_4}, {kPatternDiag5_4, kPow3_1}, {kPatternDiag6_1, kPow3_4}, {kPatternBox10_7, kPow3_8}, {kPatternBox10_8, kPow3_9}}},
        /*C5*/ {4, {{kPatternLine4_3, kPow3_2}, {kPatternLine3_4, kPow3_4}, {kPatternDiag6_4, kPow3_2}, {kPatternDiag7_1, kPow3_4}}},
        /*D5*/ {4, {{kPatternLine4_3, kPow3_3}, {kPatternLine4_4, kPow3_4}, {kPatternDiag7_4, kPow3_3}, {kPatternDiag8_1, kPow3_4}}},
        /*E5*/ {4, {{kPatternLine4_3, kPow3_4}, {kPatternLine4_2, kPow3_4}, {kPatternDiag7_3, kPow3_3}, {kPatternDiag8_2, kPow3_4}}},
        /*F5*/ {4, {{kPatternLine4_3, kPow3_5}, {kPatternLine3_2, kPow3_4}, {kPatternDiag6_3, kPow3_2}, {kPatternDiag7_2, kPow3_4}}},
        /*G5*/ {6, {{kPatternLine4_3, kPow3_6}, {kPatternLine2_2, kPow3_4}, {kPatternDiag5_3, kPow3_1}, {kPatternDiag6_2, kPow3_4}, {kPatternBox10_3, kPow3_9}, {kPatternBox10_4, kPow3_8}}},
        /*H5*/ {6, {{kPatternLine4_3, kPow3_7}, {kPatternDiag4_3, kPow3_0}, {kPatternDiag5_2, kPow3_4}, {kPatternEdgeX_2, kPow3_5}, {kPatternBox10_3, kPow3_4}, {kPatternBox10_4, kPow3_3}}},
        //
        /*A6*/ {5, {{kPatternLine3_3, kPow3_0}, {kPatternDiag6_1, kPow3_5}, {kPatternEdgeX_4, kPow3_3}, {kPatternConer_4, kPow3_6}, {kPatternBox10_7, kPow3_2}}},
        /*B6*/ {6, {{kPatternLine3_3, kPow3_1}, {kPatternLine2_4, kPow3_5}, {kPatternDiag4_4, kPow3_1}, {kPatternDiag7_1, kPow3_5}, {kPatternConer_4, kPow3_7}, {kPatternBox10_7, kPow3_7}}},
        /*C6*/ {5, {{kPatternLine3_3, kPow3_2}, {kPatternLine3_4, kPow3_5}, {kPatternDiag5_4, kPow3_2}, {kPatternDiag8_1, kPow3_5}, {kPatternConer_4, kPow3_8}}},
        /*D6*/ {4, {{kPatternLine3_3, kPow3_3}, {kPatternLine4_4, kPow3_5}, {kPatternDiag6_4, kPow3_3}, {kPatternDiag7_3, kPow3_4}}},
        /*E6*/ {4, {{kPatternLine3_3, kPow3_4}, {kPatternLine4_2, kPow3_5}, {kPatternDiag6_3, kPow3_3}, {kPatternDiag7_4, kPow3_4}}},
        /*F6*/ {5, {{kPatternLine3_3, kPow3_5}, {kPatternLine3_2, kPow3_5}, {kPatternDiag5_3, kPow3_2}, {kPatternDiag8_2, kPow3_5}, {kPatternConer_3, kPow3_8}}},
        /*G6*/ {6, {{kPatternLine3_3, kPow3_6}, {kPatternLine2_2, kPow3_5}, {kPatternDiag4_3, kPow3_1}, {kPatternDiag7_2, kPow3_5}, {kPatternConer_3, kPow3_7}, {kPatternBox10_4, kPow3_7}}},
        /*H6*/ {5, {{kPatternLine3_3, kPow3_7}, {kPatternDiag6_2, kPow3_5}, {kPatternEdgeX_2, kPow3_6}, {kPatternConer_3, kPow3_6}, {kPatternBox10_4, kPow3_2}}},
        //
        /*A7*/ {6, {{kPatternLine2_3, kPow3_0}, {kPatternDiag7_1, kPow3_6}, {kPatternEdgeX_4, kPow3_2}, {kPatternConer_4, kPow3_3}, {kPatternBox10_6, kPow3_5}, {kPatternBox10_7, kPow3_1}}},
        /*B7*/ {8, {{kPatternLine2_3, kPow3_1}, {kPatternLine2_4, kPow3_6}, {kPatternDiag8_1, kPow3_6}, {kPatternEdgeX_3, kPow3_9}, {kPatternEdgeX_4, kPow3_0}, {kPatternConer_4, kPow3_4}, {kPatternBox10_6, kPow3_6}, {kPatternBox10_7, kPow3_6}}},
        /*C7*/ {6, {{kPatternLine2_3, kPow3_2}, {kPatternLine3_4, kPow3_6}, {kPatternDiag4_4, kPow3_2}, {kPatternDiag7_3, kPow3_5}, {kPatternConer_4, kPow3_5}, {kPatternBox10_6, kPow3_7}}},
        /*D7*/ {6, {{kPatternLine2_3, kPow3_3}, {kPatternLine4_4, kPow3_6}, {kPatternDiag5_4, kPow3_3}, {kPatternDiag6_3, kPow3_4}, {kPatternBox10_5, kPow3_9}, {kPatternBox10_6, kPow3_8}}},
        /*E7*/ {6, {{kPatternLine2_3, kPow3_4}, {kPatternLine4_2, kPow3_6}, {kPatternDiag5_3, kPow3_3}, {kPatternDiag6_4, kPow3_4}, {kPatternBox10_5, kPow3_8}, {kPatternBox10_6, kPow3_9}}},
        /*F7*/ {6, {{kPatternLine2_3, kPow3_5}, {kPatternLine3_2, kPow3_6}, {kPatternDiag4_3, kPow3_2}, {kPatternDiag7_4, kPow3_5}, {kPatternConer_3, kPow3_5}, {kPatternBox10_5, kPow3_7}}},
        /*G7*/ {8, {{kPatternLine2_3, kPow3_6}, {kPatternLine2_2, kPow3_6}, {kPatternDiag8_2, kPow3_6}, {kPatternEdgeX_2, kPow3_9}, {kPatternEdgeX_3, kPow3_0}, {kPatternConer_3, kPow3_4}, {kPatternBox10_4, kPow3_6}, {kPatternBox10_5, kPow3_6}}},
        /*H7*/ {6, {{kPatternLine2_3, kPow3_7}, {kPatternDiag7_2, kPow3_6}, {kPatternEdgeX_2, kPow3_7}, {kPatternConer_3, kPow3_3}, {kPatternBox10_4, kPow3_1}, {kPatternBox10_5, kPow3_5}}},
        //
        /*A8*/ {6, {{kPatternDiag8_1, kPow3_7}, {kPatternEdgeX_3, kPow3_8}, {kPatternEdgeX_4, kPow3_1}, {kPatternConer_4, kPow3_0}, {kPatternBox10_6, kPow3_0}, {kPatternBox10_7, kPow3_0}}},
        /*B8*/ {6, {{kPatternLine2_4, kPow3_7}, {kPatternDiag7_3, kPow3_6}, {kPatternEdgeX_3, kPow3_7}, {kPatternConer_4, kPow3_1}, {kPatternBox10_6, kPow3_1}, {kPatternBox10_7, kPow3_5}}},
        /*C8*/ {5, {{kPatternLine3_4, kPow3_7}, {kPatternDiag6_3, kPow3_5}, {kPatternEdgeX_3, kPow3_6}, {kPatternConer_4, kPow3_2}, {kPatternBox10_6, kPow3_2}}},
        /*D8*/ {6, {{kPatternLine4_4, kPow3_7}, {kPatternDiag4_4, kPow3_3}, {kPatternDiag5_3, kPow3_4}, {kPatternEdgeX_3, kPow3_5}, {kPatternBox10_5, kPow3_4}, {kPatternBox10_6, kPow3_3}}},
        /*E8*/ {6, {{kPatternLine4_2, kPow3_7}, {kPatternDiag4_3, kPow3_3}, {kPatternDiag5_4, kPow3_4}, {kPatternEdgeX_3, kPow3_4}, {kPatternBox10_5, kPow3_3}, {kPatternBox10_6, kPow3_4}}},
        /*F8*/ {5, {{kPatternLine3_2, kPow3_7}, {kPatternDiag6_4, kPow3_5}, {kPatternEdgeX_3, kPow3_3}, {kPatternConer_3, kPow3_2}, {kPatternBox10_5, kPow3_2}}},
        /*G8*/ {6, {{kPatternLine2_2, kPow3_7}, {kPatternDiag7_4, kPow3_6}, {kPatternEdgeX_3, kPow3_2}, {kPatternConer_3, kPow3_1}, {kPatternBox10_4, kPow3_5}, {kPatternBox10_5, kPow3_1}}},
        /*H8*/ {6, {{kPatternDiag8_2, kPow3_7}, {kPatternEdgeX_2, kPow3_8}, {kPatternEdgeX_3, kPow3_1}, {kPatternConer_3, kPow3_0}, {kPatternBox10_4, kPow3_0}, {kPatternBox10_5, kPow3_0}}},
    };
}
#endif