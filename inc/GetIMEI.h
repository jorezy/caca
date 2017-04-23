/* Copyright (c) 2006, Nokia Mobile Phones. All rights reserved */

#ifndef __GETIMEI_H__
#define __GETIMEI_H__

#include <e32base.h>
#include <etel3rdparty.h>

class CGetIMEI : public CActive
{
public:
	static CGetIMEI* NewL(/*TDes& aIMEI*/);

// Cancel and destroy
	~CGetIMEI();

public: // New functions
// Function for making the initial request
	void StartL();

	const TPtrC GetIMEI();

private: 
// C++ constructor
	CGetIMEI(/*TDes& aIMEI*/); 
// Second-phase constructor
void ConstructL();

// From CActive
// Handle completion
	void RunL();

// How to cancel me
	void DoCancel();

// Override to handle leaves from RunL(). Default implementation causes
// the active scheduler to panic.
//void RunError(TInt aError);

private:
	enum TGetIMEIState
	{
	EStart = 1,
	EGetPhoneInfo,
	EDone
	};

	private:
	TInt iState; // State of the active object
	CTelephony* iTelephony;
	CTelephony::TPhoneIdV1 iPhoneId;
	CActiveSchedulerWait iActiveSchedulerWait;
	TBuf<CTelephony::KPhoneSerialNumberSize> iIMEI;
	//CGetUNAppUi* iAppUi;
};


#endif // __GETIMEI_H__

