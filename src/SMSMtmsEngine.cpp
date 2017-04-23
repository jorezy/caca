/*
* ============================================================================
*  Name     : CSMSMtmsEngine from SMSMtmsEngine.h
*  Part of  : SMS
*  Created  : 12.03.2005 by Forum Nokia
*  Version  : 1.0
*  Copyright: Nokia Corporation
* ============================================================================
*/

// INCLUDE FILES
#include "SMSMtmsEngine.h"
#include "SmsEnginePanics.pan"

// SYSTEM FILES
#include <f32file.h>        // TParsePtrC
#include <mmsclient.h>      // CMmsClientMtm
#include <mtclreg.h>        // CClientMtmRegistry
#include <mtmdef.h>         // KMsvMessagePartBody
#include <smsclnt.h>        // CSmsClientMtm
#include <smscmds.h>        // ESmsMtmCommandScheduleCopy
//Not in 1.2 or 2.0:
#include <smutset.h>        // CSmsSettings
//Not in 2.0
#include <smuthdr.h>        // CSmsHeader
#include <txtrich.h>        // CRichText
#include <eikenv.h>

const TInt KMessageAddressLength = 100;
const TInt KMessageBodySize = 512;
const TInt KArrayGranularity=10;

_LIT(KSenderJohnDoe,"*Unknown*");

CSMSMtmsEngine* CSMSMtmsEngine::NewL(
                                      MSMSMtmsEngineObserver& aObserver)
    {
    CSMSMtmsEngine* self =
                           new (ELeave) CSMSMtmsEngine(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CSMSMtmsEngine::CSMSMtmsEngine(
                                      MSMSMtmsEngineObserver& aObserver)
: CActive(0),
  iObserver(aObserver),
  iSmsId(KMsvNullIndexEntryId)
    {

    }

void CSMSMtmsEngine::ConstructL()
    {
    CActiveScheduler::Add(this);

    // iEntrySelection encapsulates an array of entry IDs
    iEntrySelection = new (ELeave) CMsvEntrySelection;

    // Represents a channel of communication between a client thread
    // (Client-side MTM, User Interface MTM, or message client application)
    // and the Message Server thread.
	// Session is opened asynchorously. CreateMtmClientL() is called afterwards.
	// Another possibility is use OpenSyncL which is synchronous.
    iSession = CMsvSession::OpenAsyncL(*this);
    }

// Creates CSmsClientMtm after session has been opened.
void CSMSMtmsEngine::CreateMtmClientL()
    {
    // Client-side MTM registry.
    iClientMtmReg = CClientMtmRegistry::NewL(*iSession);

    //3rd: edition notice: If capabilities are missing, then iSmsMtm stays null
    // Get the SMS Mtm client from the registry
    iSmsMtm = static_cast<CSmsClientMtm*>(iClientMtmReg->NewMtmL(KUidMsgTypeSMS));
    }

CSMSMtmsEngine::~CSMSMtmsEngine()
    {
    Cancel();
    delete iMsvOper;
    delete iEntrySelection;
    delete iSmsMtm;
    delete iClientMtmReg;
    delete iSession;
    }

void CSMSMtmsEngine::DoCancel()
    {
    if (iMsvOper)
        {
        iMsvOper->Cancel();
        delete iMsvOper;
        iMsvOper = NULL;
        }
    }

void CSMSMtmsEngine::RunL()
    {
    iObserver.HandleMessageSentL(iStatus.Int());
    }

TMsvId CSMSMtmsEngine::CreateSMSMessageL(const TDesC& aAddress,
                                              const TDesC& aMessage)
    {
    // Set SMS parameters
    TMsvEntry indexEntry;
	indexEntry.iDate.HomeTime();
    indexEntry.SetInPreparation(ETrue);
	// This is an SMS message
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;

	//Gets the ID of the current SMS service.
	//in 3rd edition crashes here if capabilities are wrong
    indexEntry.iServiceId = iSmsMtm->ServiceId();

    // Create entry to drafts
    iSmsMtm->SwitchCurrentEntryL(KMsvDraftEntryId);
    //iSmsMtm->SwitchCurrentEntryL(KMsvGlobalOutBoxIndexEntryId); //test!

	// Creates a new child entry owned by the context synchronously.
    iSmsMtm->Entry().CreateL(indexEntry);

    // Set the MTM's active context to the new message
    iSmsId = indexEntry.Id();
    iSmsMtm->SwitchCurrentEntryL(iSmsId);

    // Add message body. Body is set twice because index entry keeps a copy
	// of some summary information. Index entry and full stored entry
	// must be in sync.
    CRichText& body = iSmsMtm->Body();
    body.Reset();
    body.InsertL(0, aMessage);
    indexEntry.iDescription.Set(aMessage);

    // Add destination address (recipient).Copy address also to the index entry
    iSmsMtm->AddAddresseeL(aAddress);
    indexEntry.iDetails.Set(aAddress);

    // Commit changes because index entry is only a local variable
    iSmsMtm->Entry().ChangeL(indexEntry);

    // Save full message data to the store
    iSmsMtm->SaveMessageL();

    return iSmsId;

    }

TBool CSMSMtmsEngine::ValidateCreatedSMS()
    {
    TMsvPartList partsToBeChecked =
        KMsvMessagePartBody | KMsvMessagePartRecipient |
        KMsvMessagePartOriginator | KMsvMessagePartDate;

	// ValidateMessage return KErrNone if message is valid.
    TMsvPartList failedParts = iSmsMtm->ValidateMessage(partsToBeChecked);

	if (failedParts == KMsvMessagePartNone)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
    }

// Before sending sms message should be created CreateSMSMessageL() and
// Validated ValidateCreatedSMS().
void CSMSMtmsEngine::SendSMSL()
    {
    // Changes the entry on which later actions are performed to the entry
	// with the specified TMsvId.
    iSmsMtm->SwitchCurrentEntryL(iSmsId);

    // Load the created message
    iSmsMtm->LoadMessageL();

    // Gets the current SMS service settings
    CSmsSettings& serviceSettings = iSmsMtm->ServiceSettings();


	// Gets the number of service centre addresses stored in this object.
	#ifndef __SERIES60_30__
        const TInt numSCAddresses = serviceSettings.NumSCAddresses();
    #else // In 3.0
        const TInt numSCAddresses = serviceSettings.ServiceCenterCount();
    #endif

	// There should always be a service center number
	if (numSCAddresses > 0)
		{
        TInt scIndex=0;
        #ifndef __SERIES60_30__
        scIndex = serviceSettings.DefaultSC();
        // get the service center number:
        if ((scIndex < 0) || (scIndex >= numSCAddresses))
	        {
	        //scIndex = 0;
	        Panic(ESmsEngineInternal);
	        }
        CSmsNumber*	serviceCentreNumber = &(serviceSettings.SCAddress(
                                            serviceSettings.DefaultSC()));

        iSmsMtm->SmsHeader().SetServiceCenterAddressL(
                                               serviceCentreNumber->Address());

        #else
        scIndex = serviceSettings.DefaultServiceCenter();

        if ((scIndex < 0) || (scIndex >= numSCAddresses))
	        {
	        scIndex = 0; //???
	        }
        //get the service center number:
        TPtrC serviceCentreNumber =
              serviceSettings.GetServiceCenter(scIndex).Address();

        //This can only be used on SMS-SUBMIT type messages. The message PDU
        //type can be checked by using the CSmsHeader::Type API.
        iSmsMtm->SmsHeader().SetServiceCenterAddressL(serviceCentreNumber);
        #endif

        }
	else
		{
		#ifndef __WINS__
		// Panic if there is no service center number
		Panic(ESmsNoServiceSenterNumberSet);
		#endif
		}

    iSmsMtm->SaveMessageL();

    // Index entry must be Updated
    TMsvEntry indexEntry = iSmsMtm->Entry().Entry();

	// Set in-preparation flag
    indexEntry.SetInPreparation(EFalse);

	// Sets the sending state
    indexEntry.SetSendingState(KMsvSendStateWaiting);
    iSmsMtm->Entry().ChangeL(indexEntry);

    // Time to send the message
    Cancel(); // prepare iMsvOper for use
	iEntrySelection->Reset();
	iEntrySelection->AppendL(iSmsId);


    //The old way of sending (1st and 2nd edition)
    //TBuf8<1> dummyParam;
	// There is also InvokeSyncFunctionL which is synchronous.
	//iMsvOper = iSmsMtm->InvokeAsyncFunctionL(ESmsMtmCommandScheduleCopy,
	//                                  *iEntrySelection, dummyParam, iStatus);

    //a change for 3rd edition:
    CMsvEntry* entry = iSession->GetEntryL(KMsvDraftEntryId);
    TRAPD(err,
    entry->CopyL(iSmsMtm->Entry().EntryId(), iSmsMtm->ServiceId(), iStatus) );

    SetActive();
    }

// Move message to folder. Notice that the iSmsMtm points to the parent, not
// to the message itself. If you move messages from inbox to drafts, those
// messages cannot be edited because their complete flag is true.
void CSMSMtmsEngine::MoveToFolderL( TMsvId aMessageId,  TMsvId aFolder )
	{
	iSmsMtm->SwitchCurrentEntryL( aMessageId );
	TMsvSelectionOrdering selection;
	selection.SetShowInvisibleEntries(ETrue);
	CMsvEntry* parentEntry = CMsvEntry::NewL( iSmsMtm->Session(),
		iSmsMtm->Entry().Entry().Parent(), selection );
	CleanupStack::PushL(parentEntry);
	// Move the message
	parentEntry->MoveL( aMessageId, aFolder );
	CleanupStack::PopAndDestroy(parentEntry); // parentEntry
	}

// Delete message from a folder. Notice that the iSmsMtm points to the parent,
// not to the message itself.
void CSMSMtmsEngine::DeleteMessageL( TMsvId aMessageId )
	{
	iSmsMtm->SwitchCurrentEntryL( aMessageId );
	TMsvId parent = iSmsMtm->Entry().Entry().Parent();

	iSmsMtm->SwitchCurrentEntryL( parent );
	iSmsMtm->Entry().DeleteL( aMessageId );
	}

// Get originator address from SmsHeader.
void CSMSMtmsEngine::GetMessageAddressL( TMsvId aMessageId,
                                                TDes& aAddress )
	{
	iSmsMtm->SwitchCurrentEntryL( aMessageId );

	// Remember to load before using the SmsHeader
	iSmsMtm->LoadMessageL();

	CSmsHeader&	header = iSmsMtm->SmsHeader();

	aAddress.Append( header.FromAddress() );
	// Other possibility is this: (It's little bit faster than the previous one).
	// aAddress.Append( iSmsMtm->Entry().Entry().iDetails );
	}

// Get message body from entry storage (CMsvStore). CMsvStore stores whole
// message, not summary information ( see GetMessageIndexBodyTextL() ).
TBool CSMSMtmsEngine::GetMessageL( TMsvId aMessageId, TDes& aMessage)
	{
	iSmsMtm->SwitchCurrentEntryL( aMessageId );

	if ( iSmsMtm->Entry().HasStoreL() )
		{
		// SMS message is stored inside Messaging store.
		CMsvStore* store = iSmsMtm->Entry().ReadStoreL();
		CleanupStack::PushL(store);

		if (store->HasBodyTextL())
			{
			CRichText* richText = CRichText::NewL(
			                  CEikonEnv::Static()->SystemParaFormatLayerL(),
			                  CEikonEnv::Static()->SystemCharFormatLayerL());
			richText->Reset();
			CleanupStack::PushL(richText);

			// Get the SMS body text.
			store->RestoreBodyTextL(*richText);
			const TInt length = richText->DocumentLength();
			TBuf<KMessageBodySize> message;

			// Check length because message is read to limited size TBuf.
			if ( length >= KMessageBodySize )
				{
				message.Append( richText->Read(0, KMessageBodySize -1) );
				}
			else
				{
				message.Append( richText->Read(0, length) );
				}

			aMessage.Append( message );

			CleanupStack::PopAndDestroy(richText);
			}
		CleanupStack::PopAndDestroy(store);
		}
	else
		{
		return EFalse;
		}

	return ETrue;
	}

// Get beginning of message's body. Index entry is just a summary of the whole
// message.
TBool CSMSMtmsEngine::GetMessageIndexBodyTextL( TMsvId aMessageId,
                                                       TDes& aMessage)
	{
	iSmsMtm->SwitchCurrentEntryL( aMessageId );

	aMessage.Append(iSmsMtm->Entry().Entry().iDescription );
	return ETrue;
	}


// Copy message to another folder.
void CSMSMtmsEngine::CopyMessageL( TMsvId aMessageId, TMsvId aFolder )
	{
	iSmsMtm->SwitchCurrentEntryL( aMessageId );
	TMsvSelectionOrdering selection;
	selection.SetShowInvisibleEntries(ETrue);
	CMsvEntry* parentEntry = CMsvEntry::NewL( iSmsMtm->Session(),
		iSmsMtm->Entry().Entry().Parent(), selection );
	CleanupStack::PushL(parentEntry);
	// iSmsMtm points to the parent
	parentEntry->CopyL( aMessageId, aFolder );
	CleanupStack::PopAndDestroy(); // parentEntry
	}

// ids of messages that has been got using ListMessagesL
RArray<TMsvId>* CSMSMtmsEngine::GetMessageIds()
	{
	return iIdArray;
	}


// Get all folder's children which are SMS messages.
// Note that the folder can contain .sis files which have to be filtered out.
// IdArray is made here because it makes finding the SMS easier later on.
void CSMSMtmsEngine::GetFolderSMSMessageInformationL(
                                                TMsvId aFolderID,
												CDesCArrayFlat*& aAddresses,
												CDesCArrayFlat*& aMessages )
	{

	iSmsMtm->SwitchCurrentEntryL( aFolderID );
	CMsvEntry& entry = iSmsMtm->Entry();

	// Remember to delete this entry after no longer needed!
	// Only intrested in messages. Filter out service etries.
	CMsvEntrySelection* entries = entry.ChildrenWithMtmL(KUidMsgTypeSMS);

	CDesCArrayFlat* arrayAddr =
	                    new (ELeave) CDesCArrayFlat( KArrayGranularity );
	CDesCArrayFlat* arrayMsgBody =
	                    new (ELeave) CDesCArrayFlat( KArrayGranularity );

	iIdArray = new (ELeave) RArray<TMsvId>;

	for (TInt i = 0; i < entries->Count(); i++ )
		{
		TBuf<KMessageBodySize> body;
		TBuf<KMessageAddressLength> address;

		// Append only SMS messages, .sis files etc. are disregarded.
		// Take only beginning of SMS body, because listbox only shows
		// beginning of message.
		if ( GetMessageIndexBodyTextL( (*entries)[i], body ) ) // SMS body
			{
			iIdArray->Append( (*entries)[i] );
			arrayMsgBody->AppendL ( body );

			// Recipient address
			//If done an own messafe to the drafts with out a address then failes
			TRAPD(err, GetMessageAddressL( (*entries)[i], address ) );
			if( err == KErrNone)
			    arrayAddr->AppendL ( address );
			else
			    arrayAddr->AppendL ( KSenderJohnDoe );
			}
		}

	// Delete entries. This is your responsibility.
	entries->Reset();
	delete entries;
	entries = 0;

	aAddresses = arrayAddr; // address array
	aMessages = arrayMsgBody; // msg body array
	}

// Tells when the session has been opened
void CSMSMtmsEngine::HandleSessionEventL(TMsvSessionEvent aEvent,
                                                TAny* /*aArg1*/,
                                                TAny* /*aArg2*/,
                                                TAny* /*aArg3*/)
    {
    switch (aEvent)
        {
        // This event tells us that the session has been opened
        case EMsvServerReady:
            CreateMtmClientL();
            break;

        default:
            // do nothing
            break;
        }
    }
