#include "ScaleEngine.h"
#include "eikenv.h"


CScaleEngine* CScaleEngine::NewL()
{
	CScaleEngine* self=new(ELeave) CScaleEngine;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CScaleEngine::~CScaleEngine()
{
	delete iScaler;
}

void CScaleEngine::ScaleMap(CFbsBitmap& aSourceMap,CFbsBitmap& aResultMap,TInt aMaintainAspectRatio/*=ETrue*/)
{
	Start(aSourceMap,aResultMap,aMaintainAspectRatio);
	iActiveSchedulerWait.Start();
}

CFbsBitmap* CScaleEngine::ScaleMap(CFbsBitmap& aSourceMap,TSize aResultSize,TBool aMaintainAspectRatio/*=ETrue*/)
{
	CFbsBitmap* resultMap=new(ELeave) CFbsBitmap;
	resultMap->Create(aResultSize,aSourceMap.DisplayMode());
	ScaleMap(aSourceMap,*resultMap,aMaintainAspectRatio);
	return resultMap;
}

CScaleEngine::CScaleEngine():CActive(EPriorityNormal)
{
}

void CScaleEngine::ConstructL()
{
	CActiveScheduler::Add(this);
	iScaler=CBitmapScaler::NewL();
	iCurrent=0;
}

void CScaleEngine::RunL()
{
	if(iCurrent==0)
	{
		if(iActiveSchedulerWait.IsStarted())
			iActiveSchedulerWait.AsyncStop();
	}
}

void CScaleEngine::DoCancel()
{
}

void CScaleEngine::Start(CFbsBitmap& aSourceMap,CFbsBitmap& aResultMap,TInt aMaintainAspectRatio/*=ETrue*/)
{
	iScaler->Scale(&iStatus,aSourceMap,aResultMap,aMaintainAspectRatio);
	SetActive();
//	if(iCurrent>0)
//		iCurrent--;
}
