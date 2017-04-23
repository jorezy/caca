/*
* ============================================================================
*  Name     : CFriendContainer from FriendContainer.h
*  Part of  : Friend
*  Created  : 21.07.2006 by 
*  Description:
*     Declares container control for application.
*  Version  :
*  Copyright: 
* ============================================================================
*/

#ifndef FRIENDCONTAINER_H
#define FRIENDCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "HttpEngine.h"
//#include "ListEngine.h"
#include "RichTextEngine.h"
#include "DBEngine.h"
#include "VmsList.h"

// SmsEngine
#include "SMSEngine.hrh"
#include "SMSParser.h"
#include "SMSEngine.h"
#include "SMSEnginePanics.pan"
//#include "Slider.h"
#include <AknWaitDialog.h> 
#include "InfoBarEngine.h"

//ContactEngine
#include "ContactEngine.h"

//UserInfo
#include "InfoTool.h"


//class MAknsControlContext; // for skins support

// class 

class MUIExitObserver
{
public:
	virtual void DoExit()=0;
};


class CFriendContainer:public CCoeControl,MCoeControlObserver,MSmsEngineObserver,MHTTPEngineObserver/*,MEikListBoxObserver*/,MVmsListKeyObserver,MRepaintObserver//,MSliderObserver
{
    public: // Constructors and destructor
        
		enum TStatus
		{
			EMenu,
			ELogin,
			EWait,
			ETalk,
			ELogout
		};
		TStatus iStatus;

        void ConstructL(const TRect& aRect);
        ~CFriendContainer();

		static TInt DoPeriodic(TAny* a);
		void RunPeriodic();

		void CreateMessage();
		void SendSms();

		//From MEikListBoxObserver
		//void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

		// From HttpEngine
        void ResponseStatusL(TInt aStatusCode, const TDesC& aStatusText);
        void ResponseReceivedL(const TDesC& aResponse);

		// From MSmsEngineObserver
        void SendingMessage();
        void MessageSent();
        void MessageReceived(const TDesC& aAddress, const TDesC& aMessage);
        void SmsEngineInitialized();
        void SmsEngineError(TInt aErrorCode);
        void MessageDeleted(); //if automatic deletion is on
		void HandleReceivedMessageL(const TDesC& aAddress, const TDesC& aMessage);

		TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent,TEventCode aCode);
		void LoadMap();
		void LoadMap(CFriendContainer::TStatus aStatus);
		void ShowDesCArray(const CDesCArray& aDesCArray);

		void AddHeader(CDesCArray* aDesCArray,const CArrayFix<TInt>& aNumArray,const TDesC& aHeader);
		void DeleteHeader(CDesCArray* aDesCArray,const TDesC& aHeader);

		void ConstructFirstList();
		void DoCheckLogin();
		void DoLogin();
		//TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
		//void SetSkin();
		void SetStatus(CFriendContainer::TStatus aStatus);
		void DoTalkOver();
		void SetUIObserver(MUIExitObserver* aObserver);
		void DoMenuMessage();
		void DoMenuSms();
		void DoMenuMtm();
		void DoMenuShowTel();
		void DoMenuTel();
		void DoMenuEdit();
		void ExitUI();

    private: // Functions from base classes

        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void Draw(const TRect& aRect) const;
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

		void DoVmsListKeyEventL();
		void DoReDraw(const TRect& aRect);
		bool CheckInputTxt();
		void SendInputTxt();
		void ReserveMsg(const TDesC& aMsg);
		void RunSlider();
		void DoSliderOverEvent();
		void ChangeScene();
		void DoRepaint(const TRect& aRect);
		void BuildInfoBar();

		TInt NetLogin();
		TInt NetUploadInfo();
		TInt NetSendMsg(const TDesC& aMsg);
		TInt NetReceiveMsg();
		void DoNetCompleted(const TDesC& aReceived);
		bool HasUserInfo();
		TInt BuildUserInfo();
		TInt PackageFriendInfo(HBufC& aFriendInfo);

		MAknsControlContext* iBackGround; // for skins support
		CHttpEngine* iHttpEngine;
		//CListEngine* iListEngine;
		CRichTextEngine* iRichText;
		CPeriodic* iPeriodic;
		CFbsBitmap* iMap;
		CFbsBitmap* iCellMap;

        CSmsEngine* iSmsEngine;
		CSMSParser* iParser;
        TBuf<EMtmsEngineMaxAddressLength> iAddress;
		TBuf<KSmsMessageLength> iMessage;
		TBool iAutomaticDelete;
		CDBEngine* iDBEngine;
		CVmsList* iVList;

		CContactEngine* iContactEngine;
		CDesCArrayFlat* iNameArray;
		CDesCArrayFlat* iNumberArray;
		CDesCArray* iListFullArray;
		TInt iIconNumber;
		CEikEdwin* iNameEdwin;
		CEikEdwin* iPasswordEdwin;
		CEikEdwin* iWordsEdwin;
		CEikEdwin* iInputEdwin;
		CEikEdwin* iCurrentEdwin;

		//CSlider* iSlider;
		CAknWaitDialog* iWaitDialog;
		CInfoBarEngine* iInfoBar;
		MUIExitObserver* iUIObserver;

		TBuf<10> iUserName;
		TBuf<11> iMobileNumber;
		TBuf<300> iUrl;

		enum TNetStatus
		{
			ENetNone,
			ENetLogin,
			ENetUploadInfo,
			ENetSendMsg,
			ENetReceiveMsg
		};
		TNetStatus iNetStatus;
		bool isFirstLogin;
		TBuf<15> iIMEI;
		TBuf<15> iIMSI;

		TBuf<100> iSendMsg;
		TBuf<20> iSendName;
		TBuf<11> iSendNumber;
		TBuf<100> iReceiveMsg;
		TBuf<11> iReceiveNumber;
		TBuf<1000> iTotalWord;

		bool iNetIsSending;
		bool iNetIsReserving;
};

#endif

// End of File
