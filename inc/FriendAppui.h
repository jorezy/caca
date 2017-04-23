/*
* ============================================================================
*  Name     : CFriendAppUi from FriendAppui.h
*  Part of  : Friend
*  Created  : 21.07.2006 by 
*  Description:
*     Declares UI class for application.
*  Version  :
*  Copyright: 
* ============================================================================
*/

#ifndef FRIENDAPPUI_H
#define FRIENDAPPUI_H

// INCLUDES
#include <aknappui.h>
#include "FriendContainer.h" 

// FORWARD DECLARATIONS
class CFriendContainer;


// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* 
*/

class CFriendAppUi : public CAknAppUi,MUIExitObserver
    {
    public: // // Constructors and destructor

		void DoExit();
        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CFriendAppUi();
    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: //Data
        CFriendContainer* iAppContainer; 
    };

#endif

// End of File
