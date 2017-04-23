#pragma once 

#include "e32base.h"
#include "badesca.h"

class StringTool : public CBase
{
public:
	static void Trim(TDes& srcBuf);
	static CDesCArray* AddDesArray(const CDesCArray& aArray1,const CDesCArray& aArray2);
	static void ShowString(const TDesC& str);
	static void ShowString(TInt num);
	static void ShowString(const TDesC& str,TInt num);
	static void ShowString(const TDesC& str1,const TDesC& str2);
};