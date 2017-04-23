/*
* ============================================================================
*  Name     : CFriendApp from FriendApp.h
*  Part of  : Friend
*  Created  : 21.07.2006 by 
*  Description:
*     Declares main application class.
*  Version  :
*  Copyright: 
* ============================================================================
*/

#ifndef FRIENDAPP_H
#define FRIENDAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidFriend = { 0x0F4E86E7 };

// CLASS DECLARATION

/**
* CFriendApp application class.
* Provides factory to create concrete document object.
* 
*/
class CFriendApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CFriendDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidFriend).
        * @return The value of KUidFriend.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

