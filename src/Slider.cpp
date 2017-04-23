#include "Slider.h"
#include "eikenv.h"


CSlider* CSlider::NewL(MSliderObserver* aObserver,CWindowGc& aGc,RDrawableWindow& aWindow)
{
	CSlider* self=new(ELeave) CSlider(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aGc,aWindow);
	CleanupStack::Pop(self);
	return self;
}

CSlider::~CSlider()
{
	delete iPeriodic;
}

void CSlider::SetBgMapL(const TDesC& aBgMap,TInt aIndex)
{
	iBgMap=new(ELeave) CFbsBitmap;
	iBgMap->Load(aBgMap,aIndex);
}

void CSlider::SetSliderMapL(const TDesC& aSliderMap,TInt aMicroSecond,TInt aStartIndex,TInt aEndIndex,TInt aRepeat/*=0*/)
{
	if(aStartIndex<0 || aEndIndex<0 || aStartIndex>aEndIndex || aRepeat<=0)
		User::Leave(-6);

	iStartIndex=aStartIndex;
	iEndIndex=aEndIndex;
	iRepeat=aRepeat;
	iMicroSecond=aMicroSecond;
	iCurrentIndex=aStartIndex;
	int num=aEndIndex-aStartIndex;
	iAllFrameCount=num*aRepeat;

	iMoveMaps=new(ELeave) CFbsBitmap*[num];
	for(int m=0;m<num;m++)
	{
		iMoveMaps[m]=new(ELeave) CFbsBitmap;
		iMoveMaps[m]->Load(aSliderMap,aStartIndex+m);
	}
}

void CSlider::SetSliderMapL(const TDesC& aSliderMap,const TDesC& aMaskMap,TInt aMicroSecond,TInt aStartIndex,TInt aEndIndex,TInt aRepeat/*=0*/)
{
	if(aStartIndex<=0 || aEndIndex<=0 || aStartIndex>aEndIndex || aRepeat<0)
		User::Leave(-6);

	iStartIndex=aStartIndex;
	iEndIndex=aEndIndex;
	iRepeat=aRepeat;
	iMicroSecond=aMicroSecond;
	iCurrentIndex=aStartIndex;
	int num=aEndIndex-aStartIndex+1;
	iAllFrameCount=num*(aRepeat+1);

	iMoveMaps=new(ELeave) CFbsBitmap*[num];
	for(int m=0;m<num;m++)
	{
		iMoveMaps[m]=new(ELeave) CFbsBitmap;
		iMoveMaps[m]->Load(aSliderMap,aStartIndex+m);
	}
	iMaskMaps=new(ELeave) CFbsBitmap*[num];
	for(int m=0;m<num;m++)
	{
		iMaskMaps[m]=new(ELeave) CFbsBitmap;
		iMaskMaps[m]->Load(aMaskMap,aStartIndex+m);
	}
}

void CSlider::SetPosition(TInt aX,TInt aY)
{
	iX=aX;
	iY=aY;
}

void CSlider::SetPosition(CArrayFix<TPoint>& aPosArray)
{
	iPosArray=&aPosArray;
	iArrayCount=aPosArray.Count();
}

void CSlider::Start()
{
	iPeriodic->Cancel();
	iPeriodic->Start(0,iMicroSecond,TCallBack(DoPeriodic,this));
}

void CSlider::Stop()
{
	iPeriodic->Cancel();
	delete iBgMap;
	int count=iEndIndex-iStartIndex;
	for(int m=0;m<count;m++) // can't use delete[] iMoveMaps;
	{
		delete iMoveMaps[m];

		if(iMaskMaps)
			delete iMaskMaps[m];
	}
	delete iMoveMaps;
	delete iMaskMaps;
}

void CSlider::SetShowTxt(bool aShowLoadTxt,bool aShowPercentTxt)
{
	iShowLoadTxt=aShowLoadTxt;
	iShowPercentTxt=aShowPercentTxt;
}

void CSlider::ConstructL(CWindowGc& aGc,RDrawableWindow& aWindow)
{
	iGc=&aGc;
	iWindow=&aWindow;
	iPeriodic=CPeriodic::NewL(EPriorityNormal);
	iShowLoadTxt=true;
	iShowPercentTxt=true;
}

CSlider::CSlider(MSliderObserver* aObserver):iObserver(aObserver)
{
}

TInt CSlider::DoPeriodic(TAny* a)
{
	((CSlider*)a)->RunPeriodic();
	return KErrNone;
}

void CSlider::RunPeriodic()
{
	iCurrentArrayIndex++;
	if(iCurrentArrayIndex>=iArrayCount)
		iCurrentArrayIndex=0;
	
	iCurrentIndex++;
	if(iCurrentIndex>iEndIndex)
	{
		iCurrentIndex=0;
		iCurrentRepeat++;
	}

	if(iCurrentRepeat>=iRepeat)
	{
		iPeriodic->Cancel();
		iObserver->DoSliderOverEvent();
		iCurrentRepeat=0;
		return;
	}

	iCurrentFrame++;
	DrawCurrent();
}

void CSlider::DrawCurrent()
{
	iGc->Activate(*iWindow);

	iGc->Clear();
	iGc->BitBlt(TPoint(),iBgMap);
	if(iPosArray)
	{
		if(iMaskMaps)
			iGc->BitBltMasked(iPosArray->At(iCurrentArrayIndex),iMoveMaps[iCurrentIndex],TRect(0,0,100,100),iMaskMaps[iCurrentIndex],true);
		else
			iGc->BitBlt(iPosArray->At(iCurrentArrayIndex),iMoveMaps[iCurrentIndex]);
	}
	else
	{
		if(iMaskMaps)
			iGc->BitBltMasked(TPoint(iX,iY),iMoveMaps[iCurrentIndex],TRect(0,0,100,100),iMaskMaps[iCurrentIndex],true);
		else
			iGc->BitBlt(TPoint(iX,iY),iMoveMaps[iCurrentIndex]);
	}

	if(iShowLoadTxt)
	{
		const CFont* font=CEikonEnv::Static()->AnnotationFont();
		iGc->UseFont(font);
		iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		iGc->SetPenColor(KRgbWhite);
		iGc->SetPenStyle(CGraphicsContext::ESolidPen);
		TBuf<20> buf(_L("Loading...    "));
		if(iShowPercentTxt)
		{
			int kk=iCurrentFrame*100/iAllFrameCount;
			if(kk<10)
				buf.Append(_L("0"));
			buf.AppendNum(kk);
			buf.Append(_L("%"));
		}
		iGc->DrawText(buf,TPoint(50,160));
		iGc->DiscardFont();

		iGc->Deactivate();
	}
}

