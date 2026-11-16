// *****************************************************************************
    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/ui/ChatTexts.h"
// *****************************************************************************

ChatMessage[ NumChatMessages ] ChatMessages =
{
    {
        UserChip,
        {
            "Pixel Hustler is almost done. Proper \n"
            "announcement tomorrow.",

            "Pixel Hustler casi terminado. Ma" n_ "ana \n"
            "lo anuncio en condiciones."
        }
    },
    {
        UserJastro,
        {
            "Pixel Hustler is a shameless rip-off of \n"
            "my game Ocean Storm. A disgrace to \n"
            "Vircon32 developers.",

            "Pixel Hustler es un plagio descarado de \n"
            "mi juego Ocean Storm. Una verg" u_diere "enza \n"
            "para los desarrolladores de Vircon32."
        }
    },
    {
        UserRidote,
        {
            "Excellent. GTA with a certificate of \n"
            "maritime origin.",

            "Excelente. GTA con certificado de \n"
            "procedencia mar" i_ "tima."
        }
    },
    {
        UserTotespare,
        {
            "Five minutes since the announcement \n"
            "and we already have a civil war. The \n"
            "community is alive again.",

            "Cinco minutos anunciado y ya tenemos \n"
            "guerra civil. La comunidad vuelve a \n"
            "estar viva."
        }
    },
    {
        UserFreak1337,
        {
            "I just want to know if the car takes \n"
            "off when you hold the accelerator.",

            "Yo solo quiero saber si el coche \n"
            "despega cuando mantienes pulsado \n"
            "acelerar."
        }
    },
    {
        UserKidandcat,
        {
            "Has anyone actually played it, or are \n"
            "we giving opinions out of tradition?",

            interr "Alguien ha probado el juego o \n"
            "estamos opinando por tradici" o_ "n?"
        }
    },
    {
        UserPaletochen,
        {
            "Joyrider is at the Discord Caf" e_ ". If \n"
            "anyone knows where all this crap \n"
            "comes from, it's him. That man has \n"
            "released so many games he probably \n"
            "remembers who started every fight.",

            "Joyrider est" a_ " en el Discord Caf" e_ ". \n"
            "Si alguien sabe de d" o_ "nde viene toda \n"
            "esta mierda, es " e_ "l. Ese hombre ha \n"
            "publicado tantos juegos que \n"
            "probablemente recuerde hasta qui" e_ "n \n"
            "empez" o_ " cada pelea."
        }
    }
};

ChatToast[ NumChatToasts ] ChatToasts =
{
    { UserJastro,    { "Pixel Hustler is a rip-off", "Pixel Hustler es un plagio" } },
    { UserRidote,    { "Excellent. GTA with...", "Excelente. GTA con..." } },
    { UserTotespare, { "Five minutes and...", "Cinco minutos y ya..." } },
    { UserFreak1337, { "Does the car take off?", interr "El coche despega?" } },
    { UserKidandcat, { "Has anyone played it?", interr "Alguien lo ha probado?" } }
};

int*[ NumChatUsers ] ChatUserNames;
int[ NumChatUsers ] ChatUserColors;

void InitializeChatUsers()
{
    ChatUserNames[ UserChip ]       = "Chip Buster";
    ChatUserNames[ UserJastro ]     = "Jastro";
    ChatUserNames[ UserRidote ]     = "Ridote";
    ChatUserNames[ UserTotespare ]  = "Totespare";
    ChatUserNames[ UserFreak1337 ]  = "Freak1337";
    ChatUserNames[ UserKidandcat ]  = "kidandcat";
    ChatUserNames[ UserPaletochen ] = "Paletochen";
    ChatUserNames[ UserCarra ]      = "Carra";

    ChatUserColors[ UserChip ]       = make_color_rgb( 255, 130, 50 );
    ChatUserColors[ UserJastro ]     = make_color_rgb( 60, 220, 120 );
    ChatUserColors[ UserRidote ]     = make_color_rgb( 255, 80, 90 );
    ChatUserColors[ UserTotespare ]  = make_color_rgb( 255, 160, 40 );
    ChatUserColors[ UserFreak1337 ]  = make_color_rgb( 190, 255, 60 );
    ChatUserColors[ UserKidandcat ]  = make_color_rgb( 60, 220, 250 );
    ChatUserColors[ UserPaletochen ] = make_color_rgb( 255, 220, 80 );
    ChatUserColors[ UserCarra ]      = make_color_rgb( 160, 150, 240 );
}
