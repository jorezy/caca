// DBEngine.h: interface for the CDBEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBENGINE_H__AB2B894D_C626_402B_8617_6529CF490600__INCLUDED_)
#define AFX_DBENGINE_H__AB2B894D_C626_402B_8617_6529CF490600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "e32base.h"
#include "d32dbms.h"
#include "f32file.h"
#include "e32test.h"

class CDBEngine : public CBase  
{
public:
	CDBEngine();
	virtual ~CDBEngine();
	void doMakeDatabaseL(const TDesC& aDatabaseFileName);
	void doAddDataL(const TDesC& aDatabaseFileName);
	void doShowL(const TDesC& aHeading,RDbRowSet& aSet);
	void doCreateTableL(RDbDatabase& aDatabase);
	void doCreateIndexL(RDbDatabase& aDatabase);
	void doExampleL();
private:
	RFs fsSession;
	RTest* console;

};

#endif // !defined(AFX_DBENGINE_H__AB2B894D_C626_402B_8617_6529CF490600__INCLUDED_)
