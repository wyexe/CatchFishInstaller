#include "ResText.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <include/json.hpp>
#include <CharacterLib/Character.h>
#include <LogLib/Log.h>

#define _SELF L"ResText.cpp"
CResText& CResText::GetInstance()
{
	static CResText Instance;
	return Instance;
}

BOOL CResText::ActionToCatchFishStructContent_If_Exist(_In_ CONST GameIdentity& Ideneifier, _In_ std::function<VOID(CatchFishStructContent&)> Ptr)
{
	BOOL bExist = FALSE;
	ActionToJsonFileContent([&](JsonFileContent & JContent)
		{
			auto itr = std::find_if(JContent.VecContent.begin(), JContent.VecContent.end(), [Ideneifier](CONST CatchFishStructContent & itm)
				{
					return itm.Identifier == Ideneifier;
				});


			if (itr != JContent.VecContent.end())
			{
				bExist = TRUE;
				Ptr(*itr);
			}
		});

	return bExist;
}

CONST std::wstring& CResText::ConvertScoreIndexToText(_In_ em_GenerateCode_ScoreType emScoreType, _In_ DWORD Index) CONST
{
	CONST static std::wstring wsError = L"Error";

	auto itr = g_VecGenerateCodeScoreContent.find(emScoreType);
	if (itr == g_VecGenerateCodeScoreContent.end())
		return wsError;


	auto & Vec = itr->second;
	if (Vec.size() <= Index)
		return wsError;


	return Vec.at(Index);
}

INT CResText::ConvertScoreTextToIndex(_In_ em_GenerateCode_ScoreType emScoreType, _In_ CONST std::wstring & wsScoreText) CONST
{
	int Index = -1;
	auto itr = g_VecGenerateCodeScoreContent.find(emScoreType);
	if (itr != g_VecGenerateCodeScoreContent.end())
	{
		auto& Vec = itr->second;
		for (auto& itm : Vec)
		{
			Index += 1;
			if (itm == wsScoreText)
			{
				return Index;
			}
		}
	}
	return -1;
}

VOID CResText::ActionToJsonFileContent(_In_ std::function<VOID(JsonFileContent&)> Ptr)
{
	std::lock_guard<libTools::CThreadLock> Lck(*this);
	Ptr(_JsonFileContent);
}

BOOL CResText::Refresh()
{
	std::lock_guard<libTools::CThreadLock> Lck(*this);
	return RefreshVecCatchFishStructContent() && RefreshSpecifyCustomerJson();
}

VOID CResText::SaveCatchFishStructContentToJson(_In_ CONST CatchFishStructContent & Content)
{
	std::lock_guard<libTools::CThreadLock> Lck(*this);

	using json = nlohmann::json;
	json js;

	CHAR szPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, szPath);


	std::ifstream ifstm(std::string(szPath) + "\\Res.json");
	try
	{
		ifstm >> js;
		ifstm.close();


		json jGameIdentifier;
		jGameIdentifier["Series"] = libTools::CCharacter::UnicodeToUTF8(Content.Identifier.wsGameType);
		jGameIdentifier["Name"] = libTools::CCharacter::UnicodeToUTF8(Content.Identifier.wsGameName);
		jGameIdentifier["SerialNumber"] = libTools::CCharacter::UnicodeToUTF8(libTools::CCharacter::MakeFormatText(L"%04X", Content.dwSeriesSerialNumber));
		jGameIdentifier["GameFullPath"] = libTools::CCharacter::UnicodeToUTF8(Content.wsFullPath);
		jGameIdentifier["GameCRC32"] = libTools::CCharacter::UnicodeToUTF8(libTools::CCharacter::MakeFormatText(L"0x%08X", Content.dwFileCRC32));
		jGameIdentifier["ReleaseDirectory"] = libTools::CCharacter::UnicodeToUTF8(Content.wsCopyDestPath);
		jGameIdentifier["IsExistRandCode"] = Content.IsExistRestore;
		jGameIdentifier["SpecifyInjectorPath"] = libTools::CCharacter::UnicodeToUTF8(Content.wsInjectorPath);
		jGameIdentifier["EnumValue"] = static_cast<DWORD>(Content.emVersion);
		jGameIdentifier["IsRandDiskSerailNumber"] = Content.IsRandDiskSerailNumber ? "1" : "0";

		for (auto& itm : Content.VecSupplementPath)
		{
			jGameIdentifier["IdentifierDirectory"].push_back(libTools::CCharacter::UnicodeToUTF8(itm));
		}


		json jBinaryCode;
		for (auto& itm : Content.VecCode)
		{
			jBinaryCode.push_back(std::make_pair(libTools::CCharacter::UnicodeToUTF8(libTools::CCharacter::MakeFormatText(L"0x%08X", itm.dwAddr)), libTools::CCharacter::UnicodeToUTF8(libTools::CCharacter::MakeFormatText(L"0x%08X", itm.dwValue))));
		}
		jGameIdentifier["BinaryCode"] = jBinaryCode;


		js["GameIdentifier"].push_back(jGameIdentifier);
		std::ofstream ofstm(std::string(szPath) + "\\Res.json");
		ofstm << js << std::endl;
		ofstm.close();


		// Add to Current vec
		_JsonFileContent.VecContent.push_back(Content);
	}
	catch (const std::exception & ex)
	{
		std::string str = ex.what();
		::MessageBoxA(NULL, str.c_str(), "Error", NULL);
		system("pause");
	}
}

VOID CResText::SaveSpecifyCustomerContentToJson(_In_ CONST SpecifyCustomerContent & Content)
{
	std::lock_guard<libTools::CThreadLock> Lck(*this);

	using json = nlohmann::json;
	json js;

	CHAR szPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, szPath);


	std::ifstream ifstm(std::string(szPath) + "\\ResSpecifyCutsomer.json");
	try
	{
		ifstm >> js;
		ifstm.close();


		json jSpecifyCustomer;
		jSpecifyCustomer["Series"] = libTools::CCharacter::UnicodeToUTF8(Content.Identifier.wsGameType);
		jSpecifyCustomer["Name"] = libTools::CCharacter::UnicodeToUTF8(Content.Identifier.wsGameName);
		jSpecifyCustomer["EncryptDiskSerialNumber"] = libTools::CCharacter::UnicodeToUTF8(Content.EncryptDiskSerialNumber);
		jSpecifyCustomer["Identity"] = libTools::CCharacter::UnicodeToUTF8(Content.IdentityText);


		js["SpecifyCustomer"].push_back(jSpecifyCustomer);
		std::ofstream ofstm(std::string(szPath) + "\\ResSpecifyCutsomer.json");
		ofstm << js << std::endl;
		ofstm.close();


		// Add to Current Vec
		_JsonFileContent.VecSpecifyCustomerContent.push_back(Content);
	}
	catch (const std::exception & ex)
	{
		std::string str = ex.what();
		::MessageBoxA(NULL, str.c_str(), "Error", NULL);
		system("pause");
	}
}

BOOL CResText::ActiveGame(_In_ CONST GameIdentity & Identifier)
{
	std::lock_guard<libTools::CThreadLock> Lck(*this);
	auto itr = std::find_if(_JsonFileContent.VecContent.begin(), _JsonFileContent.VecContent.end(), [Identifier](CONST CatchFishStructContent & itm)
		{
			return itm.Identifier == Identifier;
		});
	if (itr == _JsonFileContent.VecContent.end())
	{
		return FALSE;
	}


	::GetLocalTime(&itr->ActiveDate.SysTime);
	return TRUE;
}

BOOL CResText::GetContactContentByName(_In_ CONST std::wstring & wsName, _Out_ ContactContent & Contact) CONST
{
	CONST static std::vector<ContactContent> Vec =
	{
		{ L"代理1号",L"13845678900",L"12345",L"WeChatA" },
		{ L"代理2号",L"13845678901",L"12346",L"WeChatB" },
		{ L"代理3号",L"13845678902",L"123467",L"WeChatC" }
	};


	auto itr = std::find_if(Vec.begin(), Vec.end(), [wsName](CONST ContactContent & itm) { return itm.wsName == wsName; });
	if (itr != Vec.end())
	{
		Contact = *itr;
		return TRUE;
	}
	return FALSE;
}

BOOL CResText::GenerateCode(_In_ CONST GameIdentity & Identifier, _In_ _Out_ type_OutInfo & Content)
{
	static libTools::CThreadLock GenerateCodeLock;
	std::lock_guard<libTools::CThreadLock> tmpLock(GenerateCodeLock);


	static type_vsf ExprtPtr = nullptr;
	if (ExprtPtr == nullptr)
	{
		HMODULE hmDLL = ::LoadLibraryW(L"_安装接口.dll");
		if (hmDLL == NULL)
		{
			LOG_CF_E(L"加载GenerateCode.dll失败!");
			return FALSE;
		}

		ExprtPtr = reinterpret_cast<type_vsf>(::GetProcAddress(hmDLL, "sf"));
		if (ExprtPtr == nullptr)
		{
			LOG_CF_E(L"获取导出函数失败!");
			return FALSE;
		}
	}


	enum版本 emVersion = em_Null;
	if (!CResText::GetInstance().ActionToCatchFishStructContent_If_Exist(Identifier, [&emVersion](CONST CatchFishStructContent & itm) { emVersion = itm.emVersion; }))
	{
		LOG_CF_E(L"生成测试码不支持游戏[%s-%s]!", Identifier.wsGameType.c_str(), Identifier.wsGameName.c_str());
		return FALSE;
	}



	type_v生成码 GenrateCodePtr = ExprtPtr(emVersion);
	if (GenrateCodePtr == nullptr)
	{
		LOG_CF_E(L"生成测试码不支持游戏[%s-%s]!", Identifier.wsGameType.c_str(), Identifier.wsGameName.c_str());
		return FALSE;
	}


	return GenrateCodePtr(Content);
}

BOOL CResText::RefreshVecCatchFishStructContent()
{
	using json = nlohmann::json;
	json js;

	CHAR szPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, szPath);


	std::ifstream ifstm(std::string(szPath) + "\\Res.json");


	try
	{
		ifstm >> js;
		ifstm.close();
		_JsonFileContent.VecContent.clear();
		_JsonFileContent.VecCodeType.clear();



		// 
		auto jGameIdentifier = js["GameIdentifier"];
		for (CONST auto& itm : jGameIdentifier)
		{
			CatchFishStructContent Content;
			Content.Identifier.wsGameType = libTools::CCharacter::UTF8ToUnicode(itm["Series"].get<std::string>());
			Content.Identifier.wsGameName = libTools::CCharacter::UTF8ToUnicode(itm["Name"].get<std::string>());
			Content.wsFullPath = libTools::CCharacter::UTF8ToUnicode(itm["GameFullPath"].get<std::string>());
			Content.dwFileCRC32 = itm.value("GameCRC32", "0") != "0" ? static_cast<DWORD>(std::stoll(itm["GameCRC32"].get<std::string>(), 0, 16)) : 0;
			Content.uFileSize = std::stoul(itm.value("FileSize", "0"));
			Content.dwSeriesSerialNumber = static_cast<DWORD>(std::stoll(itm["SerialNumber"].get<std::string>(), 0, 16));
			Content.wsCopyDestPath = libTools::CCharacter::UTF8ToUnicode(itm["ReleaseDirectory"].get<std::string>());
			Content.IsExistRestore = itm["IsExistRandCode"].get<BOOL>();
			Content.wsInjectorPath = libTools::CCharacter::UTF8ToUnicode(itm["SpecifyInjectorPath"].get<std::string>());
			Content.emVersion = static_cast<enum版本>(itm["EnumValue"].get<int>());
			Content.IsRandDiskSerailNumber = itm.value("IsRandDiskSerailNumber", "0") == "0" ? false : true;
			Content.IsDisableDeleteGameDirectoryInjectorFile = std::stoi(itm.value("DisableDeleteGameDirectoryInjectorFile", "0"));
			Content.IsDifferenceDisk = itm.value("IsDifficultDisk", "0") == "0" ? false : true;
			Content.IsNewFramework = itm.value("IsNewFramework", "0") == "0" ? false : true;


			auto& IdentifierDirectory = itm["IdentifierDirectory"];
			for (auto& IdentifierDirectoryItem : IdentifierDirectory)
				Content.VecSupplementPath.push_back(libTools::CCharacter::UTF8ToUnicode(IdentifierDirectoryItem.get<std::string>()));


			auto & IdentifierBinaryCode = itm["BinaryCode"];
			for (auto& BinaryCodeItem : IdentifierBinaryCode)
			{
				if (BinaryCodeItem.size() % 2 != 0)
				{
					LOG_MSG_CF(L"游戏[%s-%s]的BinaryCode格式错误!", Content.Identifier.wsGameType.c_str(), Content.Identifier.wsGameName.c_str());
					return FALSE;
				}


				CatchFishBinaryCode BCode;
				BCode.dwAddr = static_cast<DWORD>(std::stoll(BinaryCodeItem.at(0).get<std::string>(), 0, 16));
				BCode.dwValue = static_cast<DWORD>(std::stoll(BinaryCodeItem.at(1).get<std::string>(), 0, 16));
				Content.VecCode.push_back(BCode);
			}


			_JsonFileContent.VecContent.push_back(Content);
		}


		//
		auto jCodeType = js["CodeType"];
		for (CONST auto& itm : jCodeType)
		{
			_JsonFileContent.VecCodeType.push_back(libTools::CCharacter::UTF8ToUnicode(itm.get<std::string>()));
		}
	}
	catch (const std::exception & ex)
	{
		std::string str = ex.what();
		::MessageBoxA(NULL, str.c_str(), "Error", NULL);
		system("pause");
	}



	if (!CheckJsonContent())
	{
		_JsonFileContent.VecContent.clear();
		return FALSE;
	}


	return TRUE;
}

BOOL CResText::RefreshSpecifyCustomerJson()
{
	using json = nlohmann::json;
	json js;

	CHAR szPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, szPath);


	std::ifstream ifstm(std::string(szPath) + "\\ResSpecifyCutsomer.json");



	try
	{
		ifstm >> js;
		ifstm.close();


		_JsonFileContent.VecSpecifyCustomerContent.clear();
		auto jSpecifyCustomer = js["SpecifyCustomer"];
		for (CONST auto& itm : jSpecifyCustomer)
		{
			type_OutInfo GenerateCodeParam = { 0 };
			SpecifyCustomerContent Content;
			Content.EncryptDiskSerialNumber = libTools::CCharacter::UTF8ToUnicode(itm["EncryptDiskSerialNumber"].get<std::string>());
			Content.IdentityText = libTools::CCharacter::UTF8ToUnicode(itm["Identity"].get<std::string>());
			Content.Identifier.wsGameType = libTools::CCharacter::UTF8ToUnicode(itm["Series"].get<std::string>());
			Content.Identifier.wsGameName = libTools::CCharacter::UTF8ToUnicode(itm["Name"].get<std::string>());


			GenerateCodeParam.n还原系统随机码 = -1;
			GenerateCodeParam.n_shu = 1;
			GenerateCodeParam.n_yin = 1;
			GenerateCodeParam.n次数 = 1;
			libTools::CCharacter::strcpy_my(GenerateCodeParam.n_as加密id, libTools::CCharacter::UnicodeToASCII(Content.EncryptDiskSerialNumber).c_str());
			if (!GenerateCode(Content.Identifier, GenerateCodeParam))
			{
				LOG_MSG_CF(L"生成[%s-%s, %s]的查询码失败!", Content.Identifier.wsGameType.c_str(), Content.Identifier.wsGameName.c_str(), Content.EncryptDiskSerialNumber.c_str());
				return FALSE;
			}


			Content.wsQueryCode = libTools::CCharacter::ASCIIToUnicode(GenerateCodeParam.as取次码);
			//LOG_C_D(L"游戏[%s,%s]的次数码=[%s]", Content.Identifier.wsGameType.c_str(), Content.Identifier.wsGameName.c_str(), Content.wsQueryCode.c_str());
			_JsonFileContent.VecSpecifyCustomerContent.push_back(Content);
		}
	}
	catch (const std::exception & ex)
	{
		std::string str = ex.what();
		::MessageBoxA(NULL, str.c_str(), "Error", NULL);
		system("pause");
	}
	return TRUE;
}

BOOL CResText::CheckJsonContent() CONST
{
	return TRUE;
}
