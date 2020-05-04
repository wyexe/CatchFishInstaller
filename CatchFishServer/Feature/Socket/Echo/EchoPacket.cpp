#include "EchoPacket.h"
#include <iostream>
#include <filesystem>
#include <CharacterLib/Character.h>
#include <ProcessLib/Lock/Lock.h>
#include <LogLib/Log.h>
#include <FileLib/File.h>
#include <AlgroithmLib/Encrypt/CRC32.h>
#include <Feature/Socket/RemoteClient/RemoteClient.h>
#include <Feature/Download/Downloader.h>
#include <Feature/Statistics/StatisticsRecorder.h>
#include <ProcessLib/Process/Process.h>

#ifdef _DEBUG
#pragma comment(lib,"AlgroithmLib_Debug.lib")
#else
#pragma comment(lib,"AlgroithmLib.lib")
#endif // _DEBUG


#define _SELF L"EchoPacket.cpp"

std::queue<std::wstring> CEchoPacket::_VecMail;
libTools::CThreadLock	 CEchoPacket::_LockVecMail;

BOOL CEchoPacket::KeepALive(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer)
{
	pRemoteClient->SetKeepALive();
	pSocketBuffer->clear();
	pSocketBuffer->InitializeHead(em_Sock_Msg_KeepLive);
	return TRUE;
}

BOOL CEchoPacket::DownloadFile(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer)
{
	GameIdentity Identifer;
	*pSocketBuffer >> Identifer.wsGameType >> Identifer.wsGameName;
	pSocketBuffer->clear();


	std::vector<CatchFishFileContent> Vec;
	if (!CDownloader::GetInstance().GetDownLoadFile(Identifer, Vec))
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"无效的文件[%s-%s]", Identifer.wsGameType.c_str(), Identifer.wsGameName.c_str());
		return TRUE;
	}


	AddTextToVecMail(libTools::CCharacter::MakeFormatText(L"正在安装游戏[%s-%s], IP=[%s]", Identifer.wsGameType.c_str(), Identifer.wsGameName.c_str(), pRemoteClient->GetRemoteClientIp().c_str()));


	LOG_C_D(L"正在下载游戏文件[%s-%s]", Identifer.wsGameType.c_str(), Identifer.wsGameName.c_str());
	// Size + FileName + CRC32 + Length + FileContent + ...
	pSocketBuffer->InitializeHead(em_Sock_Msg_Download_File);
	*pSocketBuffer << Vec.size();
	for (CONST auto& itm : Vec)
	{
		*pSocketBuffer << itm.wsFileName << itm.dwCRC32;
		*pSocketBuffer << itm.uFileSize;
		pSocketBuffer->SetDataPtr(itm.FileContent, itm.uFileSize);
	}


	pRemoteClient->SetKeepALive();
	return TRUE;
}

BOOL CEchoPacket::InstallRecord(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer)
{
	auto InstallDiskRecordContent = pSocketBuffer->GetStruct<InstallDiskRecord_Tranfer>();
	InstallDiskRecordContent.wsClientIp = pRemoteClient->GetRemoteClientIp();

	// Generate Every Disk TestCode & CountCode
	for (auto& itm : InstallDiskRecordContent.Vec)
	{
		type_OutInfo Content = { 0 };
		Content.n次数 = Content.n_shu = Content.n_yin = 1;
		libTools::CCharacter::strcpy_my(Content.n_as原始id, libTools::CCharacter::UnicodeToASCII(itm.wsDiskSerialNumber).c_str());
		if (CResText::GetInstance().GenerateCode(InstallDiskRecordContent.Identifier, Content))
		{
			itm.wsTestCode = libTools::CCharacter::ASCIIToUnicode(Content.as测试码);
			itm.wsCountCode = libTools::CCharacter::ASCIIToUnicode(Content.as取次码);
			itm.wsEncodeSerialNumber = libTools::CCharacter::ASCIIToUnicode(Content.as加密ID);
		}

		CResText::GetInstance().ActionToJsonFileContent([InstallDiskRecordContent, &itm](CONST JsonFileContent & JContent)
			{
				auto itr = std::find_if(JContent.VecContent.begin(), JContent.VecContent.end(), [InstallDiskRecordContent](CONST CatchFishStructContent & Fish)
					{
						return Fish.Identifier == InstallDiskRecordContent.Identifier;
					});
				if (itr != JContent.VecContent.end())
				{
					itm.wsLaoBanEncodeSerialNumber = GenerateEncodeSerailNumber(itm.wsEncodeSerialNumber, itr->dwSeriesSerialNumber);
				}
			});
	}

	DWORD dwInstallId = CStatisticsRecorder::GetInstance().AddInstallRecord(InstallDiskRecordContent);
	if (dwInstallId != 0)
	{
		CStatisticsRecorder::GetInstance().AddInstallDiskRecord(dwInstallId, InstallDiskRecordContent);
	}


	pSocketBuffer->clear();
	pSocketBuffer->InitializeHead(em_Sock_Msg_Record_Install);
	pRemoteClient->SetKeepALive();
	return TRUE;
}

BOOL CEchoPacket::SetInstallerVersion(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer)
{
	GameIdentity Identifer;
	*pSocketBuffer >> Identifer.wsGameType >> Identifer.wsGameName;

	std::vector<CatchFishFileContent> Vec;
	if (!CDownloader::GetInstance().GetDownLoadFile(Identifer, Vec))
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"无效的文件[%s-%s]", Identifer.wsGameType.c_str(), Identifer.wsGameName.c_str());
		return TRUE;
	}



	pSocketBuffer->InitializeHead(em_Sock_Msg_Version_File);
	*pSocketBuffer << Vec.begin()->dwCRC32;
	pRemoteClient->SetKeepALive();
	return TRUE;
}

BOOL CEchoPacket::GenerateTestCode(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer)
{
	// SerailNumber, Version
	std::wstring wsSerialNumber;
	GameIdentity Identifier;

	*pSocketBuffer >> wsSerialNumber >> Identifier.wsGameType >> Identifier.wsGameName;

	BOOL bExist = FALSE, bNewFramework = FALSE;
	bExist = CResText::GetInstance().ActionToCatchFishStructContent_If_Exist(Identifier, [&bNewFramework](CONST CatchFishStructContent & itm) {bNewFramework = itm.IsNewFramework; });
	if (!bExist)
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"不存在游戏[%s-%s]", Identifier.wsGameType.c_str(), Identifier.wsGameName.c_str());
		return FALSE;
	}

	type_OutInfo Content = { 0 };
	Content.n次数 = Content.n_shu = Content.n_yin = 1;
	libTools::CCharacter::strcpy_my(Content.n_as原始id, libTools::CCharacter::UnicodeToASCII(wsSerialNumber).c_str());
	if (!CResText::GetInstance().GenerateCode(Identifier, Content))
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << L"生成测试码失败!";
		return FALSE;
	}


	std::wstring wsEncryptDiskSerialNumber = libTools::CCharacter::ASCIIToUnicode(Content.as加密ID), wsTestCode = libTools::CCharacter::ASCIIToUnicode(bNewFramework ? Content.as私活码 : Content.as测试码), wsQueryCountCode = libTools::CCharacter::ASCIIToUnicode(Content.as取次码);


	AddTextToVecMail(libTools::CCharacter::MakeFormatText(L"安装包生成测试码  IP:[%s] 游戏[%s-%s] 测试码[%s], 取次码[%s]",
		pRemoteClient->GetRemoteClientIp().c_str(),
		Identifier.wsGameType.c_str(),
		Identifier.wsGameName.c_str(),
		wsTestCode.c_str(),
		wsQueryCountCode.c_str()));



	LOG_C_D(L"安装包生成测试码  IP:[%s] 游戏[%s-%s] 测试码[%s], 取次码[%s]",
		pRemoteClient->GetRemoteClientIp().c_str(),
		Identifier.wsGameType.c_str(),
		Identifier.wsGameName.c_str(),
		wsTestCode.c_str(),
		wsQueryCountCode.c_str());


	pSocketBuffer->InitializeHead(em_Sock_Msg_Generate_TestCode);
	*pSocketBuffer << wsEncryptDiskSerialNumber << wsTestCode << wsQueryCountCode;
	return TRUE;
}

BOOL CEchoPacket::GenerateStandardCode(_In_ libTools::CSocketRemoteClient * pRemoteClient, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	GenerateCodeContent Content = pSocketBuffer->GetStruct<GenerateCodeContent>();
	type_OutInfo GenerateCodeParam = { 0 };
	GenerateCodeParam.n还原系统随机码 = CResText::GetInstance().ActionToCatchFishStructContent_If_Exist(Content.Identifier, [](CONST CatchFishStructContent&) {}) ? Content.dwRandCode : -1;


	std::wstring wsCodeFormatText;
	if (Content.wsCodeType == L"赢分码")
	{
		GenerateCodeParam.n_shu = CResText::GetInstance().ConvertScoreTextToIndex(em_GenerateCode_ScoreType::LoseScore, Content.wsLoseScore) + 1;
		GenerateCodeParam.n_yin = CResText::GetInstance().ConvertScoreTextToIndex(em_GenerateCode_ScoreType::WinScore, Content.wsWinScore) + 1;
		wsCodeFormatText = libTools::CCharacter::MakeFormatText(L"赢[%s]币, 输[%s]币", Content.wsWinScore.c_str(), Content.wsLoseScore.c_str());
	}
	else if (Content.wsCodeType == L"杀分码")
	{
		GenerateCodeParam.n_shu = 0;
		GenerateCodeParam.n_yin = CResText::GetInstance().ConvertScoreTextToIndex(em_GenerateCode_ScoreType::KillScore, Content.wsLoseScore);
		wsCodeFormatText = libTools::CCharacter::MakeFormatText(L"[%s]", Content.wsLoseScore.c_str());
	}
	else if (Content.wsCodeType == L"测试码")
	{
		GenerateCodeParam.n_shu = 0;
		GenerateCodeParam.n_yin = 0;
	}
	else if (Content.wsCodeType == L"不死码")
	{
		GenerateCodeParam.n_shu = 0;
		GenerateCodeParam.n_yin = 4;
	}
	else
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << L"该打码类型无效:" + Content.wsCodeType;
	}


	GenerateCodeParam.n次数 = Content.dwCodeIndex;
	libTools::CCharacter::strcpy_my(GenerateCodeParam.n_as原始id, libTools::CCharacter::UnicodeToASCII(Content.wsOriginDiskSerialNumber).c_str(), _countof(GenerateCodeParam.n_as原始id));
	libTools::CCharacter::strcpy_my(GenerateCodeParam.n_as加密id, libTools::CCharacter::UnicodeToASCII(Content.wsEncryptDiskSerialNumber).c_str(), _countof(GenerateCodeParam.n_as加密id));
	if (!CResText::GetInstance().GenerateCode(Content.Identifier, GenerateCodeParam))
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << L"生成赢分码失败!";
		return FALSE;
	}

	std::wstring wsCheatCode;
	if (Content.IsHideCode)
		wsCheatCode = libTools::CCharacter::ASCIIToUnicode(Content.wsCodeType == L"测试码" ? GenerateCodeParam.dat_Hide.as私活码 : GenerateCodeParam.dat_Hide.n次数_as次数码);
	else
		wsCheatCode = libTools::CCharacter::ASCIIToUnicode(Content.wsCodeType == L"测试码" ? GenerateCodeParam.as私活码 : GenerateCodeParam.n次数_as次数码);

	// 生成正式码
	pSocketBuffer->InitializeHead(em_Sock_Msg_Generate_StandardCode);
	std::wstring wsQueryCode = libTools::CCharacter::ASCIIToUnicode(Content.IsHideCode ? GenerateCodeParam.dat_Hide.as取次码 : GenerateCodeParam.as取次码);
	*pSocketBuffer << wsCheatCode << wsQueryCode;



	AddTextToVecMail(libTools::CCharacter::MakeFormatText(L"打码器生成标准码: IP:[%s], 游戏:[%s-%s] IsHide=[%d] 次数:[%d], 硬盘序列号[%s], 类型:[%s] %s 码:[%s]",
		pRemoteClient->GetRemoteClientIp().c_str(),
		Content.Identifier.wsGameType.c_str(),
		Content.Identifier.wsGameName.c_str(),
		Content.IsHideCode,
		Content.dwCodeIndex,
		Content.wsEncryptDiskSerialNumber.c_str(),
		Content.wsCodeType.c_str(),
		wsCodeFormatText.c_str(),
		wsCheatCode.c_str()));


	// 记录
	CStatisticsRecorder::GetInstance().AddGenerateCodeRecord(Content, L"Self");
	return TRUE;
}

BOOL CEchoPacket::Generate_Specify_StandCode(_In_ libTools::CSocketRemoteClient * pRemoteClient, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	Tranfer_SpecifyCustomerContent TranferContent = pSocketBuffer->GetStruct<Tranfer_SpecifyCustomerContent>();


	BOOL bExist = FALSE;
	SpecifyCustomerContent CustomerContent;
	CResText::GetInstance().ActionToJsonFileContent([TranferContent, &CustomerContent, &bExist](CONST JsonFileContent & JContent)
		{
			auto itr = std::find_if(JContent.VecSpecifyCustomerContent.begin(), JContent.VecSpecifyCustomerContent.end(), [TranferContent](CONST SpecifyCustomerContent & Content)
				{
					return Content.IdentityText == TranferContent.IdentifyText;
				});


			if (itr != JContent.VecSpecifyCustomerContent.end())
			{
				bExist = TRUE;
				CustomerContent = *itr;
			}
		});


	if (!bExist)
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"无效的Identifier[%s]", TranferContent.IdentifyText.c_str());
	}
	else
	{
		type_OutInfo GenerateCodeParam = { 0 };
		GenerateCodeParam.n还原系统随机码 = CResText::GetInstance().ActionToCatchFishStructContent_If_Exist(CustomerContent.Identifier, [](CONST CatchFishStructContent&) {}) ? TranferContent.dwRandCode : -1;
		LOG_C_D(L"RandCode=%d", GenerateCodeParam.n还原系统随机码);
		if (TranferContent.wsWinScore == L"杀分码")
		{
			GenerateCodeParam.n_shu = 0;
			GenerateCodeParam.n_yin = CResText::GetInstance().ConvertScoreTextToIndex(em_GenerateCode_ScoreType::KillScore, TranferContent.wsLoseScore) + 3;
		}
		else
		{
			GenerateCodeParam.n_shu = CResText::GetInstance().ConvertScoreTextToIndex(em_GenerateCode_ScoreType::LoseScore, TranferContent.wsLoseScore) + 1;
			GenerateCodeParam.n_yin = CResText::GetInstance().ConvertScoreTextToIndex(em_GenerateCode_ScoreType::WinScore, TranferContent.wsWinScore) + 1;
		}
		GenerateCodeParam.n次数 = TranferContent.dwCodeIndex;


		libTools::CCharacter::strcpy_my(GenerateCodeParam.n_as加密id, libTools::CCharacter::UnicodeToASCII(CustomerContent.EncryptDiskSerialNumber).c_str(), _countof(GenerateCodeParam.n_as加密id));
		if (!CResText::GetInstance().GenerateCode(CustomerContent.Identifier, GenerateCodeParam))
		{
			pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
			*pSocketBuffer << L"生成赢分码失败!";
			return FALSE;
		}


		pSocketBuffer->InitializeHead(em_Sock_Msg_Generate_Specify_StandCode);
		*pSocketBuffer << libTools::CCharacter::ASCIIToUnicode(GenerateCodeParam.n次数_as次数码) << libTools::CCharacter::ASCIIToUnicode(GenerateCodeParam.as取次码);


		// 记录
		GenerateCodeContent Content;
		Content.dwCodeIndex = TranferContent.dwCodeIndex;
		Content.dwRandCode = TranferContent.dwRandCode;
		Content.Identifier = CustomerContent.Identifier;
		Content.wsEncryptDiskSerialNumber = CustomerContent.EncryptDiskSerialNumber;
		Content.wsCodeType = L"赢分码";
		Content.wsLoseScore = TranferContent.wsLoseScore;
		Content.wsWinScore = TranferContent.wsWinScore;


		LOG_C_D(L"子打码器生成标准码  IP:[%s], 游戏:[%s-%s] 次数:[%d], 赢分码[%s]",
			pRemoteClient->GetRemoteClientIp().c_str(),
			Content.Identifier.wsGameType.c_str(),
			Content.Identifier.wsGameName.c_str(),
			Content.dwCodeIndex,
			TranferContent.wsWinScore.c_str());


		AddTextToVecMail(libTools::CCharacter::MakeFormatText(L"子打码器生成标准码  IP:[%s], 游戏:[%s-%s] 次数:[%d], 赢分[%d]币 硬盘序列号:[%s]",
			pRemoteClient->GetRemoteClientIp().c_str(),
			Content.Identifier.wsGameType.c_str(),
			Content.Identifier.wsGameName.c_str(),
			Content.dwCodeIndex,
			TranferContent.wsWinScore.c_str(),
			Content.wsEncryptDiskSerialNumber.c_str()));


		CStatisticsRecorder::GetInstance().AddGenerateCodeRecord(Content, L"Customer");
	}
	return TRUE;
}


std::wstring CEchoPacket::GenerateEncodeSerailNumber(_In_ CONST std::wstring & wsSerialNumber, _In_ DWORD dwSeriesSerialNumber)
{
	//return libTools::CCharacter::MakeFormatText(L"FTGC%04X%s", dwSeriesSerialNumber, wsSerialNumber.c_str());
	std::wstring wsDiskSerialNumber;
	for (UINT i = 0; i < wsSerialNumber.size(); i += 2)
	{
		WCHAR wszText[3] = { 0 };
		wszText[0] = wsSerialNumber[i];
		wszText[1] = wsSerialNumber[i + 1];


		wsDiskSerialNumber += libTools::CCharacter::MakeFormatText(L"%02x", std::stoul(wszText, nullptr, 16) ^ 0xFF);
	}

	return libTools::CCharacter::MakeFormatText(L"FTGD%04X%s", dwSeriesSerialNumber, wsDiskSerialNumber.c_str());
}

BOOL CEchoPacket::EnumSetWinName(HWND hWnd, LPARAM lpParam)
{
	if (IsWindow(hWnd) && IsWindowVisible(hWnd) && (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) != WS_EX_TOOLWINDOW && (GetWindowLong(hWnd, GWL_HWNDPARENT) == 0))
	{
		WCHAR wszWinText[64] = { 0 };
		WCHAR wszWinClass[64] = { 0 };

		if (GetClassNameW(hWnd, wszWinClass, _countof(wszWinClass) - 1) > 0 && GetWindowTextW(hWnd, wszWinText, _countof(wszWinText) - 1) > 0)
		{
			if (std::wstring(wszWinText) == L"MailForm" && std::wstring(wszWinClass).find(L"WindowsForms10.Window") != std::wstring::npos)//如果能遍历到这个,并且是
			{
				MailFormContent* pContent = reinterpret_cast<MailFormContent*>(lpParam);
				if (pContent == nullptr)
				{
					return TRUE;
				}


				DWORD PID;
				::GetWindowThreadProcessId(hWnd, &PID);
				if (pContent != nullptr && PID == pContent->dwPid)
				{
					pContent->hWnd = hWnd;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

BOOL CEchoPacket::GetFishList(_In_ libTools::CSocketRemoteClient*, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	pSocketBuffer->InitializeHead(em_Sock_Msg_Get_FishList);
	CResText::GetInstance().ActionToJsonFileContent([&pSocketBuffer](CONST JsonFileContent & JContent)
		{
			*pSocketBuffer << JContent.VecContent.size();
			for (CONST auto& itm : JContent.VecContent)
			{
				*pSocketBuffer << itm.Identifier.wsGameType << itm.Identifier.wsGameName << itm.IsExistRestore << itm.dwSeriesSerialNumber;
			}
		});

	return TRUE;
}

BOOL CEchoPacket::GetCodeTypeList(_In_ libTools::CSocketRemoteClient*, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	pSocketBuffer->InitializeHead(em_Sock_Msg_Get_CodeTypeList);
	CResText::GetInstance().ActionToJsonFileContent([&pSocketBuffer](CONST JsonFileContent & Content)
		{
			*pSocketBuffer << Content.VecCodeType.size();
			for (CONST auto& itm : Content.VecCodeType)
				* pSocketBuffer << itm;
		});

	return TRUE;
}

BOOL CEchoPacket::DownloadIdentifier(_In_ libTools::CSocketRemoteClient * pRemoteClient, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	pSocketBuffer->InitializeHead(em_Sock_Msg_Download_Identifier);
	LOG_C_D(L"正在下载游戏相关数据给客户端: IP:[%s]", pRemoteClient->GetRemoteClientIp().c_str());


	CResText::GetInstance().ActionToJsonFileContent([&pSocketBuffer](CONST JsonFileContent & Content)
		{
			std::vector<CatchFishStructContent> Vec;
			for (auto& itm : Content.VecContent)
			{
				/*if (itm.ActiveDate.SysTime.wYear == 0)
					continue;*/

				Vec.push_back(itm);
				/*libTools::CTimeCharacter::TimeContent Now;
				::GetLocalTime(&Now.SysTime);
				if (libTools::CTimeCharacter::DateDiff(itm.ActiveDate, Now, libTools::CTimeTick::em_TimeTick::em_TimeTick_Hour) <= 24)
				{
					Vec.push_back(itm);
				}*/
			}


			*pSocketBuffer << Vec.size();
			for (CONST auto& itm : Vec)
				pSocketBuffer->AddStruct(itm);
		});


	return TRUE;
}

BOOL CEchoPacket::ExcuteClientSQL(_In_ libTools::CSocketRemoteClient*, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	std::wstring wsSQL;
	*pSocketBuffer >> wsSQL;


	std::vector<CStatisticsRecorder::Table> VecTable;
	if (!CStatisticsRecorder::GetInstance().ExcuteClientSQL(wsSQL, VecTable))
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << L"执行SQL错误!";
		return TRUE;
	}


	*pSocketBuffer << VecTable.size();
	for (auto& itm : VecTable)
	{
		*pSocketBuffer << itm.size();
		for (auto& Dat : itm)
		{
			*pSocketBuffer << Dat;
		}
	}
	return TRUE;
}

BOOL CEchoPacket::ReleaseNewFishGame(_In_ libTools::CSocketRemoteClient*, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	CatchFishStructContent FishContent = pSocketBuffer->GetStruct<CatchFishStructContent>();
	if (CResText::GetInstance().ActionToCatchFishStructContent_If_Exist(FishContent.Identifier, [](CONST CatchFishStructContent&) {}))
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"游戏[%s-%s] 已经存在!", FishContent.Identifier.wsGameType.c_str(), FishContent.Identifier.wsGameName.c_str());
		return TRUE;
	}


	UINT uFileCount = 0;
	*pSocketBuffer >> uFileCount;
	for (UINT i = 0; i < uFileCount; ++i)
	{
		UINT uFileSize = 0;
		DWORD dwFileCRC32 = 0;
		std::wstring wsFileName;


		*pSocketBuffer >> wsFileName >> uFileSize >> dwFileCRC32;
		std::shared_ptr<CHAR> FileBuffer = pSocketBuffer->GetSizeBuffer(uFileSize);
		if (libTools::CCRC32::GetCRC32(FileBuffer.get(), uFileSize) != dwFileCRC32)
		{
			pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
			*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"传输文件[%s]的CRC[%X] 和传输后的CRC[%X] 不对!", wsFileName.c_str(), dwFileCRC32, libTools::CCRC32::GetCRC32(FileBuffer.get(), uFileSize));
			return TRUE;
		}

		std::wstring wsPath = libTools::CCharacter::MakeCurrentPath(L"\\CatchFishFile\\" + FishContent.Identifier.wsGameType + L"\\");
		if (!libTools::CFile::DirectoryExist(wsPath))
		{
			if (!libTools::CFile::CreateMyDirectory(wsPath.c_str(), FALSE))
			{
				pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
				*pSocketBuffer << L"创建系列目录:[" + wsPath + L"]失败!";
				return TRUE;
			}
		}


		wsPath += FishContent.Identifier.wsGameName + L"\\";
		if (!libTools::CFile::DirectoryExist(wsPath))
		{
			if (!libTools::CFile::CreateMyDirectory(wsPath.c_str(), FALSE))
			{
				pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
				*pSocketBuffer << L"创建名字目录:[" + wsPath + L"]失败!";
				return TRUE;
			}
		}


		// Write File To Local Directory
		if (!libTools::CFile::WriteFile(wsPath + wsFileName, reinterpret_cast<CONST BYTE*>(FileBuffer.get()), uFileSize))
		{
			pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
			*pSocketBuffer << L"创建作弊文件:[" + wsPath + wsFileName + L"]失败!";
			return TRUE;
		}
	}


	pSocketBuffer->InitializeHead(em_Sock_Msg_CreateNew_CatchFishFileContent);
	CResText::GetInstance().SaveCatchFishStructContentToJson(FishContent);
	return TRUE;
}

BOOL CEchoPacket::ReleaseNewChildSpecifyStandCode(_In_ libTools::CSocketRemoteClient * pRemoteClient, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	BOOL bExistRandCode = FALSE;
	SpecifyCustomerContent CustomerContent = pSocketBuffer->GetStruct<SpecifyCustomerContent>();
	if (!CResText::GetInstance().ActionToCatchFishStructContent_If_Exist(CustomerContent.Identifier, [&bExistRandCode](CONST CatchFishStructContent & itm) { bExistRandCode = itm.IsExistRestore; }))
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"游戏[%s-%s] 不存在!", CustomerContent.Identifier.wsGameType.c_str(), CustomerContent.Identifier.wsGameName.c_str());
		return TRUE;
	}



	LOG_C_D(L"生成新的子打码器, IP:[%s] 游戏[%s-%s]", pRemoteClient->GetRemoteClientIp().c_str(), CustomerContent.Identifier.wsGameType.c_str(), CustomerContent.Identifier.wsGameName.c_str());
	AddTextToVecMail(libTools::CCharacter::MakeFormatText(L"生成新的子打码器, IP:[%s] 游戏[%s-%s] 硬盘序列号[%s]",
		pRemoteClient->GetRemoteClientIp().c_str(),
		CustomerContent.Identifier.wsGameType.c_str(),
		CustomerContent.Identifier.wsGameName.c_str(),
		CustomerContent.EncryptDiskSerialNumber.c_str()));

	CResText::GetInstance().ActionToJsonFileContent([&pSocketBuffer, &CustomerContent, bExistRandCode](JsonFileContent & Content)
		{
			auto itr = std::find_if(Content.VecSpecifyCustomerContent.begin(), Content.VecSpecifyCustomerContent.end(), [CustomerContent](CONST SpecifyCustomerContent & itm)
				{
					return itm.EncryptDiskSerialNumber == CustomerContent.EncryptDiskSerialNumber;
				});


			if (itr != Content.VecSpecifyCustomerContent.end())
			{
				pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
				*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"游戏加密硬盘ID[%s-%s] => [%s] 已经存在!", CustomerContent.Identifier.wsGameType.c_str(), CustomerContent.Identifier.wsGameName.c_str(), CustomerContent.EncryptDiskSerialNumber.c_str());
				return;
			}


			CustomerContent.IdentityText = libTools::CCharacter::MakeFormatText(L"0x%08X", libTools::CCRC32::GetCRC32(reinterpret_cast<CONST CHAR*>(CustomerContent.EncryptDiskSerialNumber.c_str()), CustomerContent.EncryptDiskSerialNumber.length() * 2));
			pSocketBuffer->InitializeHead(em_Sock_Msg_CreateNew_ChildSpecifyStandCode);
			*pSocketBuffer << CustomerContent.IdentityText << bExistRandCode;
		});


	CResText::GetInstance().SaveSpecifyCustomerContentToJson(CustomerContent);
	CResText::GetInstance().RefreshSpecifyCustomerJson();
	return TRUE;
}

BOOL CEchoPacket::GetContactContent(_In_ libTools::CSocketRemoteClient*, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	std::wstring wsName;
	*pSocketBuffer >> wsName;

	ContactContent_Tranfer TranferContent;
	if (!CResText::GetInstance().GetContactContentByName(wsName, TranferContent.Content))
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << L"无效的联系人!";
		return TRUE;
	}


	pSocketBuffer->InitializeHead(em_Sock_Msg_Contact_Detail);
	pSocketBuffer->AddStruct(TranferContent);
	return TRUE;
}

BOOL CEchoPacket::UploadGameFileHead(_In_ libTools::CSocketRemoteClient * pSocketClient, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	auto pRemoteClient = dynamic_cast<CRemoteClient*>(pSocketClient);
	pRemoteClient->MakeEmptyUploadFileContent();
	auto pUploadFileContent = pRemoteClient->GetUploadFileContent();

	*pSocketBuffer >> pUploadFileContent->wsGamePath >> pUploadFileContent->wsGameType >> pUploadFileContent->uMaxFileSize >> pUploadFileContent->dwFileCRC32;
	pUploadFileContent->uCurrentSize = 0;
	pUploadFileContent->FileContentPtr = std::shared_ptr<CHAR>(new CHAR[pUploadFileContent->uMaxFileSize], [](CHAR * p) { delete[] p; });

	pSocketBuffer->InitializeHead(em_Sock_Msg_Upload_GameFileHead);
	return TRUE;
}

BOOL CEchoPacket::UploadGameFileContent(_In_ libTools::CSocketRemoteClient * pSocketClient, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	SYSTEMTIME SysTime;
	UINT uFileSize = 0;

	auto pRemoteClient = dynamic_cast<CRemoteClient*>(pSocketClient);
	pRemoteClient->SetKeepALive();

	auto pUploadFileContent = pRemoteClient->GetUploadFileContent();

	*pSocketBuffer >> uFileSize;
	auto ptmpFileContentPtr = pSocketBuffer->GetSizeBuffer(uFileSize);
	if (pUploadFileContent == nullptr)
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << L"无效的文件头!";
		return FALSE;
	}
	else if (uFileSize > pUploadFileContent->uMaxFileSize - pUploadFileContent->uCurrentSize)
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << L"传输文件内容长度错误!!";
		return FALSE;
	}

	memcpy(pUploadFileContent->FileContentPtr.get() + pUploadFileContent->uCurrentSize, ptmpFileContentPtr.get(), uFileSize);
	pUploadFileContent->uCurrentSize += uFileSize;
	if (pUploadFileContent->uCurrentSize == pUploadFileContent->uMaxFileSize)
	{
		if (libTools::CCRC32::GetCRC32(pUploadFileContent->FileContentPtr.get(), pUploadFileContent->uMaxFileSize) != pUploadFileContent->dwFileCRC32)
			LOG_C_E(L"UploadGameFile CRC Error!");
		else
		{
			std::wstring wsFileName = std::experimental::filesystem::v1::path(pUploadFileContent->wsGamePath).filename().wstring();
			std::wstring wsSaveFilePath = libTools::CCharacter::MakeCurrentPath(L"");
			if (wsSaveFilePath.back() != L'\\')
				wsSaveFilePath += L"\\";


			::GetLocalTime(&SysTime);
			wsSaveFilePath += L"WaitToFixList\\";
			wsSaveFilePath += pUploadFileContent->wsGameType;
			wsSaveFilePath += L"\\";
			if (!std::experimental::filesystem::v1::exists(std::experimental::filesystem::v1::path(wsSaveFilePath)))
			{
				std::experimental::filesystem::v1::create_directory(wsSaveFilePath);
			}


			wsSaveFilePath += libTools::CCharacter::MakeFormatText(L"%04d年%02d月%02d日 %02d%02d%02d\\", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
			std::experimental::filesystem::v1::create_directory(wsSaveFilePath);
			auto wsFilePath = wsSaveFilePath + wsFileName;


			if (!libTools::CFile::WriteFile(wsFilePath, reinterpret_cast<CONST BYTE*>(pUploadFileContent->FileContentPtr.get()), pUploadFileContent->uMaxFileSize))
				LOG_C_E(L"写入游戏文件[%s]失败", wsFilePath.c_str());
			else
			{
				CDownloader::GetInstance().AddUploadFileCRC(pUploadFileContent->dwFileCRC32);
				std::wstring wsConfigPath = wsSaveFilePath + L"Directory.txt";
				if (!libTools::CFile::WriteUnicodeFile(wsConfigPath, pUploadFileContent->wsGamePath))
					LOG_C_E(L"写入配置文件[%s]失败", wsConfigPath.c_str());
			}

		}
	}
	pSocketBuffer->InitializeHead(em_Sock_Msg_Upload_GameFileContent);
	return TRUE;
}

BOOL CEchoPacket::IsExistGameFile(_In_ libTools::CSocketRemoteClient*, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	DWORD dwFileCRC32 = 0;
	*pSocketBuffer >> dwFileCRC32;


	pSocketBuffer->InitializeHead(em_Sock_Msg_IsExist_GameFile);
	*pSocketBuffer << CDownloader::GetInstance().IsExistUploadFileCRC(dwFileCRC32);
	return TRUE;
}

BOOL CEchoPacket::GetVecIdentifier(_In_ libTools::CSocketRemoteClient * pRemoteClient, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	pSocketBuffer->InitializeHead(em_Sock_Msg_Get_VecIdentifier);
	CResText::GetInstance().ActionToJsonFileContent([&pSocketBuffer](CONST JsonFileContent & Content)
		{
			std::vector<GameIdentity> Vec;
			for (auto& itm : Content.VecContent)
			{
				Vec.push_back(itm.Identifier);
			}

			*pSocketBuffer << Vec.size();
			for (auto& itm : Vec)
			{
				*pSocketBuffer << itm.wsGameType << itm.wsGameName;
			}
		});


	LOG_C_D(L"正在打开打码器, 客户端IP:[%s]", pRemoteClient->GetRemoteClientIp().c_str());
	return TRUE;
}

BOOL CEchoPacket::QueryCountCode(_In_ libTools::CSocketRemoteClient*, _In_ libTools::CSocketBuffer * pSocketBuffer)
{
	std::wstring wsQueryCode;
	*pSocketBuffer >> wsQueryCode;
	LOG_C_D(L"wsQueryCode=%s", wsQueryCode.c_str());


	BOOL bExist = FALSE;
	SpecifyCustomerContent CustomerContent;
	CResText::GetInstance().ActionToJsonFileContent([wsQueryCode, &CustomerContent, &bExist](CONST JsonFileContent & JContent)
		{
			auto itr = std::find_if(JContent.VecSpecifyCustomerContent.begin(), JContent.VecSpecifyCustomerContent.end(), [wsQueryCode](CONST SpecifyCustomerContent & Content)
				{
					return Content.wsQueryCode == wsQueryCode;
				});


			if (itr != JContent.VecSpecifyCustomerContent.end())
			{
				bExist = TRUE;
				CustomerContent = *itr;
			}
		});


	if (!bExist)
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Error);
		*pSocketBuffer << libTools::CCharacter::MakeFormatText(L"该查询码不存在[%s]", wsQueryCode.c_str());
		LOG_C_D(L"无效的查询码");
	}
	else
	{
		pSocketBuffer->InitializeHead(em_Sock_Msg_Query_CountCode);
		*pSocketBuffer << (CustomerContent.Identifier.wsGameType + L"," + CustomerContent.Identifier.wsGameName + L"," + CustomerContent.IdentityText);
		LOG_C_D(L"查询的游戏是[%s-%s],IdentityText=[%s]", CustomerContent.Identifier.wsGameType.c_str(), CustomerContent.Identifier.wsGameName.c_str(), CustomerContent.IdentityText.c_str());
	}
	return TRUE;
}

VOID CEchoPacket::Initialize()
{
	static HANDLE hThread = nullptr;
	if (hThread == nullptr)
	{
		::CreateThread(nullptr, NULL, static_cast<LPTHREAD_START_ROUTINE>(_MailWorker), nullptr, NULL, nullptr);
	}
}

DWORD WINAPI CEchoPacket::_MailWorker(LPVOID)
{
	std::wstring wsText;
	while (true)
	{
		::Sleep(500);
		{
			std::lock_guard<libTools::CThreadLock> tmpLock(_LockVecMail);
			if (_VecMail.empty())
				continue;


			wsText = _VecMail.front();
			_VecMail.pop();
		}


		std::shared_ptr<WCHAR> wsTextPtr(new WCHAR[wsText.length() + 1], [](WCHAR * p) { delete[] p; });
		libTools::CCharacter::strcpy_my(wsTextPtr.get(), wsText.c_str());


		COPYDATASTRUCT cd = { 0 };
		cd.dwData = 0x4C6F68;
		cd.lpData = wsTextPtr.get();
		cd.cbData = static_cast<DWORD>((wsText.length() + 1) * 2);


		MailFormContent MailForm;
		MailForm.dwPid = libTools::CProcess::FindPidByProcName(L"FishMail.exe");
		if (MailForm.dwPid == 0xFFFFFFFF)
		{
			LOG_MSG_CF(L"不存在FishMail.exe进程");
			continue;
		}


		MailForm.hWnd = nullptr;
		EnumWindows(EnumSetWinName, reinterpret_cast<LPARAM>(&MailForm));
		if (MailForm.hWnd == nullptr)
		{
			LOG_MSG_CF(L"MailForm 不存在!");
			continue;
		}
		::SendMessageW(MailForm.hWnd, WM_COPYDATA, reinterpret_cast<WPARAM>(&cd), reinterpret_cast<LPARAM>(&cd));
	}
	return 0;
}

VOID CEchoPacket::AddTextToVecMail(_In_ CONST std::wstring & wsText)
{
	std::lock_guard<libTools::CThreadLock> tmpLock(_LockVecMail);
	_VecMail.push(wsText);
}
