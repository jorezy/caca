// HttpEngine.cpp: implementation of the CHttpEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "HttpEngine.h"
#include <chttpformencoder.h>		// CHTTPFormEncoder
#include <httpstringconstants.h>	// HTTP string table
#include <http/rhttpheaders.h>			// RHTTPHeaders
#include <HttpFilterProxyInterface.h> 
#include <utf.h>
#include "StreamLogger.h"

#define EMaxUrlLength 128
#define EMaxNameLength 320
//#define EMaxNameLength 32


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpEngine::CHttpEngine(MHTTPEngineObserver& aObserver):iObserver(aObserver),iTransferIsOver(false),iPostData(NULL)
{
}

CHttpEngine::~CHttpEngine()
{
	Cancel();
	DEL(iResponseBuffer);
	DEL(iFormEncoder);
	DEL(iUri);
	DEL(iPostData);
}

CHttpEngine* CHttpEngine::NewL(MHTTPEngineObserver& aObserver)
{
	CHttpEngine* self = new (ELeave) CHttpEngine(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

void CHttpEngine::ConstructL()
{
	iSession.OpenL();
	iFormEncoder = CHTTPFormEncoder::NewL();
	CHttpFilterProxyInterface::InstallFilterL(iSession);
}

void CHttpEngine::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
{
	switch (aEvent.iStatus)
	{
		case THTTPEvent::EGotResponseHeaders:
		{
			RHTTPResponse resp = aTransaction.Response();
			TInt statusCode = resp.StatusCode();

			RStringF statusStr = resp.StatusText();
			HBufC* statusBuf = HBufC::NewLC(statusStr.DesC().Length());
			statusBuf->Des().Copy(statusStr.DesC());

			iObserver.ResponseStatusL(statusCode, *statusBuf);
			CleanupStack::PopAndDestroy(statusBuf);
			break;
		}
		case THTTPEvent::EGotResponseBodyData:
		{
			MHTTPDataSupplier* dataSupplier = aTransaction.Response().Body();
			TPtrC8 ptr;
			bool isLast=dataSupplier->GetNextDataPart(ptr);
			HBufC* buf = HBufC::NewLC(ptr.Length());
			buf->Des().Copy(ptr);

			if (!iResponseBuffer)
			{
				iResponseBuffer = buf->AllocL();
			}
			else
			{
				iResponseBuffer = iResponseBuffer->ReAllocL(iResponseBuffer->Length() + buf->Length());
				iResponseBuffer->Des().Append(*buf);
			}
			CleanupStack::PopAndDestroy(buf);
			dataSupplier->ReleaseData();
			break;
		}
		case THTTPEvent::EResponseComplete:
		{
			iObserver.ResponseReceivedL(*iResponseBuffer);
			DEL(iResponseBuffer); // note: if the content is large ,this needn't do
			break;
		}
	}
}

TInt CHttpEngine::MHFRunError(TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/)
{
	if(aError)
	{
		TBuf<10> buf(0);
		buf.AppendNum(aError);
		CERR<<_L("do MHFRunError")<<endl;
		CERR<<buf<<endl;
	}
	return aError;
}

void CHttpEngine::GetRequestL(const TDesC& aUri,const TDesC8& aUserAgent,const TDesC8& aAccept,const TDesC8& aProxy)
{
	ParseUriL(aUri);
	iTransaction = iSession.OpenTransactionL(iUriParser, *this,
		iSession.StringPool().StringF(HTTP::EGET, RHTTPSession::GetTable()));

	RHTTPHeaders headers = iTransaction.Request().GetHeaderCollection();
	AddHeaderL(headers, HTTP::EUserAgent, aUserAgent);
	AddHeaderL(headers, HTTP::EAccept, aAccept);
	AddHeaderL(headers, HTTP::EProxyAddress, aProxy);

	iTransaction.SubmitL();
}

void CHttpEngine::PostRequestL(const TDesC& aNumber,const TDesC& aBody/*,const TDesC& aName,const TDesC8& aPostParamName*/,const TDesC8& aPostUri,const TDesC8& aUserAgent,const TDesC8& aAccept,const TDesC8& aPostContentType,const TDesC8& aProxy)
{
	delete iFormEncoder;
	iFormEncoder = NULL;
	iFormEncoder = CHTTPFormEncoder::NewL();
	//TBuf8<3000> buf8;
	HBufC8* buffer=HBufC8::NewLC(aBody.Length()*2);
	TPtr8 buf8=buffer->Des();
	buf8.Zero();
	/*buf8.Copy(aBody);*/
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf8, aBody);
	//iFormEncoder->AddFieldL(_L8("friend"),buf8);
	iFormEncoder->AddFieldL(_L8("test"),_L8("test"));
	//TBuf8<EMaxNameLength> buf8;
	//buf8.Copy(aBody);
	//buf8.Copy(aName);
	//FormEncoder->AddFieldL(_L8("mpno"),_L8("13001234567"));
	//iFormEncoder->AddFieldL(_L8("userfile"),_L8("c:\\upload1.txt"));
	//iFormEncoder->AddFieldL(_L8("userfile"),buf8);
	//iFormEncoder->AddFieldL(aPostParamName, buf8);/**/

	/*DEL(iPostData);
	iPostData=HBufC8::NewL(aBody.Length());
	iPostData->Des().Copy(aBody);*/

	/*HBufC8* bufUrl=HBufC8::NewL(aPostUri.Length()+100);
	bufUrl->Des().Append(aPostUri);
	bufUrl->Des().Append((const TUint8*)"CONTENT-TYPE:applicationn\\octet",User::StringLength((const TUint8*)"CONTENT-TYPE:application\\octet"));
	bufUrl->Des().Append((const TUint8*)"\x0d\x0a",2);
	bufUrl->Des().Append((const TUint8*)strContentLen,User::StringLength((const TUint8*)strContentLen));
	bufUrl->Des().Append((const TUint8*)"\x0d\x0a",2);*/

	//RStringF stringF=iSession.StringPool().StringF(HTTP::EPOST, RHTTPSession::GetTable());
	iUriParser.Parse(aPostUri);
	iTransaction = iSession.OpenTransactionL(iUriParser,*this,iSession.StringPool().StringF(HTTP::EPOST, RHTTPSession::GetTable()));

	RHTTPHeaders headers=iTransaction.Request().GetHeaderCollection();
	AddHeaderL(headers,HTTP::EUserAgent,aUserAgent);
	AddHeaderL(headers,HTTP::EAccept,aAccept);
	AddHeaderL(headers,HTTP::EContentType,aPostContentType);
	AddHeaderL(headers,HTTP::EProxyAddress,aProxy);
	/*TBuf8<10> buflen(0);
	buflen.AppendNum(aBody.Length());
 	AddHeaderL(headers, HTTP::EContentLength, buflen);
 	AddHeaderL(headers, HTTP::EContentDisposition, _L8("attachment"));
 	AddHeaderL(headers, HTTP::ETransferEncoding, _L8("binary"));*/

	TBuf8<20> number(0);
	number.Copy(aNumber);
	AddHeaderL(headers,_L8("mpno"),number);
	AddHeaderL(headers,_L8("friend"),buf8);
/*	TBuf8<10> bodyLen(0);
	bodyLen.AppendNum(aBody.Length());
 	AddHeaderL(headers, HTTP::EContentLength,bodyLen);
*/

	iTransaction.Request().SetBody(*iFormEncoder);
	//iTransaction.Request().SetBody(*this);

	/*CERR<< _L("post submit") <<endl;
	TPtrC8 ptr;
	iTransaction.Request().Body()->GetNextDataPart(ptr);
	CERR<< ptr <<endl;*/
	CleanupStack::PopAndDestroy(); // delete buffer
	iTransaction.SubmitL();
}

void CHttpEngine::SendString(const TDesC8& aUrl,const TDesC& aDataBuffer)
{
	_LIT8(KHTTP ," HTTP/1.0");
	_LIT8(KNewLine ,"\r\n");
	_LIT8(KPostSize ,"Content-Length: ");
	_LIT8(KPostMime ,"Content-Type: ");
	_LIT8(KPostContentType, "text/plain");
	HBufC8* buffer=HBufC8::NewL(aUrl.Length()+aDataBuffer.Length()+200);
	TPtr8 buf=buffer->Des();
	buf.SetLength(0);

	buf.Append(aUrl);// buf.Append(KHTTP); 
	buf.Append(KNewLine); 
	buf.Append(KPostSize); buf.AppendNum(aDataBuffer.Length());
	//UserAgent, "Nokia7210/1.0 (3.08) Profile/MIDP-1.0 Configuration/CLDC-1.0");
	//buf.Append(KNewLine);
	//buf.Append(_L8("Proxy:")); buf.Append(_L8("10.0.0.172:80"));
	buf.Append(KNewLine);
	buf.Append(_L8("content-disposition:")); buf.Append(_L8("attachment"));
	buf.Append(_L8("; filename='file2.txt'"));
	buf.Append(KNewLine);
	buf.Append(KPostMime); buf.Append(KPostContentType);
	buf.Append(KNewLine);
	buf.Append(_L8("Content-Transfer-Encoding:")); buf.Append(_L8("binary"));

	buf.Append(KNewLine);
	buf.Append(KNewLine);
	buf.Append(aDataBuffer);
	buf.Append(KNewLine);

	iUriParser.Parse(buf);
	RStringF stringF=iSession.StringPool().StringF(HTTP::EPOST, RHTTPSession::GetTable());
	iTransaction = iSession.OpenTransactionL(iUriParser, *this, stringF);
	CERR<< iUriParser.UriDes() <<endl;
	iTransaction.SubmitL();
}

void CHttpEngine::SendFile(const TDesC8& aUrl,const TDesC8& aDataBuffer,const TDesC8& aMimeType)// 4 RSocket do it
{
	_LIT8(KPost ,"POST ");
	_LIT8(KHTTP ," HTTP/1.0");
	_LIT8(KPostSize ,"Content-Length: ");
	_LIT8(KPostMime ,"Content-Type: ");
	_LIT8(KNewLine ,"\r\n");

	HBufC8* buffer=HBufC8::NewL(aUrl.Length()+aDataBuffer.Length()+aMimeType.Length()+50);
	TPtr8 buf = buffer->Des();
	buf.SetLength(0);

	buf.Append(KPost); buf.Append(aUrl); buf.Append(KHTTP); 
	buf.Append(KNewLine); 
	buf.Append(KPostSize); buf.AppendNum(aDataBuffer.Length());
	buf.Append(KNewLine);
	buf.Append(KPostMime); buf.Append(aMimeType);
	buf.Append(KNewLine);
	buf.Append(KNewLine);
	buf.Append(aDataBuffer);
	buf.Append(KNewLine);

	//iTransaction.Request().SetBody(*iFormEncoder);
	iTransaction.SubmitL();
}

void CHttpEngine::ParseUriL(const TDesC& aUri)
{
	delete iUri;
	iUri = NULL;
	iUri = HBufC8::NewL(aUri.Length());
	iUri->Des().Copy(aUri);
	User::LeaveIfError(iUriParser.Parse(*iUri));
}

void CHttpEngine::AddHeaderL(RHTTPHeaders aHeaders, TInt aHeaderField, const TDesC8& aHeaderValue)
{
	RStringPool stringPool = iSession.StringPool();
	RStringF valStr = stringPool.OpenFStringL(aHeaderValue);
	THTTPHdrVal headerVal(valStr);
	aHeaders.SetFieldL(stringPool.StringF(aHeaderField, RHTTPSession::GetTable()), headerVal);
	valStr.Close();
}

void CHttpEngine::AddHeaderL(RHTTPHeaders aHeaders, const TDesC8& aFiledName, const TDesC8& aFiledValue)
{
	RStringPool stringPool = iSession.StringPool();
	RStringF valStr = stringPool.OpenFStringL(aFiledValue);
	RStringF fieldName=stringPool.OpenFStringL(aFiledName);
	aHeaders.SetFieldL(fieldName,valStr);
	valStr.Close();
};

void CHttpEngine::Cancel()
{
 	iTransaction.Cancel();
 	iSession.Close();
}

void CHttpEngine::DoClose()
{
 	iTransaction.Cancel();
 	//iSession.Close();
}

void CHttpEngine::ReleaseData()
{
	DEL(iPostData);
}

TBool CHttpEngine::GetNextDataPart(TPtrC8& aDataPart)
{
	if(iPostData) 
		aDataPart.Set(iPostData->Des());

	return ETrue;
}

TInt CHttpEngine::Reset()
{
	return KErrNone;
}

TInt CHttpEngine::OverallDataSize()
{
	if(iPostData)		
		return iPostData->Length();
	else
		return KErrNotFound ;
}



