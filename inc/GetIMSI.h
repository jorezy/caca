/* Copyright (c) 2006, Nokia Mobile Phones. All rights reserved */

#ifndef __GETIMSI_H__
#define __GETIMSI_H__

#include <e32base.h>
#include <etel3rdparty.h>

class CGetIMSI : public CActive
{
public:
	static CGetIMSI* NewL(/*TDes& IMSI*/);

// Cancel and destroy
	~CGetIMSI();

public: // New functions
// Function for making the initial request
	void StartL();

	const TPtrC GetIMSI();

private: 
// C++ constructor
	CGetIMSI(/*TDes& aIMEI*/); 
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
	enum TGetIMSIState
	{
	EStart = 1,
	EGetSubscrIdPckg,
	EDone
	};

	private:
	TInt iState; // State of the active object
	CTelephony* iTelephony;
	CTelephony::TSubscriberIdV1 iSubscrId;
	CActiveSchedulerWait iActiveSchedulerWait;
	TBuf<CTelephony::KPhoneSerialNumberSize> iIMSI;
	//CGetUNAppUi* iAppUi;
};

#endif // __GETIMSI_H__

