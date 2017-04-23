// HttpEngine.h: interface for the CHttpEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPENGINE_H__889F591E_9435_40DA_A6C1_35A6743D4CAE__INCLUDED_)
#define AFX_HTTPENGINE_H__889F591E_9435_40DA_A6C1_35A6743D4CAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEL(f) if(f) delete f; f=NULL;

#include "e32base.h"
#include <http\MHTTPAuthenticationCallback.h>
#include <http\mhttpdatasupplier.h>	
#include <http\mhttptransactioncallback.h>
#include <http\RHTTPSession.h>
#include <http\RHTTPTransaction.h>
#include <chttpformencoder.h>

class MHTTPEngineObserver
{
	public:
		virtual void ResponseStatusL(TInt aStatusCode, const TDesC& aStatusText) = 0;
		virtual void ResponseReceivedL(const TDesC& aResponseBuffer) = 0;
};

class CHttpEngine : public CBase,public MHTTPTransactionCallback//,public MHTTPDataSupplier  
{
public:

	static CHttpEngine* NewL(MHTTPEngineObserver& aObserer);
	virtual ~CHttpEngine();

//	void GetRequestL(const TDesC& aUri);
	void GetRequestL(const TDesC& aUri,const TDesC8& aUserAgent,const TDesC8& aAccept,const TDesC8& aProxy);
//	void PostRequestL(const TDesC& aName);
	void PostRequestL(const TDesC& aNumber,const TDesC& aBody/*,const TDesC& aName,const TDesC8& aPostParamName*/,const TDesC8& aPostUri,const TDesC8& aUserAgent,const TDesC8& aAccept,const TDesC8& aPostContentType,const TDesC8& aProxy);
	void SendString(const TDesC8& aUrl,const TDesC& aDataBuffer);
	void SendFile(const TDesC8& aUrl,const TDesC8& aDataBuffer,const TDesC8& aMimeType);

	//MHTTPDataSupplier
	void ReleaseData();
	TBool GetNextDataPart(TPtrC8& aDataPart);
	TInt Reset();
	TInt OverallDataSize();

	void DoClose();

private:

	CHttpEngine(MHTTPEngineObserver& aObserer);
	void ConstructL();
	void Cancel();
	void ParseUriL(const TDesC& aUri);
	void AddHeaderL(RHTTPHeaders aHeaders, TInt aHeaderField, const TDesC8& aHeaderValue);
	void AddHeaderL(RHTTPHeaders aHeaders, const TDesC8& aFiledName, const TDesC8& aFiledValue);

	// from MHTTPTransactionCallback
	virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
	virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

private:

	RHTTPSession iSession;
	RHTTPTransaction iTransaction;

	HBufC* iResponseBuffer;
	HBufC8* iUri;
	TUriParser8 iUriParser;
	CHTTPFormEncoder* iFormEncoder;

	MHTTPEngineObserver& iObserver;
	bool iTransferIsOver;

	HBufC8* iPostData; // Data for HTTP POST

};

#endif // !defined(AFX_HTTPENGINE_H__889F591E_9435_40DA_A6C1_35A6743D4CAE__INCLUDED_)
