// RichTextEngine.h: interface for the CRichTextEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHTEXTENGINE_H__7800E926_A0BA_4309_B18E_C3387D9C2EF2__INCLUDED_)
#define AFX_RICHTEXTENGINE_H__7800E926_A0BA_4309_B18E_C3387D9C2EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "eikrted.h"

class CRichTextEngine : public CEikRichTextEditor  
{
public:
	
	static CRichTextEngine* NewL(TInt aRichTextResource);
	CRichTextEngine();
	virtual ~CRichTextEngine();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

public: // members

	void ConstructL(TInt aRichTextResource);
	void SetUnderlineOn (TBool aOn);
	void SetBoldOn (TBool aOn);
	void AddCarriageReturnL ();
	void AddTextL (const TDesC& aText);
	void DisplayStartOfHelpTextL();
	void AddHeaderText(const TDesC& aHeader);
	void AddNormalText(const TDesC& aText);

private: //data

	TCharFormatMask iCharFormatMask;  // current formatting, e.g bold etc
	TCharFormat iCharFormat; // current formatting, e.g bold etc

};

#endif // !defined(AFX_RICHTEXTENGINE_H__7800E926_A0BA_4309_B18E_C3387D9C2EF2__INCLUDED_)
