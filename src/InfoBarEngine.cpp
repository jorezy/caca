#include "InfoBarEngine.h"
#include "eikenv.h"

#ifndef __DEL__
#define __DEL__
#define DEL(f) if(f) delete f; f=0;
#endif
#define VN(f) TBuf<10> bufNum(0);bufNum.AppendNum(f);CEikonEnv::Static()->AlertWin(bufNum);
#define VT(f) CEikonEnv::Static()->AlertWin(f);


CInfoBarEngine* CInfoBarEngine::NewL()
{
	CInfoBarEngine* self=new(ELeave) CInfoBarEngine;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CInfoBarEngine::~CInfoBarEngine()
{
	if(iPeriodic)
		StopRunInfo();
	DEL(iBgMap);
	DEL(iPeriodic);
}

void CInfoBarEngine::SetObserver(MRepaintObserver* aObserver)
{
	iObserver=aObserver;
}

void CInfoBarEngine::LoadMap(const TDesC& aMapName,TInt aIndex)
{
	DEL(iBgMap);
	iBgMap=new(ELeave) CFbsBitmap;
	iBgMap->Load(aMapName,aIndex);
}

void CInfoBarEngine::SetColor(TRgb aAllColor,TRgb aBgColor,TRgb aTxtColor)
{
	iAllColor=aAllColor;
	iBgColor=aBgColor;
	iTxtColor=aTxtColor;
}

void CInfoBarEngine::SetFont(const CFont* aFont)
{
	iFont=aFont;
	iInfoLength=iFont->TextWidthInPixels(iInfo);
}

void CInfoBarEngine::SetInfo(const TDesC& aInfo)
{
	iInfo.Copy(aInfo);
	iInfoLength=iFont->TextWidthInPixels(iInfo);
}

void CInfoBarEngine::Draw(CWindowGc& gc)
{
	gc.SetClippingRect(TRect(TPoint(iBarX,iBarY),TSize(iBarWidth,iBarHeight)));

	gc.SetBrushColor(iAllColor);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.DrawRect(TRect(TPoint(iBarX,iBarY),TSize(iBarWidth,iBarHeight)));

	if(iBgMap)
	{
		gc.BitBlt(TPoint(iBarX,iBarY),iBgMap);
	}
	else
	{
		gc.SetPenStyle(CGraphicsContext::ENullPen);
		gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc.SetBrushColor(iBgColor);
		gc.DrawRect(TRect(TPoint(iBarX,iBarY),TSize(iBarWidth,iBarHeight)));
	}

	DrawInfo(gc);
	gc.CancelClippingRect();
}

void CInfoBarEngine::DrawInfo(CWindowGc& gc)
{
	gc.SetClippingRect(TRect(TPoint(iInfoX,iInfoY),TSize(iInfoWidth,iBarHeight)));

	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(iTxtColor);
	gc.UseFont(iFont);
	int ty=iInfoY+iInfoTopGap+iFont->HeightInPixels();

	if(iPeriodic)
	{
		gc.DrawText(iInfo,TPoint(iCurrentInfoX+iInfoX,ty));
		if(iCurrentInfoX2!=0)
			gc.DrawText(iInfo,TPoint(iCurrentInfoX2+iInfoX,ty));
	}
	else
	{
		gc.DrawText(iInfo,TPoint(iInfoX,ty)); // needn't run scroll txt,it's static
	}

	gc.DiscardFont();
	gc.CancelClippingRect();
}

void CInfoBarEngine::SetBarPosition(TInt aX,TInt aY,TInt aWidth,TInt aHeight)
{
	iBarX=aX;
	iBarY=aY;
	iBarWidth=aWidth;
	iBarHeight=aHeight;
}

void CInfoBarEngine::SetInfoPosition(TInt aX,TInt aY,TInt aWidth)
{
	iInfoX=aX;
	iInfoY=aY;
	iInfoWidth=aWidth;
	iCurrentInfoX=iInfoWidth;
}

void CInfoBarEngine::SetInfoGap(TInt aTopGap,TInt aInfoBlank)
{
	iInfoTopGap=aTopGap;
	iInfoBlank-aInfoBlank;
}


void CInfoBarEngine::StartRunInfo()
{
	DEL(iPeriodic);
	iPeriodic=CPeriodic::NewL(EPriorityNormal);
	iPeriodic->Start(iTimeDelay,iTimePer,TCallBack(DoPeriodic,this));
}

void CInfoBarEngine::StopRunInfo()
{
	iPeriodic->Cancel();
}

TInt CInfoBarEngine::DoPeriodic(TAny* a)
{
	((CInfoBarEngine*)a)->RunPeriodic();
	return KErrNone;
}

void CInfoBarEngine::RunPeriodic()
{
	iCurrentInfoX-=iInfoIntever;

	if(-iCurrentInfoX>iInfoLength+iInfoBlank)
		iCurrentInfoX=0;

	int temp=(iInfoWidth-iCurrentInfoX)-(iInfoLength+iInfoBlank)-iInfoIntever;
	if(temp>0)
	{
		iCurrentInfoX2=iInfoWidth-temp+iInfoIntever;
	}
	else
	{
		iCurrentInfoX2=0;
	}

	iObserver->DoRepaint(TRect(TPoint(iBarX,iBarY),TSize(iBarWidth,iBarHeight))); // can be small,only txt rect
}

CInfoBarEngine::CInfoBarEngine()
{
}

void CInfoBarEngine::ConstructL()
{
	iInfo.Zero();
	iAllColor=KRgbWhite;
	iBgColor=KRgbBlack;
	iTxtColor=KRgbWhite;
	iFont=CEikonEnv::Static()->TitleFont();
	iBarX=0;
	iBarY=0;
	iBarWidth=CEikonEnv::Static()->ScreenDevice()->SizeInPixels().iWidth;
	iBarHeight=20;
	iInfoX=60;
	iInfoY=0;
	iInfoWidth=90;
	iInfoTopGap=2;
	iInfoBlank=30;
	iCurrentInfoX=iInfoWidth;
	iInfoIntever=2;
	iTimeDelay=200000;
	iTimePer=100000;
}

