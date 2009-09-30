/* Copyright (C) 2009 Mobile Sorcery AB

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.
*/

#include <e32std.h>
#include <aknnotewrappers.h>
#include <ImageConversion.h>
#include <AknUtils.h>
#include <stringloader.h>
#include <EIKAPP.H>

#include "config_platform.h"

#include "FileStream.h"
#include <core/core.h>

#include "symbian_helpers.h"
#include "AppUi.h"

#include <helpers/log.h>

#ifdef LOGGING_ENABLED

#ifdef __SERIES60_3X__
_LIT(KLogFilePath, "C:\\data\\msrlog.txt");
_LIT(KOldLogFilePath, "C:\\data\\msrlogold.txt");
#else	//Series 60, 2nd Ed.
_LIT(KLogFilePath, "C:\\msrlog.txt");
_LIT(KOldLogFilePath, "C:\\msrlogold.txt");
#endif	//__SERIES60_3X__

_LIT(KSemaphoreName, "MoSync");

//#define SEMAPHORE

void InitLog(const char* filenameOverride) {
#ifdef SEMAPHORE
	RSemaphore semaphore;
	TInt res = semaphore.CreateGlobal(KSemaphoreName, 1);
	if(res != KErrAlreadyExists)
		PanicIfError(res);
#endif	//SEMAPHORE
	{
		MyRFs myrfs;
		myrfs.Connect();
#ifdef GUIDO
		RFile oldFile;
		//if the old file exists
		if(oldFile.Open(myrfs(), KLogFilePath, EFileRead) == KErrNone) {
			oldFile.Close();
			//rename old log file to msrlog%i.txt
			int i=0;
			TBuf<32> tempFileName;
			do {
#ifdef __SERIES60_3X__
				_LIT(KLogFileFormat, "C:\\data\\msrlog%i.txt");
#else	//Series 60, 2nd Ed.
				_LIT(KLogFileFormat, "C:\\msrlog%i.txt");
#endif	//__SERIES60_3X__
				tempFileName.Format(KLogFileFormat, ++i);
			} while(FSS.Rename(KLogFilePath, tempFileName) != KErrNone);
		}
#else
		FSS.Delete(KOldLogFilePath);
		FSS.Rename(KLogFilePath, KOldLogFilePath);
#endif	//GUIDO
		{
			RFile file;
			LHEL(file.Create(FSS, KLogFilePath,
				EFileWrite | EFileShareExclusive | EFileStreamText));
			file.Close();
		}
	}
	Log("Logging started\n");
	LogAvailableMemory();
}

void LogBin(const void* data, int size) {
	TPtrC8 ptr(CBP data, size);
	AppendToFile(KLogFilePath, ptr);
}

void LogV(const char* fmt, VA_LIST args) {
	TBuf8<512> buffer;
	TPtrC8 fmtP(CBP fmt);
	buffer.FormatList(fmtP, args);
	AppendToFile(KLogFilePath, buffer);
}

void LogTime(const char* fmt, ...) {
#ifdef SEMAPHORE
	RSemaphore semaphore;
	PanicIfError(semaphore.OpenGlobal(KSemaphoreName));
	semaphore.Wait();
#endif	//SEMAPHORE
	VA_LIST argptr;
	VA_START(argptr, fmt);
	LogV(fmt, argptr);

	TBuf8<32> buffer;
	_LIT8(KFormat, " @ time %d\n");
	buffer.Format(KFormat, User::TickCount());
	AppendToFile(KLogFilePath, buffer);
#ifdef SEMAPHORE
	semaphore.Signal();
	semaphore.Close();
#endif	//SEMAPHORE
}

void AppendToFile(const TDesC& aFileName, const TDesC8& aText) {
	{
#ifdef SEMAPHORE
	RSemaphore semaphore;
	PanicIfError(semaphore.OpenGlobal(KSemaphoreName));
	semaphore.Wait();
#endif	//SEMAPHORE
	MyRFs myrfs;
	myrfs.Connect();
	RFile file;
	PanicIfError(file.Open(FSS, aFileName,
		EFileWrite | EFileShareAny | EFileStreamText));
	TInt pos = 0;
	PanicIfError(file.Seek(ESeekEnd, pos));
	PanicIfError(file.Write(aText));
	file.Close();
	}
#ifdef SEMAPHORE
	semaphore.Signal();
	semaphore.Close();
#endif	//SEMAPHORE
}

void DumpToFileL(const TDesC& aFileName, const TDesC8& aData) {
	MyRFs myrfs;
	myrfs.Connect();
	FSS.Delete(aFileName);
	RFile file;
	LHEL(file.Create(FSS, aFileName, EFileWrite | EFileShareExclusive));
	LHEL(file.Write(aData));
	file.Close();
}

void LogAvailableMemory() {
	TInt availableMemory, biggestBlock;
	availableMemory = User::Available(biggestBlock);
	LOGD("HeapA: %d(%d).\n", availableMemory, biggestBlock);
}

#endif	//LOGGING_ENABLED

#ifdef _DEBUG
void failFunction() {
}
#endif

//***************************************************************************
//My stdlib
//***************************************************************************
#ifndef __WINS__
extern "C" {
//#ifndef _STRING_H_
char* strrchr(const char* str, int c) {
	TPtrC8 ptr(CBP str);
	int result = ptr.LocateReverse(c);
	if(result == KErrNotFound)
		return NULL;
	return (char*) (str + result);
}
//#endif

size_t strlen(const char* str) {
	TPtrC8 ptr(CBP str);
	return ptr.Length();	
}

int strcmp(const char* a, const char* b) {
	return memcmp(a, b, 1024*1024);	//hack
}
int strncmp(const char *a, const char *b, size_t len) {
	return memcmp(a, b, len);	//hack
}
int isdigit(int c) {
	return TChar(c).IsDigit();
}


int memcmp(const void* a, const void* b, size_t len) {
	return Mem::Compare(CBP a, len, CBP b, len);
}
void* memcpy(void* dst, const void* src, size_t len) {
	Mem::Copy(dst, src, len);
	return dst;
}
#ifndef __SERIES60_3X__	//Series 60, 2nd Ed.
void abort() {
	LOG("abort()\n");
	ShowAknErrorNoteThenExitL(_L("abort()"));
}
#endif

}
#endif	//__WINS__

//***************************************************************************
//Error handlers
//***************************************************************************

const char* FileNameFromPath(const char* path) {
	const char* ptr = strrchr(path, '\\');
	return ptr ? (ptr + 1) : path;
}

#ifdef PUBLIC_DEBUG
static void fillPanicReport(MAPanicReport& pr, int rt, int code) {
	pr.runtimeId = RUNTIME_SYMBIAN;
	pr.reportType = rt;
	pr.code = code;
	TTime ut;
	ut.UniversalTime();
	pr.time = unixTime(ut);
	pr.ip = -1;
	CAppUi* ui = (CAppUi*)(CEikonEnv::Static()->AppUi());
	const Core::VMCore* core = ui->GetCore();
	if(core) if(core->mem_cs) {
		pr.ip = Core::GetIp(core);
	}
}

static void dumpPanicReport(const void* data, int len) {
	_LIT8(KPanicReportFilename, "panic.report");
	TBuf8<64> path;
	path.Append(KMAStorePath8);
	path.Append(KPanicReportFilename);

	WriteFileStream file(CCP path.PtrZ());
	file.write(data, len);
}

void writePanicReport(int rt, int code) {
	MAPanicReport pr;
	fillPanicReport(pr, rt, code);
	dumpPanicReport(&pr, sizeof(pr));
}
void writePanicReport(int rt, int code, const TDesC& text) {
	int buflen = sizeof(MAPanicReport) + text.Length();
	//byte buffer[buflen]; // generates an 'illegal constant expression' in carbide
	byte* buffer = new byte[buflen];
	MAPanicReport& pr(*(MAPanicReport*)buffer);
	fillPanicReport(pr, rt, code);
	TPtr8 tptr((byte*) pr.string, text.Length());
	tptr.Copy(text);
	buffer[buflen - 1] = 0;	//needed?
	dumpPanicReport(buffer, buflen);
	delete[] buffer;
}
#endif

//This function handles error conditions that are so basic it'd be no use
//to try to log them or recover from them.
void PanicIfError(TInt code) {
	_LIT(KCodePanic, "MSRC");
	if(code < 0) {
		User::Panic(KCodePanic, code);
	}
}

void MoSyncExit(int LOG_ARG(result)) {
	LOG("MoSyncExit %i\n", result);
	CAppUi* ui = (CAppUi*)(CEikonEnv::Static()->AppUi());
#ifdef SUPPORT_RELOAD
	const Core::VMCore* core = ui->GetCore();
	if(core) if(core->currentSyscallId < 0) {
		ui->Exit();
		goto unreach;
	}

	if(ui->iReload)
		User::Leave(KLeaveMoSyncReload);
	else
		User::Leave(KLeaveMoSyncExit);
unreach:
#else
	ui->Exit();
#endif
	LOG("This code should not be reached.\n");
	Panic(EMoSyncLoggedPanic);
	while(1) {}
}

static void ShowAknErrorNote(const TDesC& text) {
#ifdef LOGGING_ENABLED
	TBuf8<128> buffer;
	buffer.Copy(text);
	LogBin(buffer.Ptr(), buffer.Size());
	if(buffer[buffer.Length()-1] != '\n') {
		LOG("\n");
	}
#endif
	CAppUi* ui = (CAppUi*)(CEikonEnv::Static()->AppUi());
	if(!ui->isExiting()) {
		ui->Stop();
		//TODO: the note disappears if the user presses any key.
		//make the note respond only to the OK key
		CAknErrorNote *note = new (ELeave) CAknErrorNote(true);
		note->SetTimeout(CAknNoteDialog::ENoTimeout);

		//the TRAPD macro causes a memory leak just by being compiled.
		//iff the function has an Exit call.
		//probably 'cause they allocate something that relies
		//on a destructor to release memory.
		TRAPD(res, note->ExecuteLD(text));

		if(IS_SYMBIAN_ERROR(res)) {
			LOG("AknErrorNote error %i\n", res);
		}
	}
}

void __declspec(noreturn) ShowAknErrorNoteThenExitL(const TDesC& text) {
	LOG("ShowAknErrorNoteThenExit\n");
#ifdef SUPPORT_RELOAD
	CAppUi* ui = (CAppUi*)(CEikonEnv::Static()->AppUi());
	if(ui->iReload)
		User::Leave(KLeaveMoSyncReload);
#endif
	ShowAknErrorNote(text);
	MoSyncExit(-1);
}
void __declspec(noreturn) ShowErrorCodeThenExitL(TInt aError) {
	LOG("Unhandled Symbian Error %d(0x%08X)\n", aError, aError);
#ifdef PUBLIC_DEBUG
	writePanicReport(REPORT_PLATFORM_CODE, aError);
#endif
	TBuf<64> buffer;
	buffer.Format(_L("Unhandled Symbian Error\n%d(0x%08X)"), aError, aError);
	ShowAknErrorNoteThenExitL(buffer);
}
void MoSyncErrorExit(int errorCode) {
	LOG("MoSync Panic %d\n", errorCode);
#ifdef PUBLIC_DEBUG
	writePanicReport(REPORT_PANIC, errorCode);
#endif
	CAppUi* ui = (CAppUi*)(CEikonEnv::Static()->AppUi());
	const Core::VMCore* core = ui->GetCore();
	TBuf<128> buffer;
	bool formatted = false;
	if(core) if(core->mem_cs) {
		TBuf<4> appCode(4);
		for(int i=0; i<4; i++) {
			appCode[i] = (byte)(core->Head.AppCode >> ((3-i)*8));
		}
#ifdef PUBLIC_DEBUG
		//TODO: check to see if running.
		if(SYSCALL_NUMBER_IS_VALID(core->currentSyscallId)) {
			buffer.Format(_L("MoSync Panic\np%d.s%d.i%x.%S"),
				errorCode, core->currentSyscallId, Core::GetIp(core), &appCode);
		} else {
			buffer.Format(_L("MoSync Panic\np%d.i%x.%S"),
				errorCode, Core::GetIp(core), &appCode);
		}
#else
		buffer.Format(_L("MoSync Panic\np%d.%S"), errorCode, &appCode);
#endif
		formatted = true;
	}

	if(!formatted) {
		buffer.Format(_L("MoSync Panic\np%d."), errorCode);
	}
	ShowAknErrorNoteThenExitL(buffer);
}

//***************************************************************************
//Utilities
//***************************************************************************

int unixTime(const TTime& tt) {
	_LIT(KEpochStart, "19700000:");
	TTime epochStart(KEpochStart);
	return I64INT((tt.Int64() - epochStart.Int64()) / 1000000);
}


HBufC8* LoadFileLC(const TDesC& aFileName) {
	HBufC8* buffer;
	{
		MyRFs myrfs;
		myrfs.Connect();
		RFile file;
		LHEL(file.Open(FSS, aFileName, EFileRead | EFileShareReadersOnly | EFileStream));
		CleanupClosePushL(file);
		TInt size;
		LHEL(file.Size(size));
		buffer = HBufC8::NewLC(size);
		TPtr8 ptr = buffer->Des();
		LHEL(file.Read(ptr));
		CleanupStack::Pop(buffer);
		CleanupStack::PopAndDestroy();  //file
	}
	CleanupStack::PushL(buffer);
	return buffer;
}

/*CFbsBitmap* LoadEncodedImageL(const TDesC8& aEncodedData) {
//Initialize decoder
CImageDecoder *decoder = CImageDecoder::DataNewL(FSS, aEncodedData,
//to avoid freeze at WaitForRequest. seems it doesn't work with Data decoders.
CImageDecoder::EOptionAlwaysThread);
CleanupStack::PushL(decoder);

//Create bitmap
const TFrameInfo& frameInfo = decoder->FrameInfo(0);
//TRect aFrameSize = frameInfo.iFrameCoordsInPixels;
CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
CleanupStack::PushL(bitmap);
//CCoeEnv* coeenv = CCoeEnv::Static();
//CGraphicsDevice* gd = coeenv->SystemGc().Device();
//TDisplayMode mode = gd->DisplayMode();
//bitmap->Create(aFrameSize.Size(), mode);
bitmap->Create(frameInfo.iOverallSizeInPixels, frameInfo.iFrameDisplayMode);

//Perform decoding
TRequestStatus status = KErrNone;
decoder->Convert(&status, *bitmap);
User::WaitForRequest(status);
LHEL(status.Int());

//Cleanup
CleanupStack::Pop(bitmap);
CleanupStack::PopAndDestroy(decoder);
return bitmap;
}*/

CFbsBitmapDevice* TAlphaBitmap::Device() {
	if(!mDevice) {
		mDevice = CFbsBitmapDevice::NewL(Color());
	}
	return mDevice;
}

void TAlphaBitmap::SetClipRect(const TRect& aRect) {
	mClipRect = aRect;
}

int LoadEncodedImageWAlphaL(const TDesC8& aEncodedData, TAlphaBitmap** ptab) {
	MyRFs myrfs;
	myrfs.Connect();
	//Initialize decoder
	_LIT8(KPngMime, "image/png");
	TCleaner<CImageDecoder> decoder(CImageDecoder::DataNewL(FSS, aEncodedData, KPngMime));
	CleanupStack::PushL(decoder());

	//Create bitmap
	const TFrameInfo& frameInfo = decoder->FrameInfo(0);
	LOG("LEIWA: %i bytes, %ix%i pixels, %x flags\n", aEncodedData.Size(),
		frameInfo.iOverallSizeInPixels.iWidth, frameInfo.iOverallSizeInPixels.iHeight,
		frameInfo.iFlags);

	TCleaner<CFbsBitmap> bmpClr(new CFbsBitmap);
	if(!bmpClr)
		return NULL;
	CleanupStack::PushL(bmpClr());

	CCoeEnv* coeenv = CCoeEnv::Static();
	CGraphicsDevice* gd = coeenv->SystemGc().Device();
	TDisplayMode mode = gd->DisplayMode();
	//frameInfo.iFrameDisplayMode);
	LHEL_BASE(bmpClr->Create(frameInfo.iOverallSizeInPixels, mode), return NULL);

	//Create Synchronizer  
	TCleaner<CLocalSynchronizer> sync(new CLocalSynchronizer);
	if(!sync)
		return NULL;
	CleanupStack::PushL(sync());

	//Create mask, if needed.
	TCleaner<CFbsBitmap> bmpMask(NULL);
	if(frameInfo.iFlags & TFrameInfo::ETransparencyPossible) {
		bmpMask = new CFbsBitmap;
		if(!bmpMask)
			return NULL;
		CleanupStack::PushL(bmpMask());
		LHEL_BASE(bmpMask->Create(frameInfo.iOverallSizeInPixels,
			(frameInfo.iFlags & TFrameInfo::EAlphaChannel) ? EGray256 : EGray2),
			return NULL);
		decoder->Convert(sync->Status(), *bmpClr, *bmpMask);  //Perform decoding
	} else {
		decoder->Convert(sync->Status(), *bmpClr);  //Perform decoding
	}
	sync->Wait();
	LHEL(sync->Status()->Int());

	//dump loaded bitmap
	//LHEL(bitmap->Save(_L("C:\\dump.bmp")));

	*ptab = new TAlphaBitmap(bmpClr(), bmpMask());
	if(!*ptab)
		return RES_OUT_OF_MEMORY;

	//Cleanup
	bmpMask.pop();
	sync.popAndDestroy();
	bmpClr.pop();
	decoder.popAndDestroy();
	return RES_OK;
}

CBufFlat* NewCBufFlatL(TInt size) {
	CBufFlat* self = CBufFlat::NewL(32);
	CleanupStack::PushL(self);
	self->ResizeL(size);
	CleanupStack::Pop(self);
	return self;
}