#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <filesystem>
#include <devioctl.h>
#include <ntdddisk.h>
#include <iostream>
#include <CharacterLib/Character.h>
#include <FileLib/File.h>

#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"FileLib_Debug.lib")



#define _SELF L"main.cpp"
int main(int argc, char * argv[])
{

	WCHAR wszText[1024] = { 0 };
	::GetSystemDirectoryW(wszText, 1024);

	system("pause");
	return 0;
}
