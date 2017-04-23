/*
* ============================================================================
*  Name     : CSMSEngine from SMSEngine.h
*  Part of  : SMS
*  Created  : 03.02.2005 by Forum Nokia
*  Version  : 1.0
*  Copyright: Nokia Corporation
* ============================================================================
*/

#include "SMSEngine.h"
#include <msvids.h> // Folder Ids
#include <txtrich.h> // CRichText
#include <smut.h>
#include <eikdef.h>
#include <eikenv.h>

#include "SmsEnginePanics.pan"


#ifdef __WINS__
const TMsvId KObservedFolderId = KMsvDraftEntryId;
#else
const TMsvId KObservedFolderId =  KMsvGlobalInBoxIndexEntryId;
#endif

const TMsvId KInbox = KMsvGlobalInBoxIndexEntryId;
const TMsvId KOutbox = KMsvGlobalOutBoxIndexEntryId;
const TMsvId KDrafts = KMsvDraftEntryId;

const TInt KDelayTime = 1000*3000;
const TInt KErrMsgLength = 20;

_LIT(KEmptyMsg,"");

//  CONSTRUCTION AND DESTRUCTION
CSmsEngine* CSmsEngine::NewL(MSmsEngineObserver& aObserver)
    {
    CSmsEngine* self = CSmsEngine::NewLC(aObserver);
    CleanupStack::Pop( self );
    return self;
    }

CSmsEngine* CSmsEngine::NewLC(MSmsEngineObserver& aObserver)
    {
    CSmsEngine* self = new( ELeave ) CSmsEngine(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CSmsEngine::~CSmsEngine()
    {
	delete iMsvEntry;
	iMsvEntry = NULL;

	delete iMsvSession;
	iMsvSession = NULL;

	delete iEngine;
	iEngine = NULL;
    }

CSmsEngine::CSmsEngine(MSmsEngineObserver& aObserver) : iObserver(aObserver)
    {
    }

void CSmsEngine::ConstructL()
    {
	iAutomaticDelete = EFalse;

	iListeningForIncoming = ETrue;

	iEngine = CSMSMtmsEngine::NewL(*this);

	// SMS automatic receiving needs a session to the messaging server
	iMsvSession = CMsvSession::OpenAsyncL(*this);
    }

//listening for incoming message
void CSmsEngine::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
                                           TAny* aArg2, TAny* /*aArg3*/)
    {
    switch (aEvent)
        {
            //One way to make sure that the message has been sent
            //is to watch the sent items folder.
            //The sent SMS could also be deleted here

            //case EMsvEntriesMoved:
        	//{
	        //Check if our message has gone to the sent items
			//if (aArg2 && (*(static_cast<TMsvId*>(aArg2)) == KMsvSentEntryId) )
			//	  {
            //    CMsvEntrySelection* entries =
            //    	static_cast<CMsvEntrySelection*>(aArg1);

                //Note: this doesn't work if the message has been waiting
                //to be sent and has been sent after a boot.
                //In that case the deletion should be done when the server is
                //ready again and the sent items (and also drafts) is searched.
            //    if (entries && iSentMessageId == entries->At(0))
            //        {
            //        Model().State(ESmsEngineSent);
		    		//if it's our message and we're supposed to delete it
		    //		  if( iAutomaticDeleteFromSentItems ) //note a new flag
		    //    		  {
		    //			  iMsvSession->RemoveEntry(iSentMessageId);
		    //    		  }
            //        }
            //    iObserver.MessageSent();
			//	  }
			//break;
        	//}
        case EMsvServerReady:
            // Initialise iMsvEntry
            if (!iMsvEntry)
                {
                iMsvEntry = CMsvEntry::NewL(*iMsvSession, KInbox,
                                            TMsvSelectionOrdering());


                //Sent SMS might be left to drafts if the phone was booted during
                //SMS sending.

                //Engine could also be used for retrieving a response for
                //a message. Response could arrive after phone has been booted
                //so it should be checked here.
                //Now that we're ready to handle messages we could check
                //if there's a response in the inbox.
                //This could be the case only if the message has been
                //sent and the phone has been booted while waiting.

                //The same applies to sending and deleting the sent message
                //Sent message could be left in the sent items if the phone
                //was booted before the sent message had been removed.

                //for these situations the engine should be implemented as a
                //state machine and engines state should be checked on initialization.
                //Of course there might be multiple clients (for  if engine
                //implementation would be a server) so there's a need to
                //have own state of each client (as in server session).

                //implementation could be something like
                //TSmsEngineState state = Model().State();

				//if( state == ESmsEngineSending )
				//	{

					//Make sure that the sent message will be deleted.
					//TBool found(EFalse);

					//TRAPD(error, SearchForSentMessageFromSentMessagesL(found) );

					//if ( !found )
					//	{
						//If the sent message is stuck in drafts
					//	TRAP(error, SearchForSentMessageFromDraftsL(found) );
						//If this is the case then resend!
					//	}

					//if the message is somehow mystically lost,
					//then it's handled as the message has been sent.
					//if( !found )
					//	{
					//	Model().State(ESmsEngineSMSSent);
					//	}
				//  }

                }
            break;

        case EMsvEntriesCreated:
            // Only look for changes in the Inbox
            if (aArg2 &&  *(static_cast<TMsvId*>(aArg2)) == KObservedFolderId)
                {
                CMsvEntrySelection* entries =
                                    static_cast<CMsvEntrySelection*>(aArg1);
                if( entries->Count() >= 1 )
                    {
                    iNewMessageId = entries->At(0);
                    }
                else
                    {
                    Panic(ESmsEngineInternal);
                    }
                }
            break;

        case EMsvEntriesChanged:
            //Look for changes. When using the emulator observed folder is
            //drafts, otherwise inbox.
            //Also a check for the iListeningForIncoming is made
            if (aArg2 &&  *(static_cast<TMsvId*>(aArg2)) == KObservedFolderId
                      && iListeningForIncoming )
                {
                CMsvEntrySelection* entries =
                                    static_cast<CMsvEntrySelection*>(aArg1);

                //improvement for the case of receiving a response

                //When the phone is booted and the message has arrived before
				//application has started it only receives notification of
				//changes
				//in the inbox (when the user reads the message!), not the
				//creation where these id is initialized.
				//therefore we check when changes occur that if the iNewMessageID
				//is -1 we make client handle the response.
                //Check the id of the message (iNewMessageId is set
                //in case of EMsvEntriesCreated)

                //code could be something like:
                //if ( entries && (iNewMessageId == entries->At(0) || iNewMessageId == -1) )

                if( entries->Count() < 1 )
                {
                    Panic(ESmsEngineInternal);
                }
                else if (iNewMessageId == entries->At(0))
                    {

                    if( !iMsvEntry )
                    	{
                    	Panic(ESmsEngineNotInitialized);
                    	return;
                    	}

                    // Set entry context to the new message
                    iMsvEntry->SetEntryL(iNewMessageId);

					// Check the type of the arrived message and that the
					// message is complete.
					// only SMS's are our consern.
					if ( iMsvEntry->Entry().iMtm != KUidMsgTypeSMS ||
					     !iMsvEntry->Entry().Complete() )
					    {
						return;
    					}

                    // Read-only store.
                    CMsvStore* store = iMsvEntry->ReadStoreL();
                    CleanupStack::PushL(store);

					// Get address of received message.
					TBuf<KSmsMessageLength> iAddress(
					                               iMsvEntry->Entry().iDetails
					                               );

                    if (store->HasBodyTextL())
                        {
						CRichText* richText = CRichText::NewL(
                           CEikonEnv::Static()->SystemParaFormatLayerL(),
                           CEikonEnv::Static()->SystemCharFormatLayerL());
                        CleanupStack::PushL(richText);
                        store->RestoreBodyTextL(*richText);
                        const TInt length = richText->DocumentLength();

						TBuf<KSmsMessageLength> number;
						TPtrC ptr = richText->Read(0, length);

					    //iMessage = ptr;
					    iMessage.Copy(ptr);
					    //iLogView->DrawTextL( address );
					    CleanupStack::PopAndDestroy(richText);


                        iObserver.MessageReceived(iAddress, iMessage);

                        }
                    else
                        {
                        iObserver.MessageReceived(iAddress, KEmptyMsg);
                        }

					CleanupStack::PopAndDestroy(store);

					// Automatic delete setting
					if ( iAutomaticDelete )
						{
						// Ncnlist seems to panic if there is no time to play arrived
						// message tone before deletion.
						//Codescanner gives a critical
						User::After(KDelayTime);

						iMsvSession->RemoveEntry(iNewMessageId);

						iObserver.MessageDeleted();
						}
                    }
                }
            break;

        default:
            break;
        }
    }

//callback from CSMSMtmsEngine
void CSmsEngine::HandleMessageSentL(TInt aError)
    {
    if (aError == KErrNone)
        {
	    iObserver.MessageSent();
        }
    else // If there was some error sending the SMS
        {
        iObserver.SmsEngineError(aError);
        }
    }

//sending a sms
void CSmsEngine::SendSmsL(const TDesC& aAddr, const TDesC& aMsg)
    {
    TInt err (KErrNone);
    #ifdef __SERIES60_30__
    TRAP(err,SendSmsInThirdEditionL(aAddr, aMsg)); //Use RSendAs if available
    #else
	// Demonstrate the usage of MTM for sending the message
	// Create a new SMS to drafts
	iSentMessageId = iEngine->CreateSMSMessageL(aAddr, aMsg);

	// Validate it before sending
	if ( iEngine->ValidateCreatedSMS() )
		{
		// Now send
		TRAP(err, iEngine->SendSMSL() );
		}
	else
		{
		iObserver.SmsEngineError( KErrArgument );
		}
    #endif

	if( err )
	    {
	    iObserver.SmsEngineError(err);
	    }
	 else
	    {
	    iObserver.SendingMessage();
	    }
    }

//Sending the sms in third edition with RSendAs
void CSmsEngine::SendSmsInThirdEditionL(const TDesC& aAddr, const TDesC& aMsg)
    {
#ifdef __SERIES60_30__

    RSendAs sendAs;
    User::LeaveIfError(sendAs.Connect());
    CleanupClosePushL(sendAs);

    RSendAsMessage sendAsMessage;
    sendAsMessage.CreateL(sendAs, KUidMsgTypeSMS);
    CleanupClosePushL(sendAsMessage);

    // prepare the message
    sendAsMessage.AddRecipientL(aAddr, RSendAsMessage::ESendAsRecipientTo);
    sendAsMessage.SetBodyTextL(aMsg);

    // send the message
    sendAsMessage.SendMessageAndCloseL();

    // sendAsMessage (already closed)
    CleanupStack::Pop();

    // sendAs
    CleanupStack::PopAndDestroy();
#endif
    }

//setting the received message notyfication on/off
void CSmsEngine::ListenforIncomingSms(TBool aListening)
    {
    // the iMsvSession could be opened in here when needed
    // now the flag only indicates that do we notify the client
    iListeningForIncoming = aListening;
    }

//setting the automatic deletion of received message on/off
void CSmsEngine::SetAutomaticDeletetion(TBool aDeletion)
    {
    // automatic deletion for incoming message isn't done if
    // iListeningForIncoming is false
    iAutomaticDelete = aDeletion;
    }

//getting messages from a folder
void CSmsEngine::GetFolderSMSMessageInformationL(TMsvId aFolderID,
												CDesCArrayFlat*& aAddresses,
												CDesCArrayFlat*& aMessages)
    {
    iEngine->GetFolderSMSMessageInformationL(aFolderID, aAddresses, aMessages);
    }

//getting messageids from a folder after having called
//GetFolderSMSMessageInformationL
RArray<TMsvId>* CSmsEngine::GetMessageIds()
    {
    return iEngine->GetMessageIds();
    }

//Copying a message to specified folder
void CSmsEngine::CopyMessageL( TMsvId aMessageId, TMsvId aFolder )
    {
    iEngine->CopyMessageL(aMessageId, aFolder);
    }

//Moving a message to specified folder
void CSmsEngine::MoveToFolderL( TMsvId aMessageId,  TMsvId aFolder )
    {
    iEngine->MoveToFolderL(aMessageId, aFolder);
    }

//Deleting a message from specified folder
void CSmsEngine::DeleteMessageL( TMsvId aMessageId )
    {
    iEngine->DeleteMessageL(aMessageId);
    }

// End of file
