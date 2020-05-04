#ifndef __CATCHFISHINSTALLER_CATCHFISHINSTALLER_FEATURE_UPLOAD_UPLOADFILE_H__
#define __CATCHFISHINSTALLER_CATCHFISHINSTALLER_FEATURE_UPLOAD_UPLOADFILE_H__

#include <CatchFishServer/Feature/Common/Common.h>

class CUploadFile
{
public:
	CUploadFile() = default;
	~CUploadFile() = default;

	static CUploadFile& GetInstance();

	// 
	BOOL IsExistCompatibleFile(_In_ WCHAR wchDisk);

	//
	CONST std::wstring& GetGameType() CONST;

	//
	CONST std::wstring& GetGamePath() CONST;

	//
	BOOL UpLoadGameFile();

	//
	BOOL IsExistGameFileInServer(_Out_ BOOL& bResult) CONST;

	//
	DWORD GetUploadFilePercent() CONST;
private:
	std::wstring _wsGamePath;
	std::wstring _wsGameType;
	DWORD		 _dwUploadPercent = 0;
};


#endif // !__CATCHFISHINSTALLER_CATCHFISHINSTALLER_FEATURE_UPLOAD_UPLOADFILE_H__
