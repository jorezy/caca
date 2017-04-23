// DBEngine.cpp: implementation of the CDBEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "DBEngine.h"
#include "s32file.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBEngine::CDBEngine()
{
	User::LeaveIfError(fsSession.Connect());
	console=new(ELeave) RTest(_L("DB Test"));
}

CDBEngine::~CDBEngine()
{
	fsSession.Close();
	console->Close();
	delete console;
}

void CDBEngine::doExampleL()
{
	_LIT(KDbCreateDb,"dbcreate.db");
	_LIT(KCEpoc32exData,"C:\\epoc32ex\\data\\");

	TParse	name;
	fsSession.MkDirAll(KCEpoc32exData);
	fsSession.Parse(KDbCreateDb,KCEpoc32exData,name);

	doMakeDatabaseL(name.FullName());
	doAddDataL(name.FullName());
}

void CDBEngine::doMakeDatabaseL(const TDesC& aDatabaseFileName)
{
	CFileStore* store = CPermanentFileStore::ReplaceLC(fsSession,aDatabaseFileName,EFileRead|EFileWrite);
	store->SetTypeL(store->Layout());
	RDbStoreDatabase database;
	TStreamId id=database.CreateL(store);
	store->SetRootL(id);
	store->CommitL();
	doCreateTableL(database);
	doCreateIndexL(database);
	database.Close();
	CleanupStack::PopAndDestroy();
}

void CDBEngine::doAddDataL(const TDesC& aDatabaseFileName)
{
	_LIT(KSQLStatement,"select title,artist,price from CDs order by artist,title");
	_LIT(KComposition1, "Enigma Variations");
	_LIT(KComposer1, "Elgar");
	_LIT(KComposition2, "Symphony no. 5");
	_LIT(KComposer2, "Beethoven");

	CFileStore* store = CFileStore::OpenLC(fsSession,aDatabaseFileName,EFileRead|EFileWrite);
	RDbStoreDatabase database;
	database.OpenL(store,store->Root());

	RDbView view;
	User::LeaveIfError(view.Prepare(database,TDbQuery(KSQLStatement,EDbCompareNormal)));
	User::LeaveIfError(view.EvaluateAll());

	view.InsertL();

	view.SetColL(1,KComposition1);
	view.SetColL(2,KComposer1);
	view.SetColL(3,1299);

	view.PutL();

	view.InsertL();

	view.SetColL(1,KComposition2);
	view.SetColL(2,KComposer2);
	view.SetColL(3,1499);

	view.PutL();

	_LIT(KShowInfo, "Rowset contents...");
	doShowL(KShowInfo, view);
	view.Close();
	database.Close();
	CleanupStack::PopAndDestroy();
}

void CDBEngine::doShowL(const TDesC& aHeading,RDbRowSet& aSet)
{
	_LIT(KNewline, "\n") ;
	_LIT(KRowHeadingFormatter, "\n%d row(s):");
	_LIT(KRowFormatter, "\n  %S, %S:  %d.%02.2d");

	console->Printf(KNewline);
	console->Printf(aHeading);

	console->Printf(KRowHeadingFormatter,aSet.CountL());
	for (aSet.FirstL();aSet.AtRow();aSet.NextL())
	{
		aSet.GetL();
		TPtrC title=aSet.ColDes(1);
		TPtrC artist=aSet.ColDes(2);

		TInt pricePounds=(aSet.ColInt(3)/100);
		TInt pricePence=(aSet.ColInt(3)-(pricePounds*100));
		console->Printf(KRowFormatter,&artist,&title,pricePounds,pricePence);
	}
	console->Printf(KNewline);
}

void CDBEngine::doCreateTableL(RDbDatabase& aDatabase)
{
	_LIT(KCol1, "Artist");
	_LIT(KCol2, "Title");
	_LIT(KCol3, "Price");
	_LIT(KTable, "CDs");

	CDbColSet* columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KCol1,EDbColText));

	TDbCol title(KCol2,EDbColText,60);
	title.iAttributes=TDbCol::ENotNull;
	columns->AddL(title);

	TDbCol price(KCol3,EDbColInt32);
	price.iAttributes=TDbCol::ENotNull;
	columns->AddL(price);
	User::LeaveIfError(aDatabase.CreateTable(KTable,*columns));
	CleanupStack::PopAndDestroy();
}

void CDBEngine::doCreateIndexL(RDbDatabase& aDatabase)
{
	_LIT(KCol1, "Artist");
	_LIT(KCol2, "Title");
	_LIT(KTable, "CDs");

	CDbKey* key=CDbKey::NewLC();
	TDbKeyCol artist(KCol1);
	key->AddL(artist);
	TDbKeyCol title(KCol2);
	key->AddL(title);

	User::LeaveIfError(aDatabase.CreateIndex(KTable,KTable,*key));
	CleanupStack::PopAndDestroy();
}


