#include "VmsList.h"
#include "eikenv.h"
#include <AknIconArray.h> //avkon.lib eikcore.lib
#include "gulicon.h" //egul.lib
#include "ScaleEngine.h"

#define DEL(f);  delete f; f=NULL;
const TInt KIconWidth=20;
const TInt KIconHeight=20;
#define KIconSize TSize(KIconWidth,KIconHeight)


CVmsList* CVmsList::NewL(CCoeControl* aParent,MVmsListKeyObserver* aObserver)
{
	CVmsList* self=new(ELeave) CVmsList(aParent,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CVmsList::~CVmsList()
{
	DEL(iBgMap);

	DEL(iHeadMaps);

	if(iTxtArray)
		iTxtArray->Reset();
	DEL(iTxtArray);

	if(iHeadArray)
		iHeadArray->Reset();
	DEL(iHeadArray);
}

TKeyResponse CVmsList::DoKeyEventL(const TKeyEvent& aEvent,TEventCode aCode)  // need add // add turn page
{
 	if(aCode!=EEventKey)
		return EKeyWasNotConsumed;

	switch(aEvent.iScanCode)
	{
		case EStdKeyDevice3:
		{
			iObserver->DoVmsListKeyEventL();
			break;
 		}
		case EStdKeyUpArrow:
		{
			if(iCurrentIndex>0)
			{
				iCurrentIndex--;
				if(iY-iRealY>iCurrentIndex*iLineHeight)//iLineHeight<0)
				{
					iRealY+=iLineHeight;
					iObserver->DoReDraw(iParent->Rect());
				}
				else
				{
					TRect rect(TPoint(iX,iRealY+iCurrentIndex*iLineHeight),TSize(iWidth,iLineHeight<<1));
					iObserver->DoReDraw(rect);
					//iObserver->DoReDraw(iParent->Rect());
				}
			}
			break;
		}
		case EStdKeyDownArrow:
		{
			if(iCurrentIndex<iCount-1)
			{
				iCurrentIndex++;
				int k=(iHeight-iY)/iLineHeight;
				if(iCurrentIndex>k)
				{
					iRealY=iY-iLineHeight*(iCurrentIndex-k);
					iObserver->DoReDraw(iParent->Rect());
				}
				else
				{
					TRect rect(TPoint(iX,iY+(iCurrentIndex-1)*iLineHeight),TSize(iWidth,iLineHeight<<1));
					iObserver->DoReDraw(rect);
				}
			}
			break;
		}
	}
	return EKeyWasNotConsumed;
}

// need add two row // need add current pos flag view
void CVmsList::VDraw(CWindowGc& gc,const CFont* aFont1,const CFont* aFont2,TBool aNeedDrawHead/*=true*/)const
{
	gc.SetClippingRect(TRect(TPoint(iX,iY),TSize(iWidth,iHeight)));

	gc.SetBrushColor(iBgColor);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.DrawRect(TRect(TPoint(iX,iY),TSize(iWidth,iHeight)));

	if(iBgMap)
		gc.BitBlt(TPoint(iX,iY),iBgMap);

	TInt tx=iRealX+iTxtGap;
	TInt ty=iRealY;
	//TInt systemWidth=CEikonEnv::Static()->ScreenDevice()->SizeInPixels().iWidth;
	TInt hx;
	TInt w1;
	TInt h1;
	TInt cy1;
	TInt cy2;

	for(TInt m=0;m<iCount;m++)
	{
		cy1=ty+m*iLineHeight;//=ty+m*(iLineHeight+iLineGap);

		if(m!=iCurrentIndex)
		{
			w1=aFont1->TextWidthInPixels(iTxtArray->MdcaPoint(m));
			h1=aFont1->HeightInPixels();

			gc.UseFont(aFont1);
			gc.SetPenColor(iTxtColor1);
		}
		else
		{
			gc.SetPenStyle(CGraphicsContext::ENullPen);
			gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
			gc.SetBrushColor(iBlockColor);
			gc.DrawRect(TRect(TPoint(iX,cy1),TSize(iWidth,iLineHeight)));

			w1=aFont2->TextWidthInPixels(iTxtArray->MdcaPoint(m));
			h1=aFont2->HeightInPixels();

			gc.UseFont(aFont2);
			gc.SetPenColor(iTxtColor2);
		}

		if(aNeedDrawHead)
		{
			if(iHeadMaps)
			{
				hx=iX+iHeadMapGap;
				int ih=iHeadArray->At(m);
				if(ih>iHeadMapCount-1)
					ih=iHeadMapCount-1;

				CFbsBitmap* map=iHeadMaps->At(ih)->Bitmap();
				CFbsBitmap* mask=iHeadMaps->At(ih)->Mask();
				TSize size=map->SizeInPixels();
				TPoint pt(hx,cy1);
				//TRect rect(TPoint(),size);
				tx=iX+iTxtGap+size.iWidth+iHeadMapGap;

				gc.BitBltMasked(pt,map,TRect(TPoint(),size),mask,true);
			}
			else
			{
/*				hx=iX+iHeadNumGap;
				tx+=iHeadMaps[0]->SizeInPixels().iWidth+iHeadNumGap;
				TBuf<3> numBuf;
				numBuf.AppendNum(iHeadArray[m]);
				gc.DrawText(buf,TPoint(tx,cy2));
*/			}
		}

		cy2=cy1+iLineHeight-(iLineHeight-h1)/2;
		gc.SetBrushStyle(CGraphicsContext::ENullBrush);
		gc.SetPenStyle(CGraphicsContext::ESolidPen);
		TBuf<50> buf(iTxtArray->MdcaPoint(m));
		gc.DrawText(buf,TPoint(tx,cy2));
	}

	gc.CancelClippingRect();
}

void CVmsList::SetPos(TInt aX,TInt aY)
{
	iX=aX;
	iY=aY;
}

void CVmsList::SetBgMap(const TDesC& aFileName,TInt aIndex)
{
	DEL(iBgMap);
	iBgMap=new(ELeave) CFbsBitmap;
	iBgMap->Load(aFileName,aIndex);

	CScaleEngine* oEngine=CScaleEngine::NewL();
	CFbsBitmap* iTempMap=oEngine->ScaleMap(*iBgMap,TSize(iWidth,iHeight),false);
	DEL(iBgMap);
	iBgMap=iTempMap;
	delete oEngine;
/**/
}

void CVmsList::SetHeadMaps(const TDesC& aFileName,TInt aFirst,TInt aEnd,TInt aMaskFirst,TInt aMaskEnd)
{
	int count=aEnd-aFirst+1;
	if(aFirst<0 || aEnd<0 || count<0)
		User::Leave(-6);
	if(aMaskFirst<0 || aMaskEnd<0 || aMaskEnd<aMaskFirst)
		User::Leave(-6);

	iHeadMapCount=count;
	if(iHeadMaps)
		DEL(iHeadMaps);

	iHeadMaps=new(ELeave) CAknIconArray(iHeadMapCount);
	CEikonEnv* iEnv=CEikonEnv::Static();
	CScaleEngine* oEngine=CScaleEngine::NewL();
	for(int k=0;k<iHeadMapCount;k++)
	{
		int maskIndex;
		maskIndex=aMaskFirst+k;
		if(aMaskFirst+k>aMaskEnd)
			maskIndex=aMaskFirst;

		CGulIcon* icon=iEnv->CreateIconL(aFileName,aFirst+k,maskIndex);

		CFbsBitmap* map=oEngine->ScaleMap(*(icon->Bitmap()),KIconSize,false);
		CFbsBitmap* mask=oEngine->ScaleMap(*(icon->Mask()),KIconSize,false);
		delete icon;
		icon=CGulIcon::NewL(map,mask);

		iHeadMaps->AppendL(icon);
	}
	delete oEngine;
}

/*
void CVmsList::SetHeadMaps(const CDesCArray& aDesCArray,const CArrayFix<TInt>& aIndexArray)
{
	int count=aDesCArray.Count();
	if(count!=aIndexArray.Count())
		User::Leave(-6);

	iHeadMapCount=count;
	if(iHeadMaps)
	{
		for(int k=0;k<iHeadMapCount;k++)
		{
			DEL(iHeadMaps[k]);
		}
		DEL(iHeadMaps);
	}

	iHeadMaps=new(ELeave) CFbsBitmap*[count];
	for(int k=0;k<count;k++)
	{
		iHeadMaps[k]=new(ELeave) CFbsBitmap;
		iHeadMaps[k]->Load(aDesCArray.MdcaPoint(k),aIndexArray[k]);
	}
}
*/

void CVmsList::SetContent(const CDesCArray& aDesCArray,bool aNeedClear,TBool aDefaultOrder/*=false*/)
{
	int count=aDesCArray.Count();
	if(count<=0)
	{
		CEikonEnv::Static()->AlertWin(_L("Error"),_L("you have none friend!"));  // test //
		User::Panic(_L("No friend"),-6);
	}

	if(!iTxtArray)
		iTxtArray=new(ELeave) CDesCArrayFlat(count);
	if(!iHeadArray)
		iHeadArray=new(ELeave) CArrayFixFlat<TInt>(count);

	if(aNeedClear)
	{
		iTxtArray->Reset();
		iHeadArray->Reset();
	}

	if(aDefaultOrder)
	{
		for(int m=0;m<count;m++)
		{
			iTxtArray->AppendL(aDesCArray.MdcaPoint(m));
			iHeadArray->AppendL(m);
		}
	}
	else
	{
		for(int m=0;m<count;m++)
		{
			TBuf<200> buf(aDesCArray.MdcaPoint(m));
			int pos=buf.Find(_L("\t"));
			if(pos<0)
				User::Leave(-6);

			iTxtArray->AppendL(buf.Mid(pos+1));

			TLex16 lex(buf.Mid(0,pos));
			TInt num;
			int re=lex.Val(num);
			if(re==KErrNone)
				iHeadArray->AppendL(num);
		}
	}
	iCount=count;
}

void CVmsList::SetContent(const CDesCArray& aDesCArray,const CArrayFix<TInt>& aIndexArray,bool aNeedClear)
{
	int count=aDesCArray.Count();
	int count2=aIndexArray.Count();
	if(count<=0 || count!=count2)
		User::Leave(-6);

	if(!iTxtArray)
		iTxtArray=new(ELeave) CDesCArrayFlat(count);
	if(!iHeadArray)
		iHeadArray=new(ELeave) CArrayFixFlat<TInt>(count);

	if(aNeedClear)
	{
		iTxtArray->Reset();
		iHeadArray->Reset();
	}

	for(int m=0;m<count;m++)
	{
		iTxtArray->AppendL(aDesCArray.MdcaPoint(m));
		iHeadArray->AppendL(aIndexArray[m]);
	}
	iCount=count;
}

void CVmsList::SetBgColor(TRgb aBgColor)
{
	iBgColor=aBgColor;
}

void CVmsList::SetTxtColor(TRgb aTxtColor1,TRgb aTxtColor2/*=KRgbRed*/)
{
	iTxtColor1=aTxtColor1;
	iTxtColor2=aTxtColor2;
}

void CVmsList::SetBlockColor(TRgb aBlockColor)
{
	iBlockColor=aBlockColor;
}

void CVmsList::SetSize(TInt aWidth,TInt aHeight)
{
	iWidth=aWidth;
	iHeight=aHeight;
}

void CVmsList::SetLineSize(TInt aLineHeight,TInt aLineGap)
{
	iLineHeight=aLineHeight;
	iLineGap=aLineGap;
}

TInt CVmsList::Count()
{
	return iCount;
}

TInt CVmsList::CurrentIndex()
{
	return iCurrentIndex;
}

void CVmsList::SetCurrent(TInt aIndex)
{
	iCurrentIndex=aIndex;
}

CDesCArray* CVmsList::TxtArray()
{
	return iTxtArray;
}

CVmsList::CVmsList(CCoeControl* aParent,MVmsListKeyObserver* aObserver)
{
	iParent=aParent;
	iObserver=aObserver;
}

void CVmsList::ConstructL()
{
	iRealX=iX=20;
	iRealY=iY=20;
	iBgColor=KRgbWhite;
	iTxtColor1=KRgbBlack;
	iTxtColor2=KRgbRed;
	iBlockColor=TRgb(200,200,200);
	iWidth=120;
	iHeight=160;
	iLineHeight=20;
	iLineGap=4;
	iTxtGap=4;
	iHeadMapGap=3;
	iHeadMapWidth=15;
	iHeadMapHeight=15;
	iHeadNumGap=3;
}

