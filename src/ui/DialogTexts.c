// *****************************************************************************
    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/ui/DialogTexts.h"
// *****************************************************************************

// Translation workflow (Jastro):
//   1. write the English text
//   2. ask Carra to translate it
//   3. Carra: "you're Spanish"
//   4. Jastro: "exactly, I'm too close to the language"

DialogWindow DW_Intro1 =
{
    RegionPortraitCarra,
    {
        "Welcome to Pixel Hustler. Every \n"
        "building in this city is a chip, and \n"
        "every sign is a game made by the \n"
        "Vircon32 community.",

        "Bienvenido a Pixel Hustler. Cada \n"
        "edificio de esta ciudad es un chip, y \n"
        "cada cartel es un juego hecho por la \n"
        "comunidad de Vircon32."
    }
};

DialogWindow DW_Intro2 =
{
    RegionPortraitJastro,
    {
        "I designed this whole city myself!",

        exclam "Esta ciudad la he dise" n_ "ado yo!"
    }
};

DialogWindow DW_Intro3 =
{
    RegionPortraitCarra,
    {
        "You said 'make it look like a \n"
        "motherboard' and went for lunch.",

        "Dijiste 'que parezca una placa base' \n"
        "y te fuiste a comer."
    }
};

DialogWindow DW_Intro4 =
{
    RegionPortraitJastro,
    {
        "Delegating is also designing. \n"
        "Now, how do I get into a car?",

        "Delegar tambi" e_ "n es dise" n_ "ar. \n"
        interr "Y c" o_ "mo me subo a un coche?"
    }
};

DialogWindow DW_Intro5 =
{
    RegionPortraitCarra,
    {
        "Walk up to one and press B. A sign \n"
        "will show up. The OceanStorm one.",

        "Ac" e_ "rcate a uno y pulsa B. Saldr" a_ " \n"
        "un cartel. El de OceanStorm."
    }
};

DialogWindow DW_Intro6 =
{
    RegionPortraitJastro,
    {
        "Nobody will notice.",

        "Nadie se dar" a_ " cuenta."
    }
};

DialogWindow DW_FirstCar1 =
{
    RegionPortraitJastro,
    {
        "Wait. This doesn't drive like the \n"
        "heli at all!",

        exclam "Espera! Esto no se conduce como \n"
        "el heli."
    }
};

DialogWindow DW_FirstCar2 =
{
    RegionPortraitCarra,
    {
        "I rewrote it. Cars have inertia. \n"
        "A is the handbrake, by the way.",

        "Lo reescrib" i_ ". Los coches tienen \n"
        "inercia. A es el freno de mano."
    }
};

DialogWindow DW_FirstCar3 =
{
    RegionPortraitJastro,
    {
        "So it's the heli code... with \n"
        "inertia. Nobody will notice.",

        "O sea, el c" o_ "digo del heli... con \n"
        "inercia. Nadie se dar" a_ " cuenta."
    }
};

DialogWindow DW_FirstCar4 =
{
    RegionPortraitCarra,
    {
        "You keep saying that.",

        "Deja de decir eso."
    }
};
