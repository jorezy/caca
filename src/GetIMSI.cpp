/* Copyright (c) 2006, Nokia Mobile Phones. All rights reserved */

#include "GetIMSI.h"
#include <eikenv.h>

CGetIMSI* CGetIMSI::NewL(/*TDes& aIMSI*/)
{
	CGetIMSI* self = new (ELeave) CGetIMSI(/*aIMSI*/);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CGetIMSI::CGetIMSI() : CActive(EPriorityHigh) // HIGH priority
/*,iIMSI(aIMSI)*/
{
	iIMSI.Zero();
	iState = EStart;
}

void CGetIMSI::ConstructL()
{
	iTelephony = CTelephony::NewL();
	CActiveScheduler::Add(this); // Add to scheduler
}

CGetIMSI::~CGetIMSI()
{
	Cancel(); // Cancel any request, if outstanding
	// Delete instance variables if any
	delete iTelephony;
	iTelephony = NULL;
}

void CGetIMSI::DoCancel()
{
	
}

void CGetIMSI::StartL()
{
	Cancel(); // Cancel any request, just to be sure
	iState = EGetSubscrIdPckg;
	CTelephony::TSubscriberIdV1Pckg subscrIdPckg(iSubscrId);
	iTelephony->GetSubscriberId(iStatus, subscrIdPckg);

	SetActive(); // Tell scheduler a request is active
	iActiveSchedulerWait.Start();
}

void CGetIMSI::RunL()
{
	iState = EDone;
	if ( iActiveSchedulerWait.IsStarted() )
	{
		iActiveSchedulerWait.AsyncStop();
		if(iStatus == KErrNone)
		{
		iIMSI.Append(iSubscrId.iSubscriberId);
		//iAppUi->iIMSI.Append(iSubscrId.iSubscriberId);
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

const TPtrC CGetIMSI::GetIMSI()
{
	StartL();
	TPtrC ptr(iIMSI.Ptr());
	return ptr;
}

