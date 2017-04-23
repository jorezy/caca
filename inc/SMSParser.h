/*
* ============================================================================
*  Name     : CSMSParser from SMSParser.h
*  Part of  : SMS
*  Created  : 08.02.2005 by Forum Nokia
*  Description:
*     Parser that searches for a number from SMS message body.
*  Version  : 1.0
*  Copyright: Nokia Corporation
* ============================================================================
*/

#ifndef SMSPARSER_H
#define SMSPARSER_H

// System includes 
#include <e32base.h> // CBase

/**
* Parser
*/
class CSMSParser: public CBase
	{
	public:

		/**
		* Create new CSMSParser object
		* 
		* @return a pointer to the created instance of CSMSParser.
		*/
		static CSMSParser* NewL();

		/**
		* Destructor.
		*/
	    ~CSMSParser(void);

		/**
		* Parse body and try to find a number.
		* @param aMessage SMS message body.
		* @param aNumber Found number. Searches for exactly 10 digits that are one after 
		*                another(no whitespaces).
		* @return ETrue if found, EFalse otherwise.
		*/
	    TBool ParseMessage(const TDesC& aMessage, TDes& aNumber);
      
	private:

		/**
		* ConstructL()
		*/
        void ConstructL();

	private:

		/**
		* A constructor.
		*/
		CSMSParser(void);
		
	};

#endif 