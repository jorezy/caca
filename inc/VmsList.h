#pragma once

#include "e32base.h"
#include "coecntrl.h"
#include "fbs.h"
#include "gdi.h"
#include "badesca.h"


class CGulIcon;

class MVmsListKeyObserver
{
public:
	virtual void DoVmsListKeyEventL()=0;
	virtual void DoReDraw(const TRect& aRect)=0;
};

class CVmsList: public CBase
{
public:

	static CVmsList* NewL(CCoeControl* aParent,MVmsListKeyObserver* aObserver);
	~CVmsList();
	TKeyResponse DoKeyEventL(const TKeyEvent& aEvent,TEventCode aCode);

	void VDraw(CWindowGc& gc,const CFont* aFont1,const CFont* aFont2,TBool aNeedDrawHead=true)const;
	void SetPos(TInt aX,TInt aY);
	void SetBgMap(const TDesC& aFileName,TInt aIndex);
	void SetHeadMaps(const TDesC& aFileName,TInt aFirst,TInt aEnd,TInt aMaskFirst,TInt aMaskEnd);
//	void SetHeadMaps(const CDesCArray& aDesCArray,const CArrayFix<TInt>& aIndexArray);
	void SetContent(const CDesCArray& aDesCArray,bool aNeedClear=true,TBool aDefaultOrder=false);
	void SetContent(const CDesCArray& aDesCArray,const CArrayFix<TInt>& aIndexArray,bool aNeedClear);
	void SetBgColor(TRgb aBgColor);
	void SetTxtColor(TRgb aTxtColor1,TRgb aTxtColor2=KRgbRed);
	void SetBlockColor(TRgb aBlockColor);
	void SetSize(TInt aWidth,TInt aHeight);
	void SetLineSize(TInt aLineHeight,TInt aLineGap);
	TInt Count();
	TInt CurrentIndex();
	void SetCurrent(TInt aIndex);
	CDesCArray* TxtArray();

private:

	CVmsList(CCoeControl* aParent,MVmsListKeyObserver* aObserver);
	void ConstructL();

	CCoeControl* iParent;
	MVmsListKeyObserver* iObserver;

	TInt iX;
	TInt iY;
	TInt iRealX;
	TInt iRealY;
	CFbsBitmap* iBgMap;
	CArrayPtr<CGulIcon>* iHeadMaps;
	TInt iHeadMapCount;
	CDesCArray* iTxtArray;
	CArrayFix<TInt>* iHeadArray;
	TRgb iBgColor;
	TRgb iTxtColor1;
	TRgb iTxtColor2;
	TRgb iBlockColor;
	TInt iWidth;
	TInt iHeight;
	TInt iLineHeight;
	TInt iLineGap;
	TInt iCount;
	TInt iCurrentIndex;
	TInt iTxtGap;
	TInt iHeadMapGap;
	TInt iHeadMapWidth;
	TInt iHeadMapHeight;
	TInt iHeadNumGap;

};