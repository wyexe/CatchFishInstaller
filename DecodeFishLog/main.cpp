#include <Windows.h>
#include <iostream>
#include <AlgroithmLib/Encrypt/RC4.h>
#include <CharacterLib/Character.h>
#include <FileLib/File.h>

#pragma comment(lib,"AlgroithmLib.lib")
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"FileLib.lib")
#pragma comment(lib,"user32.lib")

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::wcout << L"Invalid FilePath" << std::endl;
		return 0;
	}


	libTools::CCharacter::SetConsoleLanguage();
	std::wstring wsPath = libTools::CCharacter::ASCIIToUnicode(argv[1]);
	std::wcout << L"FilePath:" << wsPath.c_str() << std::endl;
	if (!libTools::CFile::FileExist(wsPath))
	{
		std::wcout << L"UnExist File:" << wsPath.c_str() << std::endl;
		return 0;
	}



	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;
	if (!libTools::CFile::ReadFileContent(wsPath, lpFileContent, uFileSize))
	{
		std::wcout << L"ReadFile Faild Path:" << wsPath.c_str() << std::endl;
		return 0;
	}


	std::string strRc4Key = "83e4a96aed96436c621b9809e258b309";
	libTools::CRC4 Rc4Encrypt(strRc4Key.c_str(), strRc4Key.length());
	auto DeCodeFileTextPtr = libTools::CRC4_Decryption::DecryptText((CONST CHAR*)lpFileContent, uFileSize, Rc4Encrypt.GetKeyStream(uFileSize).get());

	CONST WCHAR* pszText = reinterpret_cast<CONST WCHAR*>(DeCodeFileTextPtr.get());
	std::wstring wsFileText(pszText, uFileSize / 2);

	std::wstring DecodeFilePath = libTools::CCharacter::MakeCurrentPath(L"\\DecodeText.txt");
	libTools::CFile::WriteUnicodeFile(DecodeFilePath, wsFileText);
	
	std::string strText = "\"";
	strText += libTools::CCharacter::UnicodeToASCII(DecodeFilePath);
	strText += "\"";
	system(strText.c_str());
	return 0;
}