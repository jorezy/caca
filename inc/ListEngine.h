// ListEngine.h: interface for the CListEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTENGINE_H__2C1ECE01_12BD_4744_A5FE_55F9FB75919A__INCLUDED_)
#define AFX_LISTENGINE_H__2C1ECE01_12BD_4744_A5FE_55F9FB75919A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <aknlists.h> // CAknSingleStyleListBox

class CListEngine : public CAknSingleGraphicStyleListBox//CAknSingleStyleListBox  
{
public:

	void ConstructL(TInt aListResource);
	void ConstructL(const CDesCArray& aDesCArray,bool aNeedClear);
	void BuildIconsL(TInt aResource,const TDesC& aFileName,const CArrayFix<TInt>* aMapArray,const CArrayFix<TInt>* aMaskArray=NULL);
	virtual ~CListEngine();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent,TEventCode aCode);

private:

 	void SetupScrollBarsL();
   	void SizeChanged();
   	void Draw(const TRect& aRect)const;
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;

public:
//	bool iFocus;

};

#endif // !defined(AFX_LISTENGINE_H__2C1ECE01_12BD_4744_A5FE_55F9FB75919A__INCLUDED_)
