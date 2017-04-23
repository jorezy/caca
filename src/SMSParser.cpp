/*
* ============================================================================
*  Name     : CSMSParser from SMSParser.h
*  Part of  : SMS
*  Created  : 12.03.2005 by Forum Nokia
*  Version  : 1.0
*  Copyright: Nokia Corporation
* ============================================================================
*/

#include "SMSParser.h"

const TInt KNumberLength = 10;

CSMSParser::CSMSParser(void)
	{
	}
 
CSMSParser::~CSMSParser(void)
	{
	}

CSMSParser* CSMSParser::NewL()
	{
	CSMSParser* self = new (ELeave) CSMSParser();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}
	  
void CSMSParser::ConstructL()
	{
	}

TBool CSMSParser::ParseMessage(const TDesC& aMessage, TDes& aNumber)
	{
	TLex numberLex = TLex( aMessage );

	// while not end-of-file
	while ( !numberLex.Eos() )
		{
		TChar character = numberLex.Get();

		if ( character.IsDigit() ) 
			{
			aNumber.Append( character );
			}
		else 
			{
			aNumber.Delete( 0, aNumber.Length() );
			}

		// Search for a number which is in format XXXXXXXXXX ( X = [0-9], 
		// whitespaces are not allowed). 
		// For  "0551234567" would return true. 
		// but "055 1234567" would not
		if ( aNumber.Length() == KNumberLength ) 
			{
			return ETrue;
			}
		}

	return EFalse;
	}
