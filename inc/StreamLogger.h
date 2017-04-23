//
//just for simple use,
//this file doesn't following any code convensions or symbian rules.
//

#ifndef __DLL_LOGGER_H__
#define __DLL_LOGGER_H__

#include <f32file.h>  // link against efsrv.lib
#include <eikenv.h>

#ifndef __CERR__
#define __CERR__
#define CERR  StreamLogger::begin()
#else
#endif

/**
*usage::StreamLogger::begin()<<"your string"<<yourNumber<<..<<end;
*/

class StreamLogger
{
	//internel class 
	//use RAII instead of symbian 2 phases contruction.

	class LogFile
	{
		public:
			LogFile()
			{
				_LIT( KLogFile,"c:\\FriendDebug.log");
				RFs& fs = CEikonEnv::Static()->FsSession();
				if( file.Open( fs,KLogFile,EFileWrite | EFileShareAny ) != KErrNone )
					file.Create(fs,KLogFile,EFileWrite | EFileShareAny );
				TInt pos = 0;  //this must be 0
				file.Seek( ESeekEnd,pos);
			}
			~LogFile()
			{
				file.Close();
			}
			RFile& operator()() { return file ;}
		private:
			RFile file;
	};
		public :
			~StreamLogger(){}
			
			static StreamLogger& begin()
			{
				StreamLogger* self = new(ELeave)StreamLogger;
				return *self;
			}

			void suicide() { delete this; }

			StreamLogger& operator<<(const TDesC& aText)
			{
				LogFile file;
				HBufC8* text = HBufC8::NewL( aText.Length() );
				TPtr8 textPtr = text->Des();
				textPtr.Copy( aText );
				file().Write( *text );
				delete text;

				return *this;
			}

			StreamLogger& operator<<(const TDesC8& aText)
			{
				LogFile file;
				file().Write( aText );
				return *this;
			}

			StreamLogger& operator<<(TInt n)
			{
				LogFile file;
				TInt i = 1;
				for(TInt v=n;v!=0;++i,v/=10 )
				{
				}
				HBufC* text = HBufC::NewL( i);
				TPtr textPtr = text->Des();

				_LIT(KFmt,"%d");
				textPtr.Format( KFmt,n);
				HBufC8* text8 = HBufC8::NewL( textPtr.Length() );
				TPtr8 textPtr8 = text8->Des();
				textPtr8.Copy(*text);

				file().Write( *text8);
				delete text;
				delete text8;

				return *this;
			}

			StreamLogger& cr() 
			{
				LogFile file;
				TBuf8<2> enter;
				enter.Append( 13 );
				enter.Append( 10 );
				file().Write( enter );
				return *this;
			}

			typedef StreamLogger& (*_Manipulator)(StreamLogger&);
			StreamLogger& operator<<(_Manipulator op)
			{
				return op(*this);
			}

		private:
			StreamLogger() {};   //disable ctor

};

inline StreamLogger& end(StreamLogger& self)
{
	self.suicide();
	return self;
}

inline StreamLogger& endl(StreamLogger& self)
{
	self.cr();
	self.cr();
	self.suicide();
	return self;
}
#endif