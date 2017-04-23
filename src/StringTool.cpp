#include "StringTool.h"
#include "eikenv.h"

void StringTool::Trim(TDes& srcBuf)
{
	int num=srcBuf.Length();

	for(int m=0;m<num;m++) // del " " from front
	{
		int fp=srcBuf.Mid(0,1).Find(_L(" "));
		if(fp>=0)
		{
			srcBuf.Copy(srcBuf.Mid(1));
		}
	}

	num=srcBuf.Length();
	TUint16 blank=' ';
	for(int n=num-1;n>0;n--)
	{
		int ep=srcBuf.Find(&blank,n);
		if(ep>=0)
		{
			srcBuf.Copy(srcBuf.Mid(0,n));
		}
	}
}

CDesCArray* StringTool::AddDesArray(const CDesCArray& aArray1,const CDesCArray& aArray2)
{
	int count=aArray1.Count();
	int count2=aArray2.Count();
	if(count<=0 || count2<=0 || count!=count2)
		User::Leave(-6);

	CDesCArray* result=new CDesCArrayFlat(count);
	for(int m=0;m<count;m++)
	{
		int k1=aArray1.MdcaPoint(m).Length();
		int k2=aArray2.MdcaPoint(m).Length();
		HBufC* buf=HBufC::NewL(k1+k2+1);
		buf->Des().Copy(aArray1.MdcaPoint(m));
		buf->Des().Append(_L(" "));
		buf->Des().Append(aArray2.MdcaPoint(m));
		result->AppendL(buf->Des());
		delete buf;
	}
	return result;
}

void StringTool::ShowString(const TDesC& str)
{
	CEikonEnv::Static()->AlertWin(str);
}

void StringTool::ShowString(TInt num)
{
	TBuf<10> str(0);
	str.AppendNum(num);
	CEikonEnv::Static()->AlertWin(str);
}

void StringTool::ShowString(const TDesC& str,TInt num)
{
	TBuf<100> str2(str);
	str2.AppendNum(num);
	CEikonEnv::Static()->AlertWin(str2);
}

void StringTool::ShowString(const TDesC& str1,const TDesC& str2)
{
	CEikonEnv::Static()->AlertWin(str1,str2);
}
