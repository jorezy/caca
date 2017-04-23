#pragma once

#include "e32base.h"
#include "fbs.h"
#include "w32std.h"

class MSliderObserver
{
public:
	virtual void DoSliderOverEvent()=0;
};

class CSlider:public CBase
{
public:

	static CSlider* NewL(MSliderObserver* aObserver,CWindowGc& aGc,RDrawableWindow& aWindow);
	~CSlider();

	void SetBgMapL(const TDesC& aBgMap,TInt aIndex);
	void SetSliderMapL(const TDesC& aSliderMap,TInt aMicroSecond,TInt aStartIndex,TInt aEndIndex,TInt aRepeat=0);
	void SetSliderMapL(const TDesC& aSliderMap,const TDesC& aMaskMap,TInt aMicroSecond,TInt aStartIndex,TInt aEndIndex,TInt aRepeat);
	void SetPosition(TInt aX,TInt aY);
	void SetPosition(CArrayFix<TPoint>& aPosArray);
	void Start();
	void Stop();
	void SetShowTxt(bool aShowLoadTxt,bool aShowPercentTxt);

private:

	void ConstructL(CWindowGc& aGc,RDrawableWindow& aWindow);
	CSlider(MSliderObserver* aObserver);

	static TInt DoPeriodic(TAny* a);
	void RunPeriodic();
	void DrawCurrent();

	CPeriodic* iPeriodic;
	TInt iStartIndex;
	TInt iEndIndex;
	TInt iRepeat;
	TInt iX;
	TInt iY;
	TInt iMicroSecond;
	TInt iCurrentIndex;
	TInt iCurrentRepeat;
	TInt iArrayCount;
	TInt iCurrentArrayIndex;
	TInt iCurrentFrame;
	TInt iAllFrameCount;
	CFbsBitmap* iBgMap;
	CFbsBitmap** iMoveMaps;
	CFbsBitmap** iMaskMaps;
	CArrayFix<TPoint>* iPosArray;
	MSliderObserver* iObserver;
	CWindowGc* iGc;
	RDrawableWindow* iWindow;
	bool iShowLoadTxt;
	bool iShowPercentTxt;

};