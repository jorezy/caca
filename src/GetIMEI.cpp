/* Copyright (c) 2006, Nokia Mobile Phones. All rights reserved */

#include <eikenv.h>
#include "GetIMEI.h"

CGetIMEI* CGetIMEI::NewL(/*TDes& aIMEI*/)
{
	CGetIMEI* self = new (ELeave) CGetIMEI(/*aIMEI*/);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CGetIMEI::CGetIMEI() : CActive(EPriorityHigh) // HIGH priority
/*,iIMEI(aIMEI)*/
{
	iIMEI.Zero();
	iState = EStart;
}

void CGetIMEI::ConstructL()
{
	iTelephony = CTelephony::NewL();
	CActiveScheduler::Add(this); // Add to scheduler
}

CGetIMEI::~CGetIMEI()
{
	Cancel(); // Cancel any request, if outstanding
	// Delete instance variables if any
	delete iTelephony;
	iTelephony = NULL;
}

void CGetIMEI::DoCancel()
{
	
}

void CGetIMEI::StartL()
{
	Cancel(); // Cancel any request, just to be sure
	iState = EGetPhoneInfo;
	CTelephony::TPhoneIdV1Pckg phoneIdPckg( iPhoneId ); 
	iTelephony->GetPhoneId(iStatus, phoneIdPckg);
	SetActive(); // Tell scheduler a request is active
	iActiveSchedulerWait.Start();
}

void CGetIMEI::RunL()
{
	iState = EDone;
	if ( iActiveSchedulerWait.IsStarted() )
	{
		iActiveSchedulerWait.AsyncStop();
		if(iStatus == KErrNone)
		{
		iIMEI.Append(iPhoneId.iSerialNumber);
		//iAppUi->iIMEI.Append(iPhoneId.iSerialNumber);
		}
		else
		{
			TBuf<64> buf;
			buf.Num(iStatus.Int());
			_LIT(KE,"Get Phone Info error:");
			CEikonEnv::Static()->InfoWinL(KE(),buf);
		} 
	}
}

const TPtrC CGetIMEI::GetIMEI()
{
	StartL();
	TPtrC ptr(iIMEI.Ptr());
	return ptr;
}

// End of File

