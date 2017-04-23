#pragma once

#include "e32base.h"
//#include "MdaImageConverter.h"  //CMdaBitmapScaler  MediaClientImage.lib
#include "BitmapTransforms.h"  //CBitmapScaler  BitmapTransforms.lib


class CScaleEngine:public CActive 
{
public:

	static CScaleEngine* NewL();
	~CScaleEngine();

	void ScaleMap(CFbsBitmap& aSourceMap,CFbsBitmap& aResultMap,TBool aMaintainAspectRatio=ETrue);
	CFbsBitmap* ScaleMap(CFbsBitmap& aSourceMap,TSize aResultSize,TBool aMaintainAspectRatio=ETrue);

private:

	CScaleEngine();
	void ConstructL();

	void RunL();
	void DoCancel();
	void Start(CFbsBitmap& aSourceMap,CFbsBitmap& aResultMap,TBool aMaintainAspectRatio=ETrue);

	CBitmapScaler* iScaler;
	CActiveSchedulerWait iActiveSchedulerWait;
	TInt iCurrent;

};