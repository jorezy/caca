/*
* ============================================================================
*  Name     : CSMSMtmsEngine from SMSMtmsEngine.h
*  Part of  : SMS
*  Created  : 12.03.2005 by Forum Nokia
*  Description:
*     SMS engine handles SMS message creation, validation, sending and SMS
*     message handling (i.e moving, deleting, copying, retreiving message data).
*  Version  : 1.0
*  Copyright: Nokia Corporation
* ============================================================================
*/

#ifndef SMS__ENGINE
#define SMS__ENGINE

// INCLUDES 
#include <e32base.h> // CBase
#include <msvapi.h>  // MMsvSessionObserver
#include <badesca.h> // CDesCArrayFlat

// FORWARD DECLARATIONS
class CClientMtmRegistry;
class CSmsClientMtm;
class MSMSMtmsEngineObserver;

// CLASS DECLARATION

/**
*  SMS engine.
*/
class CSMSMtmsEngine : public CActive, public MMsvSessionObserver
    {
    public:
	   
		/**
		* Create new CSMSMtmsEngine object
		*
		* @param aObserver a reference to MSMSMtmsEngineObserver. 
		* @return a pointer to the created instance of CSMSMtmsEngine
		*/
        static CSMSMtmsEngine* NewL(MSMSMtmsEngineObserver& 
                                           aObserver);
        
		/**
		* Destructor.
		*/
		~CSMSMtmsEngine();

    public:

		/**
		* Send an SMS that has been created with CreateSMSMessageL().
		*/
		void SendSMSL();

		/**
		* Create an SMS message.
		* @param aAddress a destination address.
		* @param aMessage message to be sent. 
		*/
        TMsvId CreateSMSMessageL(const TDesC& aAddress, const TDesC& aMessage);

		/**
		* Validate message that has been created with CreateDraftSMSMessageL().
		*/
        TBool ValidateCreatedSMS();
       
		/**
		* Move an SMS message to folder.
		* @param aMessageID id of an SMS message.
		* @param aFolder target folder id.
		*/
		void MoveToFolderL( TMsvId aMessageId,  TMsvId aFolder );

		/**
		* Delete an SMS message.
		* @param aMessageID id of an SMS message.
		*/
		void DeleteMessageL( TMsvId aMessageId );

		/**
		* Copy an SMS message to folder.
		* @param aMessageID id of an SMS message.
		* @param aFolder target folder id.
		*/
		void CopyMessageL( TMsvId aMessageId, TMsvId aFolder );

		/**
		* Gets the originator address.
		* @param aMessageID id of an SMS message.
		* @param aAddress originator address.
		*/
		void GetMessageAddressL( TMsvId aMessageId, TDes& aAddress );

		/**
		* Gets the message body from the messaging store.
		* @param aMessageID id of an SMS message.
		* @param aMessage the message.
		* @return ETrue if message correspongin aMessageId is valid SMS 
		*         (.sis files / settings are filtered out), EFalse otherwise.
		*/
		TBool GetMessageL( TMsvId aMessageId, TDes& aMessage );

		/**
		* Gets the message body summary information(just the beginning of body)
		* @param aMessageID id of an SMS message.
		* @param aMessage the beginning of message.
		* @return ETrue if message correspongin aMessageId is valid SMS 
		*         (.sis files / settings are filtered out), EFalse otherwise.
		*/
		TBool GetMessageIndexBodyTextL( TMsvId aMessageId, TDes& aMessage );

		/**
		* Retrieve one folder's SMS messages (their address+message body).
		* @param aAddresses Array of recipient addresses
		* @param aMessages Array of message bodys.
		*/
		void GetFolderSMSMessageInformationL( TMsvId aFolderID, 
		                                      CDesCArrayFlat*& aAddresses, 
											  CDesCArrayFlat*& aMessages );

		/**
		* All ids of messages that has been got with 
		* GetFolderSMSMessageInformationL.
	    * @return array of ids.
		*/
		RArray<TMsvId>* GetMessageIds();

    private:
		/**
		* A constructor.
		* @param aObserver Observer of this engine.
		*/
        CSMSMtmsEngine(MSMSMtmsEngineObserver& aObserver);

		/**
		* ConstructL()
		*/
        void ConstructL();

		/**
		* Creates SMS client.
		*/
        void CreateMtmClientL();

    private: // from CActive
        virtual void DoCancel();
        virtual void RunL();


    private: // from MMsvSessionObserver
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, 
                                 TAny* aArg2, TAny* aArg3);

    private:

		// SMS client MTM interface
		CSmsClientMtm*      iSmsMtm;

		// Observer of this engine.
        MSMSMtmsEngineObserver& iObserver;

		// Controls asynchronous commands in the messaging system
		// Get progress information about the sending.
        CMsvOperation*      iMsvOper;

		// Encapsulates an array of entry IDs.
        CMsvEntrySelection* iEntrySelection;

		// Represents a session to the messaging server
        CMsvSession*        iSession;   

		// This registry holds details of the all the Client-side 
		// MTMs currently available on the system. 
        CClientMtmRegistry* iClientMtmReg;
       
		// Array of ids
		RArray<TMsvId>*     iIdArray;
       
		// Id of an SMS message
        TMsvId              iSmsId;
    };

/**
*  CSMSMtmsEngine SMS engine observer.
*/
class MSMSMtmsEngineObserver
    {
    public:
        virtual void HandleMessageSentL(TInt aError) = 0;
    };


#endif  // SMS__ENGINE

