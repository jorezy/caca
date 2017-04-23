#include "InfoTool.h"
#include "EikEnv.h"
 
_LIT(KInfoFilter,"###");
_LIT(KKeyFilter,"$$$");


TInt CInfoTool::OpenFile(const TDesC& aFileName,TBool aCreate)
{
	int k=file.Open(fs,aFileName,EFileRead|EFileWrite|EFileShareAny);
	if(k==KErrNotFound && aCreate)
	{
		int err=file.Create(fs,aFileName,EFileRead|EFileWrite|EFileShareAny);
		if(err>0)
			return KErrNone;
		else return err;
	}
	
	return k;
}

void CInfoTool::Close()
{
	file.Close();
	fs.Close();
}

TInt CInfoTool::Set(const TDesC& aKey,TInt aValue)
{
	TBuf<15> buf;
	buf.AppendNum(aValue);
	return Set(aKey,buf);
}

TInt CInfoTool::Set(const TDesC& aKey,const TDesC& aValue)
{
	if(!CheckKey(aKey))
		return KErrArgument;

	int pos=0;
	file.Seek(ESeekEnd,pos);
	int size=aKey.Length()+aValue.Length()+KInfoFilter().Length()+KKeyFilter().Length();
	HBufC* buf=HBufC::NewL(size);
	buf->Des().Copy(KInfoFilter);
	buf->Des().Append(aKey);
	buf->Des().Append(KKeyFilter);
	buf->Des().Append(aValue);

	HBufC8* buf8=HBufC8::NewL(size);
	buf8->Des().Copy(buf->Des());
	file.Write(buf8->Des(),size);
	file.Flush();

	delete buf8;
	delete buf;
	return KErrNone;
}

TInt CInfoTool::Get(const TDesC& aKey,TInt& aValue)
{
	TBuf<15> buf(NULL);
	int err=Get(aKey,buf);
	if(err==KErrNone)
	{
		buf.Num(aValue);
	}

	return err;
}

TInt CInfoTool::Get(const TDesC& aKey,TDes& aValue)
{
	int k=0;
	file.Seek(ESeekStart,k);

	int result=KErrNone;
	int size=0;
	int err=file.Size(size);
	if(err!=KErrNone)
		User::Leave(KErrNotFound);
	HBufC8* hb=HBufC8::NewL(size);

	file.Read(hb->Des(),size);
	HBufC* hb2=HBufC::NewL(size);
	hb2->Des().Copy(hb->Des());

	int pos=hb2->Des().Find(aKey);
	if(pos>=0)
	{
		int size2=size-pos-aKey.Length()-KKeyFilter().Length();
		HBufC* hb3=HBufC::NewL(size2);
		hb3->Des().Copy(hb2->Des().Mid(pos+aKey.Length()+KKeyFilter().Length()));
		int pos2=hb3->Des().Find(KInfoFilter);
		if(pos2>=0)
			aValue.Copy(hb3->Des().Mid(0,pos2));
		else
			aValue.Copy(hb3->Des());

		delete hb3;
	}
	else
		result=pos;

	delete hb2;
	delete hb;

	return result;
}

TBool CInfoTool::CheckKey(const TDesC& aKey)
{
	int size=0;
	int err=file.Size(size);
	if(err!=KErrNone)
		User::Leave(KErrNotFound);
	HBufC8* hb=HBufC8::NewL(size);

	int k=0;
	file.Seek(ESeekStart,k);
	file.Read(hb->Des(),size);
	HBufC* hb2=HBufC::NewL(size);
	hb2->Des().Copy(hb->Des());

	int pos=hb2->Des().Find(aKey);
	delete hb2;
	delete hb;

	if(pos>=0)
		return false;  // the key is exist
	else
		return true; // the key is none
}

TInt CInfoTool::Update(const TDesC& aKey,TInt aValue)
{
	TBuf<15> buf;
	buf.AppendNum(aValue);
	return Set(aKey,buf);
}

TInt CInfoTool::Update(const TDesC& aKey,const TDesC& aValue)
{
	int result=-10;
	int size=0;
	int err=file.Size(size);
	if(err!=KErrNone)
		User::Leave(KErrNotFound);
	HBufC8* hb=HBufC8::NewL(size);

	int k=0;
	file.Seek(ESeekStart,k);
	file.Read(hb->Des(),size);
	HBufC* hb2=HBufC::NewL(size);
	hb2->Des().Copy(hb->Des());

	int pos=hb2->Des().Find(aKey);
	if(pos<0)
		result=-1;  // the key is none
	else			// the key is exist
	{
		HBufC* temp=HBufC::NewL(size-pos);
		temp->Des().Copy(hb2->Des().Mid(pos));
		int k2=temp->Des().Find(KInfoFilter);
		HBufC* end=HBufC::NewL(size-pos-k2);
		end->Des().Copy(temp->Des().Mid(k2));

		HBufC* first=HBufC::NewL(pos);
		first->Des().Copy(hb2->Des().Mid(0,pos));

		int midSize=aKey.Length()+KKeyFilter().Length()+aValue.Length();
		HBufC* mid=HBufC::NewL(midSize);
		mid->Des().Copy(aKey);
		mid->Des().Append(KKeyFilter);
		mid->Des().Append(aValue);

		int allSize=midSize+size-k2;
		HBufC* all=HBufC::NewL(allSize);
		all->Des().Copy(first->Des());
		all->Des().Append(mid->Des());
		all->Des().Append(end->Des());

		delete all;
		delete mid;
		delete end;
		delete temp;
		result=KErrNone;
	}
	delete hb2;
	delete hb;
	return result;
}

 
CInfoTool* CInfoTool::NewL()
{
	CInfoTool* self=new(ELeave) CInfoTool;
  	CleanupStack::PushL(self);
 	self->ConstructL();
  	CleanupStack::Pop(self);
	return self;
}

void CInfoTool::ConstructL()
{
 	fs.Connect();
}

CInfoTool::CInfoTool()
{
}

CInfoTool::~CInfoTool()
{
}
