#ifndef __CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_INSTALL_INSTALLER_H__
#define __CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_INSTALL_INSTALLER_H__

#include <Windows.h>
#include <string>
#include <vector>
#include <functional>
#include <AclAPI.h>
#include <CatchFishServer/Feature/Common/Common.h>

 
class CInstaller
{
public:
	CInstaller();
	~CInstaller() = default;

	static CInstaller& GetInstance();

	BOOL Install(_In_ CONST std::wstring& wsResultPath);

	BOOL InstallDumpTool();

	BOOL PullLog();

	BOOL UnInstall();

	VOID SetIngoreDiskSerialNumber();

	BOOL IsExistGame(_In_ CONST std::wstring& wsPath);

	VOID RunTest();

	// 清除还原
	VOID ClearRestoreSystem();

	// 清空次数
	VOID ClearTheCount();

	// 生成码
	std::wstring GenerateEncodeSerailNumber(_In_ CONST std::wstring& wsSerialNumber) CONST;
private:
	VOID CheckDirectory();

	BOOL IsMatchGameRecognitionDirectory(_In_ CONST CatchFishStructContent& Content) CONST;

	BOOL GetGameIdentifier(_Out_ GameIdentity& Ideneifier);

	BOOL ReadGameApplicationValue(_In_ CONST std::wstring& wsFilePath, _In_ DWORD dwAddr, _Out_ DWORD& dwValue);

	BOOL IsGameApplicationEqualCRC(_In_ CONST std::wstring& wsFilePath, _In_ DWORD dwCRC32) CONST;

	BOOL IsGameApplicationEqualFileSize(_In_ CONST std::wstring& wsFilePath, _In_ UINT uFileSize) CONST;

	BOOL CopyCheatFileToSrc(_In_ CONST GameIdentity& Ideneifier);

	BOOL CopyDumpFileToSrc();

	VOID PrintSupportGameIdentifier() CONST;

	BOOL GetDiskSerialNumber(_In_ WCHAR wchDisk);

	BOOL Initialize(_In_ CONST std::wstring& wsResultPath);

	BOOL SetSerialNumberDiskIsUnFormatDisk();

	BOOL SetSerialNumberDiskIsUnFormatDisk_IsUsb();

	std::wstring GetSystemPhysicalDrive() CONST;

	BOOL GetDevicePhysicalDiskNumber(_In_ LPCWSTR pwszDevice, _Out_ std::wstring& wsPhysicalDiskNumber) CONST;

	BOOL IsDownloadGenerateCodeDLL() CONST;

	std::wstring GetCurrentPath() CONST;

	BOOL CreateInjectorProcFile() CONST;

	VOID InitializeMemoryShare();

	VOID ViewTestCode(_In_ CONST GameIdentity& Ideneifier);

	BOOL FillFishViewFile(_In_ CONST std::wstring& wsPath, _In_ CONST std::wstring& wsCounterCode, _In_ CONST std::wstring& wsCheatCode, _In_ CONST std::wstring& wsUnDeadCode);

	BOOL GenerateRandSerialNumber(_Out_ std::wstring& wsRandSerailNumber) CONST;

	// 填充往服务器发的安装内容
	BOOL FillInstallDiskRecord(_Out_ InstallDiskRecord_Tranfer& Content) CONST;

	// 获取所有硬盘的内容
	BOOL GenerateAllDisk(_Out_ InstallDiskRecord_Tranfer& Content) CONST;

	// 释放孙哥Disk DLL
	BOOL ReleaseDiskDLL(_Out_ std::wstring& wsText) CONST;

	// 
	BOOL SetPrivilege(_In_ HANDLE hToken, _In_ LPCTSTR lpszPrivilege, _In_ BOOL bEnablePrivilege) CONST;

	//
	LRESULT MySetSecurity(LPTSTR str_name, SE_OBJECT_TYPE e_type) CONST;

	// 重新设置相同硬盘下的逻辑磁盘
	BOOL FindOtherDisk_By_Disk(_In_ WCHAR wchDisk, _Out_ WCHAR& wchOtherDisk) CONST;

	// 保存到加密日志
	VOID SaveToEncryptLog(_In_ CONST InstallDiskRecord_Tranfer& Content, _In_ CONST std::wstring& wsFilePath, _In_ CONST std::wstring& wsTestCount, _In_ CONST std::wstring& wsCountCode) CONST;
private:
	WCHAR								_wchSerialNumberDisk;
	WCHAR								_wchDisk;
	std::wstring						_wsDiskSerialNumber;
	std::vector<CatchFishStructContent> _VecGameRecognition;
	std::vector<CatchFishFileContent>   _VecCheatFile;
	std::wstring						_wsCopyToDestPath;
	std::vector<WCHAR>					_VecDisk;
	std::vector<GameIdentity>			_VecCloseIdentifier;
	std::vector<CatchFishStructContent> _VecCatchFishStructContent;
	std::vector<CatchFishFileContent>   _VecGenerateCodeContent;
	std::wstring						_wsResultPath;
	std::wstring						_wsInjectorPathFile;
	CatchFishStructContent*				_pFishGameStructContent = nullptr;
	DownloaderMemoryShareContent*		_pMemoryShareContent = nullptr;
	BOOL								_IsIngoreDiskSerialNumber = FALSE;
};



#endif // !__CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_INSTALL_INSTALLER_H__
