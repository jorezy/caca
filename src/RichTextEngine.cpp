// RichTextEngine.cpp: implementation of the CRichTextEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "RichTextEngine.h"
#include "barsread.h"
#include "txtrich.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRichTextEngine::CRichTextEngine()
{
}

CRichTextEngine::~CRichTextEngine()
{
}

CRichTextEngine* CRichTextEngine::NewL(TInt aRichTextResource)
{
	CRichTextEngine* self=new(ELeave) CRichTextEngine;
	CleanupStack::PushL(self);
	self->ConstructL(aRichTextResource);
	CleanupStack::Pop(self);
	return self;
}

void CRichTextEngine::ConstructL(TInt aRichTextResource)
{
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader, aRichTextResource);
	ConstructFromResourceL(reader);
	CleanupStack::PopAndDestroy(); // reader
	SetFocus(ETrue);
}

void CRichTextEngine::DisplayStartOfHelpTextL()
{
	SetCursorPosL(0, EFalse);
}

void CRichTextEngine::SetUnderlineOn(TBool aOn)
{
	iCharFormatMask.SetAttrib(EAttFontUnderline);
	if (aOn)
		{
		iCharFormat.iFontPresentation.iUnderline = EUnderlineOn;
		}
	else
		{
		iCharFormat.iFontPresentation.iUnderline = EUnderlineOff;
		}
}

void CRichTextEngine::SetBoldOn(TBool aOn)
{
	iCharFormatMask.SetAttrib(EAttFontStrokeWeight);

	if (aOn)
		{
		iCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
		}
	else
		{
		iCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
		}
}

void CRichTextEngine::AddCarriageReturnL()
{
	CRichText* richText = RichText();
	richText->InsertL(richText->DocumentLength(), CEditableText::ELineBreak);
}

void CRichTextEngine::AddTextL(const TDesC& aText)
{
	CRichText* richText = RichText();
	TInt documentLength = richText->DocumentLength();
	richText->InsertL (documentLength, aText);
	richText->ApplyCharFormatL(iCharFormat, iCharFormatMask,documentLength,aText.Length());
}

TKeyResponse CRichTextEngine::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    if (aType == EEventKey)
		{
        if (aKeyEvent.iCode == EKeyDownArrow)
            {
            MoveCursorL (TCursorPosition::EFPageDown, EFalse);
            return EKeyWasConsumed;
            }
        else if (aKeyEvent.iCode == EKeyUpArrow)
            {
            MoveCursorL (TCursorPosition::EFPageUp, EFalse);
            return EKeyWasConsumed;
            }
		else
			{
			return CEikRichTextEditor::OfferKeyEventL(aKeyEvent, aType);
			}
        }
    return EKeyWasNotConsumed;
}

void CRichTextEngine::AddHeaderText(const TDesC& aHeader)
{
	SetUnderlineOn(ETrue);
	SetBoldOn(ETrue);
	AddTextL(aHeader); 
	AddCarriageReturnL();
	SetUnderlineOn(EFalse);
	SetBoldOn(EFalse);
}

void CRichTextEngine::AddNormalText(const TDesC& aText)
{
	AddTextL(aText);
	AddCarriageReturnL();
	AddCarriageReturnL();
}
