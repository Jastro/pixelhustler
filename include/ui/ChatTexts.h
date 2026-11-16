// *****************************************************************************
    // start include guard
    #ifndef CHATTEXTS_HPP
    #define CHATTEXTS_HPP

    // include project headers
    #include "DialogTexts.h"
// *****************************************************************************

struct ChatMessage
{
    int user;
    DialogText[ 2 ] texts;
};

struct ChatToast
{
    int user;
    DialogText[ 2 ] texts;
};

#define NumChatMessages 7
#define NumChatToasts   5

extern ChatMessage[ NumChatMessages ] ChatMessages;
extern ChatToast[ NumChatToasts ] ChatToasts;
extern int*[ NumChatUsers ] ChatUserNames;
extern int[ NumChatUsers ] ChatUserColors;

void InitializeChatUsers();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
