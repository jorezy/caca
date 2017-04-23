/*
* ============================================================================
*  Name     : CFriendDocument from FriendDocument.h
*  Part of  : Friend
*  Created  : 21.07.2006 by 
*  Description:
*     Declares document for application.
*  Version  :
*  Copyright: 
* ============================================================================
*/

#ifndef FRIENDDOCUMENT_H
#define FRIENDDOCUMENT_H

// INCLUDES
#include <akndoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CFriendDocument application class.
*/
class CFriendDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CFriendDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CFriendDocument();

    public: // New functions

    public: // Functions from base classes
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * EPOC default constructor.
        */
        CFriendDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CFriendAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

