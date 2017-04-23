#ifndef CSMSENGINE_H
#define CSMSENGINE_H

//  EXTERNAL INCLUDES
#include <e32base.h>

//  INTERNAL INCLUDES
#include "SMSMtmsEngine.h"

//  CONSTANTS
const TInt KSmsMessageLength = 512;
const TInt KAddressLength = 64;


class MSmsEngineObserver
{
    public:
        virtual void SendingMessage()=0;
        virtual void MessageSent()=0;
        virtual void MessageReceived(const TDesC& addr, const TDesC& msg)=0;
        virtual void SmsEngineInitialized()=0;
        virtual void SmsEngineError(TInt aErrorCode)=0;
        virtual void MessageDeleted()=0; //if automatic deletion is on
};

//  CLASS DEFINITION
/**
 * Class CSmsEngine wraps the CSMSMtmsEngine class and offers
 * an easy to use interface for basic SMS functionality.
 *
 * There are a couple of "almost implemented" features, like deleting the sent message
 * and checking for a response to the sent message.
 * Also some spesial error situations are described in code comments.
 *
 */
class CSmsEngine : public CBase, public MMsvSessionObserver, public MSMSMtmsEngineObserver
    {
    public:     // Constructors and destructor

        /**
        * NewL
        * @param aObserver the observer
        */
        static CSmsEngine* NewL(MSmsEngineObserver& aObserver);

        /**
        * NewL
        * @param aObserver the observer
        */
        static CSmsEngine* NewLC(MSmsEngineObserver& aObserver);

        /**
        * Destructor
        */
        ~CSmsEngine();

     public:     //new functions
        /**
        * Sending a message.
        * @param aAddr the receiver address
        * @param aMsg the message to send
        */
        void SendSmsL(const TDesC& aAddr, const TDesC&aMsg); //nyt CreateSmsMessage, validateCreatedSms ja SendSms, laita kaikki piiloon

        /**
        * This enables the client not to listen for incoming messages.
        * By default client receives notitycation when messages arrive
        * @param
        */
        void ListenforIncomingSms(TBool aListening);

        /**
        * Do we delete the received message automatically.
        * Default is no.
        * @param
        */
        void SetAutomaticDeletetion(TBool aDeletion);


        /**
        * A main function in viewing folder contect.
        * For usage see GUI's AppUi class.
        * @param aFolderId the folder
		* @param aAddresses Array of recipient addresses
		* @param aMessages Array of message bodys.
        */
        void GetFolderSMSMessageInformationL(TMsvId aFolderID,
												CDesCArrayFlat*& aAddresses,
												CDesCArrayFlat*& aMessages);

        /**
        * Used in getting the message Ids to GUI's listbox id array.
        * Note that GetFolderSMSMessageInformationL
        * has to have been called first so the id's for the folder
        * have been collocted.
        * For usage see GUI's CListboxView class.
	    * @return array of ids
        */
        RArray<TMsvId>* GetMessageIds();

        /**
        * Copying a message to a specified folder
		* @param aMessageID id of an SMS message.
		* @param aFolder target folder id.
        */
        void CopyMessageL( TMsvId aMessageId, TMsvId aFolder );

        /**
        * Moving a message to a specified folder
		* @param aMessageID id of an SMS message.
		* @param aFolder target folder id.
        */
		void MoveToFolderL( TMsvId aMessageId,  TMsvId aFolder );

        /**
        * Deleting a message
		* @param aMessageID id of an SMS message.
        */
		void DeleteMessageL( TMsvId aMessageId );

    public:  //from base classes

        /**
        * From MMsvSessionObserver
        */
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

        /**
        * From MSMSMtmsEngineObserver
        */
        void HandleMessageSentL(TInt aError);

    private:    // Constructors

        /**
        * Constructor
        * @param aObserver the observer
        */
        CSmsEngine(MSmsEngineObserver& aObserver);

        /**
        * ConstructL() initializes objects
        */
        void ConstructL();

    private:

        /**
        * This function is used in 3rd edition to send the sms.
        * Called by SendSmsL
        * @param aAddr the receiver address
        * @param aMsg the message to send
        */
        void SendSmsInThirdEditionL(const TDesC& aAddr, const TDesC& aMsg);

    private:    // Data

		//The observer
		MSmsEngineObserver& iObserver;

		// SMS engine
		CSMSMtmsEngine* iEngine;

		// Parser that searches phone numbers
		//CSMSParser* iParser;

		// Recipient address
        TBuf<KAddressLength> iAddress;

		// Message body
		TBuf<KSmsMessageLength> iMessage;

		// ETrue is automatic delete is on, EFalse otherwise
		TBool iAutomaticDelete;

		// are we listening for incoming messages
		TBool iListeningForIncoming;

		// Session with the messaging server
		CMsvSession* iMsvSession;

		// CMsvEntry accesses and acts upon a particular Message Server entry
        CMsvEntry* iMsvEntry;

		// Id of a new message
        TMsvId iNewMessageId;

        // Id of the sent message
        TMsvId iSentMessageId;

    };

//  INLINE FUNCTIONS

#endif      //  CSMSENGINE_H

// End of file
