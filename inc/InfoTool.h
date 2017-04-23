#pragma once

#include "e32base.h"
#include "f32file.h"

class CInfoTool: public CBase
{
public:

	static CInfoTool* NewL();
	~CInfoTool();
 
	TInt OpenFile(const TDesC& aFileName,TBool aCreate); // if file no find & aBeCreate is true ,create file
	void Close();
	TInt Set(const TDesC& aKey,TInt aValue);
	TInt Set(const TDesC& aKey,const TDesC& aValue);
	TInt Get(const TDesC& aKey,TInt& aValue);
	TInt Get(const TDesC& aKey,TDes& aValue);
	TInt Update(const TDesC& aKey,TInt aValue);
	TInt Update(const TDesC& aKey,const TDesC& aValue);
	TBool CheckKey(const TDesC& aKey);
 
private:

	CInfoTool();

	void ConstructL();

public:

private:
 
	RFs fs;
	RFile file;
 
};