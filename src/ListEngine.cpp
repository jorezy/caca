// ListEngine.cpp: implementation of the CListEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "ListEngine.h"
#include "barsread.h"
#include "eikenv.h"
#include <AknIconArray.h> // CAknIconArray
#include <eikfrlbd.h>
#include <eikclbd.h>  // CColumnListBoxData
#include <eiklbx.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListEngine::~CListEngine()
{
}

void CListEngine::ConstructL(TInt aListResource)
{
	TResourceReader reader;
	CEikonEnv::Static()->CreateResourceReaderLC(reader,aListResource);
	ConstructFromResourceL(reader);
	CleanupStack::PopAndDestroy(); // reader

	SetupScrollBarsL();
//	iFocus=true;
}

void CListEngine::ConstructL(const CDesCArray& aDesCArray,bool aNeedClear)
{
	CTextListBoxModel* pM=Model();
	pM->SetOwnershipType(ELbmOwnsItemArray);
	CDesCArray* pA=(CDesCArray*)pM->ItemTextArray();

	if(aNeedClear)
	{
		pA->Reset();
/*		TInt num=pA->Count();
		for(int m=0;m<num;m++)
			pA->Delete(0); */
	}

	int num=aDesCArray.Count();
	for(int n=0;n<num;n++)
	{
		TBuf<100> buf(NULL);//_L("0\t"));
		buf.Append(aDesCArray.MdcaPoint(n));
//		CEikonEnv::Static()->AlertWin(buf);
		pA->AppendL(buf);
	}

	CreateScrollBarFrameL();
	ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
}

void CListEngine::BuildIconsL(TInt aResource,const TDesC& aFileName,const CArrayFix<TInt>* aMapArray,const CArrayFix<TInt>* aMaskArray)
{
// 	CAknColumnListBox* pList=(CAknColumnListBox*)ControlOrNull(aResource);
//  	CAknColumnListBox* pList=(CAknColumnListBox*)this;
	int count=aMapArray->Count();
	if(aMaskArray)
	{
		int count2=aMaskArray->Count();
		if(count!=count2)
			User::Leave(-6);
	}

	CArrayPtr<CGulIcon>* icons=new(ELeave)CAknIconArray(count);
	CleanupStack::PushL(icons);
	TFileName name(aFileName);
	CompleteWithAppPath(name);

	for(int m=0;m<count;m++)
	{
		CGulIcon* icon;
		if(aMaskArray)
			icon=iEikonEnv->CreateIconL(name,aMapArray->At(m),aMaskArray->At(m));
		else
		{
			int num=aMapArray->At(m);
			icon=iEikonEnv->CreateIconL(name,num);
		}

		icons->AppendL(icon);
//		delete icon;
	}

	CleanupStack::Pop(icons);
//	ItemDrawer()->ColumnData()->SetIconArray(icons);

 	CColumnListBoxItemDrawer* lid=this->ItemDrawer();
 	CColumnListBoxData* clbd=lid->ColumnData();
//	CArrayPtr<CGulIcon>* iconArray=clbd->IconArray();
//	iconArray->Reset();
	clbd->SetIconArray(icons);  // not delete !!
/**/
// 	pList->ItemDrawer()->ColumnData()->SetIconArray(icons);  // not delete !!
}

void CListEngine::SetupScrollBarsL()
	{
	CreateScrollBarFrameL(ETrue);
	ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	}


void CListEngine::SizeChanged()
{
	CAknSingleGraphicStyleListBox::SizeChanged();
}

void CListEngine::Draw(const TRect& aRect)const
{
//	CEikColumnListBox::SetSkinEnabledL(ETrue);
// 	CEikColumnListBox::Draw(TRect(TPoint(),TSize(50,50)));
 	CAknColumnListBox::Draw(TRect(TPoint(),TSize(50,50)));
/*
	CWindowGc& gc=SystemGc();
	gc.SetPenColor(TRgb(255,0,0));
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetBrushColor(TRgb(0,255,0));
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.DrawRect(TRect(TPoint(29,35),TSize(50,50)));
*/
}

TKeyResponse CListEngine::OfferKeyEventL(const TKeyEvent& aEvent,TEventCode aCode)
{
//	CEikColumnListBox::OfferKeyEventL(aEvent,aCode);
	CAknColumnListBox::OfferKeyEventL(aEvent,aCode);
	return EKeyWasNotConsumed;
}

TInt CListEngine::CountComponentControls() const
{
	int k=CAknSingleGraphicStyleListBox::CountComponentControls();
	return k;
}

CCoeControl* CListEngine::ComponentControl(TInt aIndex) const
{
	CCoeControl* res=CAknSingleGraphicStyleListBox::ComponentControl(aIndex);
	return res;
}

/*
void CListEngine::BuildIconsL()
{
	CAknGrid* pList=(CAknGrid*)ControlOrNull(EMyDlg21);
	CArrayPtr<CGulIcon>* pA=new(ELeave)CAknIconArray(1);
	CGulIcon* icon=iEikonEnv->CreateIconL(_L("z:\\system\\apps\\maps\\maplist.mbm"),EMbmMaplist01,EMbmMaplist02);
	pA->AppendL(icon);
	pList->ItemDrawer()->FormattedCellData()->SetIconArray(pA);
}

void CListEngine::BuildIconsL()
{
 	CAknColumnListBox* pList=(CAknColumnListBox*)ControlOrNull(EDlg1);
	CArrayPtr<CGulIcon>* icons=new(ELeave)CAknIconArray(1);
	CleanupStack::PushL(icons);
	CGulIcon* icon=iEikonEnv->CreateIconL(_L("z:\\system\\apps\\maps\\maplist.mbm"),EMbmMaplist01,EMbmMaplist02);
	icons->AppendL(icon);
	CleanupStack::Pop(icons);
	pList->ItemDrawer()->ColumnData()->SetIconArray(icons);  // not delete !!
}

void CListEngine::ProcessCommandL(TInt aCmd)
{
	if(aCmd==EAknCmdMark||aCmd==EAknCmdUnmark||aCmd==EAknMarkAll||aCmd==EAknUnmarkAll)
	{
		CAknColumnListBox* pList=(CAknColumnListBox*)ControlOrNull(EDlg1);
		AknSelectionService::HandleMarkableListProcessCommandL(aCmd,pList);
	}
	CAknDialog::ProcessCommandL(aCmd);
}
*/
