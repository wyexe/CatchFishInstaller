#include "Downloader.h"
#include <filesystem> // C++ 17
#include <iostream>
#include <CharacterLib/Character.h>
#include <FileLib/File.h>
#include <LogLib/Log.h>
#include <AlgroithmLib/Encrypt/CRC32.h>
#include <Feature/Res/ResText.h>
#include <ProcessLib/Common/ResHandleManager.h>

#ifdef _DEBUG
#pragma comment(lib,"AlgroithmLib_Debug.lib")
#else
#pragma comment(lib,"AlgroithmLib.lib")
#endif // _DEBUG

#define _SELF L"Downloader.cpp"
CDownloader::CDownloader()
{
	
}

CDownloader::~CDownloader()
{
	
}

CDownloader& CDownloader::GetInstance()
{
	static CDownloader Instance;
	return Instance;
}

BOOL CDownloader::Refresh()
{
	return RefreshDownFile() && RefreshUploadFile();
}

BOOL CDownloader::RefreshDownFile()
{
	using DirectoryPath = std::experimental::filesystem::v1::path;
	using DirectoryItr = std::experimental::filesystem::v1::directory_iterator;
	

	// 系列
	decltype(_VecFileContent) Vec;
	DirectoryPath DirPath(libTools::CCharacter::MakeCurrentPath(L"\\CatchFishFile\\"));
	for (DirectoryItr GameSeriesDirItr(DirPath); GameSeriesDirItr != DirectoryItr(); ++GameSeriesDirItr)
	{
		if (!std::experimental::filesystem::v1::is_directory(GameSeriesDirItr->path()))
			continue;
		

		// 名字
		for (DirectoryItr GameNameDirItr(GameSeriesDirItr->path()); GameNameDirItr != DirectoryItr(); ++GameNameDirItr)
		{
			if (!std::experimental::filesystem::v1::is_directory(GameNameDirItr->path()))
				continue;


			// 文件
			CatchFishFileStruct CatchFishFile;
			CatchFishFile.Identifier.wsGameType = GameSeriesDirItr->path().filename().wstring();
			CatchFishFile.Identifier.wsGameName = GameNameDirItr->path().filename().wstring();
			for (CONST auto& File : DirectoryItr(GameNameDirItr->path()))
			{
				CatchFishFileContent FileContent;
				FileContent.wsFileName = File.path().filename().wstring();


				LPVOID lpFileContent = nullptr;
				SIZE_T uFileSize	 = 0;
				if (!libTools::CFile::ReadFileContent(File.path().wstring(), lpFileContent, uFileSize))
				{
					LOG_MSG_CF(L"ReadFileContent = FALSE, wsPath=[%s]", File.path().wstring().c_str());
					return FALSE;
				}


				FileContent.FileContent = std::shared_ptr<CHAR>(new CHAR[uFileSize], [](CHAR* p) { delete[] p; });
				FileContent.uFileSize = uFileSize;
				memcpy(FileContent.FileContent.get(), lpFileContent, uFileSize);

				FileContent.dwCRC32 = libTools::CCRC32::GetCRC32(FileContent.FileContent.get(), FileContent.uFileSize);

				// release
				::VirtualFree(lpFileContent, 0, MEM_RELEASE);
				lpFileContent = nullptr;


				CatchFishFile.VecFileContent.push_back(FileContent);
			}


			std::wstring wsKey = CatchFishFile.Identifier.wsGameType + L"-" + CatchFishFile.Identifier.wsGameName;
			std::wcout << L"Add [" << CatchFishFile.Identifier.wsGameType.c_str() << L" , " << CatchFishFile.Identifier.wsGameName.c_str() << L"]" << std::endl;
			Vec.insert(std::make_pair(wsKey, CatchFishFile));
		}
	}
	std::wcout << L"Refresh.Count = " << Vec.size() << std::endl;

	Lock();
	_VecFileContent = Vec;
	UnLock();
	return TRUE;
}

BOOL CDownloader::GetDownLoadFile(_In_ CONST GameIdentity& Identifier, _Out_ std::vector<CatchFishFileContent>& Vec)
{
	BOOL bExist = FALSE;

	Lock();
	auto itr = _VecFileContent.find(Identifier.wsGameType + L"-" + Identifier.wsGameName);
	if (itr != _VecFileContent.end())
	{
		Vec = itr->second.VecFileContent;
		bExist = TRUE;
	}


	UnLock();
	return bExist;
}

BOOL CDownloader::IsExistUploadFileCRC(_In_ DWORD dwCRC32)
{
	std::lock_guard<libTools::CThreadLock> _PtrLock(_UploadVecLock);

	return _VecUploadFileCRC.find(dwCRC32) != _VecUploadFileCRC.end();
}

VOID CDownloader::AddUploadFileCRC(_In_ DWORD dwCRC32)
{
	std::lock_guard<libTools::CThreadLock> _PtrLock(_UploadVecLock);
	_VecUploadFileCRC.insert(dwCRC32);
}

BOOL CDownloader::RefreshUploadFile()
{
	using DirectoryPath = std::experimental::filesystem::v1::path;
	using DirectoryItr = std::experimental::filesystem::v1::directory_iterator;
	std::lock_guard<libTools::CThreadLock> _PtrLock(_UploadVecLock);


	// Read File CRC in Directory 'WaitToFixList'
	DirectoryPath DirPath(libTools::CCharacter::MakeCurrentPath(L"\\WaitToFixList\\"));
	for (DirectoryItr GameSeriesDirItr(DirPath); GameSeriesDirItr != DirectoryItr(); ++GameSeriesDirItr)
	{
		if (!std::experimental::filesystem::v1::is_directory(GameSeriesDirItr->path()))
			continue;


		// TimeDir
		for (DirectoryItr TimeDirItr(GameSeriesDirItr->path()); TimeDirItr != DirectoryItr(); ++TimeDirItr)
		{
			if (!std::experimental::filesystem::v1::is_directory(TimeDirItr->path()))
				continue;


			for (CONST auto& File : DirectoryItr(TimeDirItr->path()))
			{
				LPVOID lpFileContent = nullptr;
				SIZE_T uFileSize = 0;

				if (!libTools::CFile::ReadFileContent(File.path().wstring(), lpFileContent, uFileSize))
				{
					std::wcout << L"读取上传文件[" << File.path().wstring().c_str() << L"] 失败!" << std::endl;
					continue;
				}


				SetResDeleter(lpFileContent, [](LPVOID& p) { ::VirtualFree(p, 0, MEM_RELEASE); });
				_VecUploadFileCRC.insert(libTools::CCRC32::GetCRC32(reinterpret_cast<LPCSTR>(lpFileContent), uFileSize));
			}
		}
	}

	// Read File CRC in Res.json
	CResText::GetInstance().ActionToJsonFileContent([&](JsonFileContent& JsonContent)
	{
		for (auto& itm : JsonContent.VecContent)
		{
			_VecUploadFileCRC.insert(itm.dwFileCRC32);
		}
	});

	return TRUE;
}
