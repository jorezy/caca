#pragma once 

#include "e32base.h"
#include "fbs.h"
#include "w32std.h"

class MRepaintObserver
{
public:
	virtual void DoRepaint(const TRect& aRect)=0;
};

class CInfoBarEngine:public CBase
{
public:

	static CInfoBarEngine* NewL();
	~CInfoBarEngine();

	void SetObserver(MRepaintObserver* aObserver);
	void LoadMap(const TDesC& aMapName,TInt aIndex);
	void SetColor(TRgb aAllColor,TRgb aBgColor,TRgb aTxtColor);
	void SetFont(const CFont* aFont);
	void SetInfo(const TDesC& aInfo);
	void Draw(CWindowGc& gc);
	void DrawInfo(CWindowGc& gc);
	void SetBarPosition(TInt aX,TInt aY,TInt aWidth,TInt aHeight);
	void SetInfoPosition(TInt aX,TInt aY,TInt aWidth);
	void SetInfoGap(TInt aTopGap,TInt aInfoBlank);
	void StartRunInfo();
	void StopRunInfo();
	static TInt DoPeriodic(TAny* a);
	void RunPeriodic();

private:

	CInfoBarEngine();
	void ConstructL();

	MRepaintObserver* iObserver;
	TBuf<200> iInfo;
	TRgb iAllColor;
	TRgb iBgColor;
	TRgb iTxtColor;
	const CFont* iFont;
	CFbsBitmap* iBgMap;
	TInt iBarX;
	TInt iBarY;
	TInt iBarWidth;
	TInt iBarHeight;
	TInt iInfoX;
	TInt iInfoY;
	TInt iInfoWidth;
	TInt iInfoTopGap;
	TInt iInfoBlank;
	CPeriodic* iPeriodic;
	TInt iTimeDelay;
	TInt iTimePer;
	TInt iCurrentInfoX;
	TInt iCurrentInfoX2;
	TInt iInfoLength;
	TInt iInfoIntever;

};