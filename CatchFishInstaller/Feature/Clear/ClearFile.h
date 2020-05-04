#ifndef __CATCHFISHINSTALLER_CATCHFISHINSTALLER_FEATURE_CLEAR_CLEARFILE_H__
#define __CATCHFISHINSTALLER_CATCHFISHINSTALLER_FEATURE_CLEAR_CLEARFILE_H__

#include <Windows.h>
#include <string>
#include <functional>
#include <CatchFishServer/Feature/Common/Common.h>

class CClearFile
{
public:
	enum class em_System
	{
		Unknow,
		Win7x86,
		Win7x64,
		XP
	};
public:
	CClearFile() = default;
	~CClearFile() = default;

	static CClearFile& GetInstance();

	// 
	BOOL ClearDiskSectorSpecifyItem() CONST;

	//
	BOOL Clear(_In_ WCHAR wchDisk, _In_ CONST std::wstring& wsGameDir);

	//
	BOOL ClearGameRestoreSystem(_In_ WCHAR wchDisk, _In_ CONST GameIdentity& Identifier) CONST;

	//
	BOOL ClearGameRestoreSystem_YouLingQianTing(_In_ CONST CHAR* fileFullPach) CONST;

	//
	static em_System GetDiskSystem(_In_ WCHAR wchDisk);

	// 禁止删除游戏目录里的所有文件
	VOID SetIsDisableDeleteGameDirectoryInjectorFile(_In_ BOOL bValue);
private:
	// 清空游戏目录的恶意文件
	BOOL ClearGameDirectory(_In_ CONST std::wstring& wsGameDir) CONST;

	// 清空系统目录的恶意文件
	BOOL ClearSystemDirectory() CONST;

	// 清空日志文件
	BOOL ClearSelfLogFile(_In_ WCHAR wchDisk) CONST;
private:
	// 
	BOOL _IsDisableDeleteGameDirectoryInjectorFile = FALSE;

	//
	WCHAR _wchDisk = NULL;
};



#endif // !__CATCHFISHINSTALLER_CATCHFISHINSTALLER_FEATURE_CLEAR_CLEARFILE_H__
