#pragma once 

#include "e32base.h"
#include "coecntrl.h"
#include <CPbkContactItem.h> 
#include <CPbkContactEngine.h>
#include <CPbkContactEditorDlg.h> 
#include <RPbkViewResourceFile.h> 

class CContactEngine: public CBase
{
public:

	static CContactEngine* NewL(CCoeControl* aParent);
	~CContactEngine(void);
	void EditContact(void);
	void OpenContactDB(void);
	void AddContact(void);
	void GetPhoneNumber(CDesCArray* aNameArray,CDesCArray* aNumberArray);

private:

	CCoeControl* iParent;

	void ConstructL(void);
	CContactEngine(CCoeControl* aParent);

};