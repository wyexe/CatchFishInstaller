#include "Downloader.h"
#include <CatchFishInstaller/Feature/Socket/Client/IocpClient.h>
#include <LogLib/Log.h>
#include <AlgroithmLib/Encrypt/CRC32.h>

#ifdef _DEBUG
#pragma comment(lib,"AlgroithmLib_Debug.lib")
#else
#pragma comment(lib,"AlgroithmLib.lib")
#endif // _DEBUG


#define _SELF L"Downloader.cpp"
CDownloader& CDownloader::GetInstance()
{
	static CDownloader Instance;
	return Instance;
}

BOOL CDownloader::Download_CheatFile(_In_ CONST GameIdentity& Identifier, _Out_ std::vector<CatchFishFileContent>& Vec)
{
	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Download_File);
	SendSocketBuffer << Identifier.wsGameType << Identifier.wsGameName;
	return CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [&Vec](libTools::CSocketBuffer& SocketBuffer)
	{
		UINT uSize = 0;
		SocketBuffer >> uSize;


		Vec.clear();
		for (UINT i = 0; i < uSize; ++i)
		{
			CatchFishFileContent Content;
			DWORD dwCRC32 = 0;
	

			SocketBuffer >> Content.wsFileName >>  dwCRC32 >> Content.uFileSize;
			Content.FileContent = SocketBuffer.GetSizeBuffer(Content.uFileSize);
			if (libTools::CCRC32::GetCRC32(Content.FileContent.get(), Content.uFileSize) != dwCRC32)
			{
				Vec.clear();
				LOG_MSG_CF(L"文件[%s]下载后的CRC校验错误! ServerCRC=[%08X], LocalCRC=[%08X]", Content.wsFileName.c_str(), Content.dwCRC32, libTools::CCRC32::GetCRC32(Content.FileContent.get(), Content.uFileSize));
				return;
			}


			Vec.push_back(Content);
		}
	});
}

VOID CDownloader::Upload_InstallRecord(_In_ CONST InstallDiskRecord_Tranfer& Content)
{
	libTools::CSocketBuffer SocketBuffer(em_Sock_Msg_Record_Install);
	SocketBuffer.AddStruct(Content);
	CIocpClient::GetInstance().SyncSend(SocketBuffer, [](libTools::CSocketBuffer&) {});
}

DWORD CDownloader::GetGenerateCodeCRC32()
{
	DWORD dwCRC32 = 0;

	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Version_File);
	SendSocketBuffer << L"Installer" << L"GenerateCode";
	CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [&dwCRC32](libTools::CSocketBuffer& SocketBuffer)
	{
		SocketBuffer >> dwCRC32;
	});

	return dwCRC32;
}

BOOL CDownloader::GetGenerateCode(_In_ CONST GameIdentity& Identifier, _In_ std::wstring& wsDiskSerialNumber, _Out_ std::wstring& wsCountCode, _Out_ std::wstring& wsTestCode)
{
	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Generate_TestCode);
	SendSocketBuffer << wsDiskSerialNumber << Identifier.wsGameType << Identifier.wsGameName;
	return CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [&](libTools::CSocketBuffer& SocketBuffer) 
	{
		SocketBuffer >> wsDiskSerialNumber >> wsTestCode >> wsCountCode;
	});
}

BOOL CDownloader::Download_GameIdentifier(std::vector<CatchFishStructContent>& Vec)
{
	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Download_Identifier);
	return CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [&Vec](libTools::CSocketBuffer& SocketBuffer)
	{
		UINT uSize = 0;
		SocketBuffer >> uSize;
		for (UINT i = 0;i < uSize; ++i)
		{
			Vec.push_back(SocketBuffer.GetStruct<CatchFishStructContent>());
		}
	});
}

BOOL CDownloader::Download_DumpTool(std::vector<CatchFishFileContent>& Vec)
{
	GameIdentity Identifier{ L"Installer", L"DumpTool" };
	return Download_CheatFile(Identifier, Vec);
}
