#include "ContactEngine.h"
#include "coemain.h"
#include "eikenv.h"
#include "StringTool.h"

const TInt KMaxDatabaseNameLength = 12;
const TInt KMaxDatabasePathAndNameLength = 60;
const TInt KMaxTemplateNameLength = 20;
const TInt KMaxTextLength = 19;
const TInt KMaxGroupNameLength = 20;


//const TBuf<KMaxDatabasePathAndNameLength> KContactDbName(_L("c:\\system\\data\\contacts.cdb"));
_LIT(KContactDbName,"c:\\System\\Data\\Contacts.cdb");

//_LIT(KNameOfXin,"姓");
//_LIT(KNameOfMin,"名");
//_LIT(KNameOfPhoneNumber,"手机");
const TUint16 KUint_Xin[] = { 0x59d3, 0 }; //姓
const TUint16 KUint_Min[] = { 0x540d, 0 }; //名
const TUint16 KUint_ShouJi[] = { 0x624b, 0x673a,  0 }; //手机

const TInt KLengthName=30;
const TInt KLengthXin=10;


CContactEngine* CContactEngine::NewL(CCoeControl* aParent)
{
	CContactEngine* self=new(ELeave) CContactEngine(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

void CContactEngine::EditContact(void)
{
	CContactDatabase* iContactDb = CContactDatabase::OpenL();
    const CContactIdArray* contacts = iContactDb->SortedItemsL();

    TContactItemId itemId( ((*contacts)[1]) ); // currentItemIndex

	RFs& fs=CCoeEnv::Static()->FsSession();
    CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL(KContactDbName,EFalse,&fs);
    CleanupStack::PushL( pbkContactEng );

    RPbkViewResourceFile pbkRes( *CCoeEnv::Static() );
    pbkRes.OpenL();

    CPbkContactItem* aContactItem = pbkContactEng->OpenContactL( itemId );
    CleanupStack::PushL( aContactItem );

    CPbkContactEditorDlg* pbkContactDlg = CPbkContactEditorDlg::NewL( *pbkContactEng,*aContactItem,EFalse, -1,EFalse );

	CleanupStack::PushL( pbkContactDlg );
    pbkContactDlg->SetMopParent( iParent ); //iAvkonAppUi

    TInt res( KErrNone );
    TRAPD( err, res = pbkContactDlg->ExecuteLD() );

    pbkRes.Close();
    pbkContactEng->CloseContactL( aContactItem->Id() );

	CleanupStack::Pop( pbkContactDlg );
    CleanupStack::PopAndDestroy( 2 ); //aContactItem, pbkContactEng
}

void CContactEngine::OpenContactDB(void)
{
	CContactDatabase* iContactDb = CContactDatabase::OpenL();
    const CContactIdArray* contacts = iContactDb->SortedItemsL();


	int count=contacts->Count();
	// ---------- 查看 ------------
	for(int k=1;k<count;k++)
	{
		TContactItemId itemId( ((*contacts)[k]) ); // currentItemIndex

		CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL(KContactDbName,EFalse,&CCoeEnv::Static()->FsSession());
		CleanupStack::PushL( pbkContactEng );

		RPbkViewResourceFile pbkRes( *CCoeEnv::Static() );
		pbkRes.OpenL();

		CPbkContactItem* cntItem = pbkContactEng->OpenContactL(itemId);//need to delete
		CleanupStack::PushL( cntItem );
		CPbkFieldArray* fieldSet = &cntItem->CardFields();
		TInt countFieldSet = fieldSet->Count();
		for(int err=0;err<countFieldSet;err++) 
		{
			TPbkContactItemField x;
			x=fieldSet->At(err);
			TBuf16<KLengthName> label(x.Label());
			TBuf16<KLengthName> text(x.Text());
			TBuf<KLengthName*2> buf;
			buf.Append(label);
			buf.Append(text);
			CEikonEnv::Static()->AlertWin(buf);
		}

		pbkContactEng->CloseContactL( cntItem->Id() );
		pbkRes.Close();
		CleanupStack::PopAndDestroy(2); //cntItem, pbkContactEng
	}

	delete iContactDb;
}

void CContactEngine::GetPhoneNumber(CDesCArray* aNameArray,CDesCArray* aNumberArray)
{
	CContactDatabase* iContactDb = CContactDatabase::OpenL();
    const CContactIdArray* contacts = iContactDb->SortedItemsL();
	int count=contacts->Count();
	//StringTool::ShowString(_L("getNum:  "),count); // test!!

	// ---------- 取名字及号码 ------------
	for(int k=0;k<count;k++)
	{
		TContactItemId itemId( ((*contacts)[k]) ); // currentItemIndex

		CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL(KContactDbName,EFalse,&CCoeEnv::Static()->FsSession());
		CleanupStack::PushL( pbkContactEng );

		RPbkViewResourceFile pbkRes( *CCoeEnv::Static() );
		pbkRes.OpenL();

        CPbkContactItem* cntItem = pbkContactEng->ReadContactLC( itemId );
		//CPbkContactItem* cntItem = pbkContactEng->OpenContactL(itemId);//need to delete
		//CleanupStack::PushL( cntItem );
		TBuf<KLengthName> xin(NULL);
		TBuf<KLengthName> min(NULL);
		TBuf<KLengthName> num(NULL);

        TPbkContactItemField* fnameField = cntItem->FindField(EPbkFieldIdFirstName);
		if(fnameField)
		{
			fnameField->GetTextL(xin);
		}
        TPbkContactItemField* lnameField = cntItem->FindField(EPbkFieldIdLastName);
		if(lnameField)
		{
			lnameField->GetTextL(min);
		}
        TPbkContactItemField* phoneFieldMobile = cntItem->FindField(EPbkFieldIdPhoneNumberMobile);
		if(phoneFieldMobile)
		{
			phoneFieldMobile->GetTextL(num);
		}

		num.Trim();
		if(num.Length()>=11)
		{
			TBuf<2> mNumber(num.Mid(num.Length()-11,2)); // 139XXXXXXXX 取13作检验,15也行
			TLex lex(mNumber);
			int nn;
			lex.Val(nn);
			if(nn==13 || nn==15)
			{
				TBuf<KLengthName*2> name(xin);
				name.Append(_L(" "));
 				name.Append(min);
 				aNameArray->AppendL(name);
				aNumberArray->AppendL(num.Mid(num.Length()-11,11));
			}
		}

		pbkContactEng->CloseContactL( cntItem->Id() );
		pbkRes.Close();
		CleanupStack::PopAndDestroy(2); //cntItem, pbkContactEng

//		CContactIdArray* ciArray=cntItem->GroupsJoinedLC();
//		TInt countFieldSet==ciArray->Count();
//		CleanupStack::PopAndDestroy(ciArray);
	}

	delete iContactDb;

	/*CContactDatabase* iContactDb = CContactDatabase::OpenL();
    const CContactIdArray* contacts = iContactDb->SortedItemsL();
	int count=contacts->Count();
	StringTool::ShowString(_L("getNum:"),count); // test!!

	// ---------- 取名字及号码 ------------
	for(int k=0;k<count;k++)
	{
		TContactItemId itemId( ((*contacts)[k]) ); // currentItemIndex

		CPbkContactEngine* pbkContactEng = CPbkContactEngine::NewL(KContactDbName,EFalse,&CCoeEnv::Static()->FsSession());
		CleanupStack::PushL( pbkContactEng );

		RPbkViewResourceFile pbkRes( *CCoeEnv::Static() );
		pbkRes.OpenL();

		CPbkContactItem* cntItem = pbkContactEng->OpenContactL(itemId);//need to delete
		CleanupStack::PushL( cntItem );
		CPbkFieldArray* fieldSet = &cntItem->CardFields();
		TInt countFieldSet = fieldSet->Count();
		//StringTool::ShowString(_L("getFieldSet:"),countFieldSet); // test!!

		TBuf<KLengthName> xin(NULL);
		TBuf<KLengthName> min(NULL);
		TBuf<KLengthName> num(NULL);
		bool canAdd=false;
		for(int err=0;err<48;err++)// on 6681,countFieldSet=52,but can error,so only can set 48
		{
			TPbkContactItemField x;
			x=fieldSet->At(err);

			TBuf<KLengthXin> buf1(KUint_Xin);
			int k1=buf1.Find(x.Label());
			if(k1>=0)
			{
				canAdd=true;
				xin.Zero();
				xin.Append(x.Text());
			}

			TBuf<KLengthXin> buf2(KUint_Min);
			int k2=buf2.Find(x.Label());
			if(k2>=0)
			{
				canAdd=true;
				min.Zero();
				min.Append(x.Text());
			}

			TBuf<KLengthXin> buf3(KUint_ShouJi);
			int k3=buf3.Find(x.Label());
			if(k3>=0)
			{
				canAdd=true;
				num.Zero();
				num.Append(x.Text());
			}
		}
		if(canAdd)
		{
			TBuf<KLengthName*2> name(xin);
			name.Append(_L(" "));
 			name.Append(min);
 			aNameArray->AppendL(name);
			aNumberArray->AppendL(num);
		}


		pbkContactEng->CloseContactL( cntItem->Id() );
		pbkRes.Close();
		CleanupStack::PopAndDestroy(2); //cntItem, pbkContactEng
	}

	delete iContactDb; */
}

void CContactEngine::AddContact(void)
{
	CContactDatabase* iContactDb = CContactDatabase::OpenL();
	CContactItem* item = CContactCard::NewLC();

	// ---------- 增加名片 ------------
	CContactItemField* field1 = CContactItemField::NewLC(KStorageTypeText , KUidContactFieldGivenName);
	CContactItemField* field2 = CContactItemField::NewLC(KStorageTypeText , KUidContactFieldFamilyName);
	CContactItemField* field3 = CContactItemField::NewLC(KStorageTypeText , KUidContactFieldPhoneNumber);
	CContactItemField* field4 = CContactItemField::NewLC(KStorageTypeText , KUidContactFieldVCardMapTITLE);
	CContactItemField* field5 = CContactItemField::NewLC(KStorageTypeText , KUidContactFieldVCardMapADR);

	field1->TextStorage()->SetTextL(_L("KName"));//*Fname);
	field1->SetMapping(KUidContactFieldVCardMapUnusedN);
	field2->TextStorage()->SetTextL(_L("LName"));//*Lname);
	field2->SetMapping(KUidContactFieldVCardMapUnusedN);
	field3->TextStorage()->SetTextL(_L("KPhone"));//*Phone);
	field3->SetMapping(KUidContactFieldVCardMapTEL);
	field4->TextStorage()->SetTextL(_L("KJob"));//*Job);
	field4->SetMapping(KUidContactFieldVCardMapTITLE);
	field5->TextStorage()->SetTextL(_L("KAddr"));//*Addr);
	field5->SetMapping(KUidContactFieldVCardMapADR);

	item->AddFieldL(*field1);
	item->AddFieldL(*field2);
	item->AddFieldL(*field3);
	item->AddFieldL(*field4);
	item->AddFieldL(*field5);
	// item takes the ownership of fields
	CleanupStack::Pop(5);
/**/
	int id = 0;
	id = iContactDb->AddNewContactL(*item);

	CContactItem* i = iContactDb->CreateContactGroupL(_L("TEST"));
	CContactGroup* Gitem = CContactGroup::NewL();
	iContactDb->AddContactToGroupL(*item, *i);

	delete Gitem;
	delete i;
	CleanupStack::PopAndDestroy(); // item
//	delete item;
//	iContactDb->Close(); // CloseContactL(TContactItemId aContactId);
	delete iContactDb;
}

void CContactEngine::ConstructL(void)
{
}

CContactEngine::CContactEngine(CCoeControl* aParent):iParent(aParent)
{
}

CContactEngine::~CContactEngine(void)
{
}
