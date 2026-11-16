// *****************************************************************************
    // start include guard
    #ifndef DIALOGTEXTS_HPP
    #define DIALOGTEXTS_HPP
// *****************************************************************************

// dialogtexts.h from OceanStorm. Verbatim.
// Even the comment below is verbatim. It's in Spanish. We kept it. For heritage.

// 256 caracteres es mas de lo que cabe en la ventana
typedef int[ 256 ] DialogText;

struct DialogWindow
{
    int portrait_region;
    DialogText[ 2 ] texts;
};

extern DialogWindow DW_ChipReact1;
extern DialogWindow DW_ChipReact2;

// para facilitar los caracteres en español
#define a_      "\xE1"   // á
#define e_      "\xE9"   // é
#define i_      "\xED"   // í
#define o_      "\xF3"   // ó
#define u_      "\xFA"   // ú
#define n_      "\xF1"   // ñ
#define u_diere "\xFC"   // ü
#define exclam  "\xA1"   // ¡
#define interr  "\xBF"   // ¿


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
