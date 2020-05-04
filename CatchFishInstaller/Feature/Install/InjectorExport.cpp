#include "Installer.h"

#pragma warning(push)
#pragma warning(disable:4002)
#pragma warning(disable:4101)
#pragma warning(disable:4189)

namespace dhj8938rwjf {

#include <Windows.h>

	using namespace std;

	BOOL AddImportTable(const char * wzPEFilePath, const char * szInjectDllName, const char *szFunctionName);
	BOOL AddNewImportDescriptor(const char * wzPEFilePath, const char * szInjectDllName, const char *szImportFunctionName);
	BOOL AddNewSection(const char * lpModulePath, DWORD dwNewSectionSize);
	DWORD PEAlign(DWORD dwTarNumber, DWORD dwAlignTo);
	DWORD RVAToOffset(PIMAGE_NT_HEADERS pImageNTHeader, DWORD dwRVA);
	PIMAGE_SECTION_HEADER ImageRVAToSection(PIMAGE_NT_HEADERS pImageNTHeader, DWORD dwRVA);


	int main4()
	{
		char TargetPath[0x20] = "o:\\gdi32 - 副本.dll";;

		char InjectDllName[0x20] = "uspdat.dll";

		AddImportTable(TargetPath, InjectDllName, "ftsWordBreak"); //修改为直接显示函数名字！！ 见 787878

		return 0;
	};


#define printf()  //屏蔽日志


	int __stdcall _添加导入表(const char* str_TargetDll, const char* str_AddDllName, const char* str_AddFunctionName)
	{
		AddImportTable(str_TargetDll, str_AddDllName, str_AddFunctionName); //修改为直接显示函数名字！！ 见 787878
		return 1;
	};


	BOOL __stdcall _已修改过(void* pbuff_, DWORD lens, const char* str_AddDllName)
	{
		BYTE* pbuff = (BYTE*)pbuff_;
		for (DWORD i = 0; i < lens; i++)
		{
			if (pbuff[i] == 0)
			{
				pbuff[i] = 0x20;
			}
		};

		pbuff[lens - 1] = 0;

		if (strstr((char*)pbuff, str_AddDllName) != 0)
		{
			return TRUE;
		}
		return FALSE;
	};


	BOOL AddImportTable(const char * wzPEFilePath, const char * szInjectDllName, const char *szFunctionName)
	{
		BOOL bOk = FALSE;
		try
		{
			//增加一个叫"WINSUN"的节
			bOk = AddNewSection(wzPEFilePath, 256);
			if (!bOk)
			{
				MessageBoxA(NULL, "Add New Section Fail", "Error", MB_OK);
				return bOk;
			}
			//增加一个导入表
			AddNewImportDescriptor(wzPEFilePath, szInjectDllName, szFunctionName);
		}
		catch (...)
		{
			return bOk;
		}
		return bOk;
	}


	//
	//增加导入表项
	//
	BOOL AddNewSection(const char* lpModulePath, DWORD dwNewSectionSize)
	{
		BOOL   bOk = FALSE;
		LPVOID lpMemoryModule = NULL;
		LPBYTE lpData = NULL;
		DWORD  dwNewSectionFileSize, dwNewSectionMemorySize;
		HANDLE FileHandle = INVALID_HANDLE_VALUE, MappingHandle = INVALID_HANDLE_VALUE;
		PIMAGE_NT_HEADERS NtHeader = NULL;
		PIMAGE_SECTION_HEADER NewSection = NULL, LastSection = NULL;

		printf("[!] AddNewSection Enter!\n");

		//TODO:可能还涉及关闭windows文件保护
		__try
		{
			//pe文件映射到内存
			FileHandle = CreateFileA(
				lpModulePath,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);
			if (INVALID_HANDLE_VALUE == FileHandle)
			{
				printf("[-] AddNewSection CreateFile Fail!\n");
				goto _EXIT_;
			}

			DWORD dwFileLength = GetFileSize(FileHandle, NULL);
			//映射PE文件
			MappingHandle = CreateFileMapping(FileHandle, NULL, PAGE_READWRITE/* | SEC_IMAGE*/, 0, dwFileLength, L"WINSUN_MAPPING_FILE");
			if (NULL == MappingHandle)
			{

				printf("[-] AddNewSection CreateFileMapping Fail!\n");
				goto _EXIT_;

			}

			lpMemoryModule = MapViewOfFile(MappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, dwFileLength);
			if (NULL == lpMemoryModule)
			{
				printf("[-] AddNewSection MapViewOfFile Fail!\n");
				goto _EXIT_;
			}

			lpData = (LPBYTE)lpMemoryModule;
			//判断是否是PE文件
			if (((PIMAGE_DOS_HEADER)lpData)->e_magic != IMAGE_DOS_SIGNATURE)
			{
				printf("[-] AddNewSection PE Header MZ error!\n");
				goto _EXIT_;
			}

			NtHeader = (PIMAGE_NT_HEADERS)(lpData + ((PIMAGE_DOS_HEADER)(lpData))->e_lfanew);
			if (NtHeader->Signature != IMAGE_NT_SIGNATURE)
			{
				printf("[-] AddNewSection PE Header PE Error!\n");
				goto _EXIT_;
			}

			//判断是否可以增加一个新节
			if (((NtHeader->FileHeader.NumberOfSections + 1) * sizeof(IMAGE_SECTION_HEADER)) > (NtHeader->OptionalHeader.SizeOfHeaders))
			{
				printf("[-] AddNewSection Cannot Add A New Section!\n");
				goto _EXIT_;
			}

			NewSection = (PIMAGE_SECTION_HEADER)(NtHeader + 1) + NtHeader->FileHeader.NumberOfSections;
			LastSection = NewSection - 1;


			DWORD rSize, vSize, rOffset, vOffset;
			//对齐偏移和RVA
			rSize = PEAlign(dwNewSectionSize,
				NtHeader->OptionalHeader.FileAlignment);

			rOffset = PEAlign(LastSection->PointerToRawData + LastSection->SizeOfRawData,
				NtHeader->OptionalHeader.FileAlignment);

			vSize = PEAlign(dwNewSectionSize,
				NtHeader->OptionalHeader.SectionAlignment);

			vOffset = PEAlign(LastSection->VirtualAddress + LastSection->Misc.VirtualSize,
				NtHeader->OptionalHeader.SectionAlignment);

			//填充新节表
			memcpy(NewSection->Name, "WINSUN", strlen("WINSUN"));
			NewSection->VirtualAddress = vOffset;
			NewSection->PointerToRawData = rOffset;
			NewSection->Misc.VirtualSize = vSize;
			NewSection->SizeOfRawData = rSize;
			NewSection->Characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;

			//修改IMAGE_NT_HEADERS，增加新节表
			NtHeader->FileHeader.NumberOfSections++;
			NtHeader->OptionalHeader.SizeOfImage += vSize;
			NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size = 0;
			NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress = 0;

			//增加新节到文件尾部
			DWORD dwWriteBytes;
			SetFilePointer(FileHandle, 0, 0, FILE_END);
			PBYTE pbNewSectionContent = new BYTE[rSize];
			ZeroMemory(pbNewSectionContent, rSize);
			bOk = WriteFile(FileHandle, pbNewSectionContent, rSize, &dwWriteBytes, NULL);
			if (!bOk)
			{
				MessageBox(NULL, L"新增节失败", L"Error", MB_OK);
				goto _EXIT_;
			}

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printf("[-] AddImportTableItem  Exception!\n");
			return false;
		}
		printf("[!] AddNewSection Exit!\n");
		bOk = true;
	_EXIT_:

		if (FileHandle)
		{
			CloseHandle(FileHandle);
		}

		if (lpMemoryModule)
		{
			UnmapViewOfFile(lpMemoryModule);
		}

		if (MappingHandle)
		{
			CloseHandle(MappingHandle);
		}
		return true;
	}
	//内存对齐
	DWORD PEAlign(DWORD dwTarNumber, DWORD dwAlignTo)
	{
		return(((dwTarNumber + dwAlignTo - 1) / dwAlignTo)*dwAlignTo);
	}

	//增加一个导入表
	BOOL AddNewImportDescriptor(const char * wzPEFilePath, const char * szInjectDllName, const char *szImportFunctionName)
	{
		BOOL bOk = FALSE;
		LPVOID lpMemoryModule = NULL;
		LPBYTE lpData = NULL;
		DWORD  dwNewSecFileSize, dwNewSecMemSize;
		HANDLE FileHandle = INVALID_HANDLE_VALUE, MappingHandle = INVALID_HANDLE_VALUE;
		PIMAGE_NT_HEADERS NtHeader = NULL;
		PIMAGE_IMPORT_DESCRIPTOR ImportTable = NULL;
		PIMAGE_SECTION_HEADER    SectionHeader = NULL;
		__try
		{
			//pe文件映射到内存
			FileHandle = CreateFileA(
				wzPEFilePath,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);
			if (INVALID_HANDLE_VALUE == FileHandle)
			{
				printf("[-] AddNewImportDescriptor CreateFile fail!\n");
				goto _EXIT_;
			}

			DWORD dwFileLength = GetFileSize(FileHandle, NULL);
			MappingHandle = CreateFileMapping(FileHandle, NULL, PAGE_READWRITE/* | SEC_IMAGE*/, 0, dwFileLength, L"WINSUN_MAPPING_FILE");
			if (NULL == MappingHandle)
			{

				printf("[-] AddNewImportDescriptor CreateFileMapping fail!\n");
				goto _EXIT_;

			}

			lpMemoryModule = MapViewOfFile(MappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, dwFileLength);
			if (NULL == lpMemoryModule)
			{
				printf("[-] AddNewImportDescriptor MapViewOfFile fail!\n");
				goto _EXIT_;
			}

			lpData = (LPBYTE)lpMemoryModule;
			//判断是否是PE
			if (((PIMAGE_DOS_HEADER)lpData)->e_magic != IMAGE_DOS_SIGNATURE)
			{
				printf("[-] AddNewImportDescriptor PE Header MZ error!\n");
				goto _EXIT_;
			}

			NtHeader = (PIMAGE_NT_HEADERS)(lpData + ((PIMAGE_DOS_HEADER)(lpData))->e_lfanew);
			if (NtHeader->Signature != IMAGE_NT_SIGNATURE)
			{
				printf("[-] AddNewImportDescriptor PE Header PE error!\n");
				goto _EXIT_;
			}
			ImportTable = (PIMAGE_IMPORT_DESCRIPTOR)(lpData + RVAToOffset(NtHeader, NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));
			BOOL bBoundImport = FALSE;
			if (ImportTable->Characteristics == 0 && ImportTable->FirstThunk != 0)
			{
				bBoundImport = TRUE;
				NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size = 0;
				NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress = 0;
			}

			SectionHeader = (PIMAGE_SECTION_HEADER)(NtHeader + 1) + NtHeader->FileHeader.NumberOfSections - 1;
			PBYTE pbNewSection = SectionHeader->PointerToRawData + lpData;
			int i = 0;
			while (ImportTable->FirstThunk != 0)
			{
				memcpy(pbNewSection, ImportTable, sizeof(IMAGE_IMPORT_DESCRIPTOR));
				ImportTable++;
				pbNewSection += sizeof(IMAGE_IMPORT_DESCRIPTOR);
				i++;
			}
			memcpy(pbNewSection, (pbNewSection - sizeof(IMAGE_IMPORT_DESCRIPTOR)), sizeof(IMAGE_IMPORT_DESCRIPTOR));



			DWORD dwDelt = SectionHeader->VirtualAddress - SectionHeader->PointerToRawData;

			//avoid import not need table
			PIMAGE_THUNK_DATA pImgThunkData = (PIMAGE_THUNK_DATA)(pbNewSection + sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2);



			//import dll name
			PBYTE pszDllNamePosition = (PBYTE)(pImgThunkData + 2);
			memcpy(pszDllNamePosition, szInjectDllName, strlen(szInjectDllName));
			pszDllNamePosition[strlen(szInjectDllName)] = 0;

			//确定IMAGE_IMPORT_BY_NAM的位置
			PIMAGE_IMPORT_BY_NAME pImgImportByName = (PIMAGE_IMPORT_BY_NAME)(pszDllNamePosition + strlen(szInjectDllName) + 1);


			//init IMAGE_THUNK_DATA
			pImgThunkData->u1.Ordinal = dwDelt + (DWORD)pImgImportByName - (DWORD)lpData;


			//init IMAGE_IMPORT_BY_NAME
			pImgImportByName->Hint = 1 * 0;  //【】必须为0才会显示名字  787878
			memcpy(pImgImportByName->Name, szImportFunctionName, strlen(szImportFunctionName)); //== dwDelt + (DWORD)pszFuncNamePosition - (DWORD)lpData ;
			pImgImportByName->Name[strlen(szImportFunctionName)] = 0;
			//init OriginalFirstThunk
			if (bBoundImport)
			{
				((PIMAGE_IMPORT_DESCRIPTOR)pbNewSection)->OriginalFirstThunk = 0;
			}
			else
				((PIMAGE_IMPORT_DESCRIPTOR)pbNewSection)->OriginalFirstThunk = dwDelt + (DWORD)pImgThunkData - (DWORD)lpData;
			//init FirstThunk
			((PIMAGE_IMPORT_DESCRIPTOR)pbNewSection)->FirstThunk = dwDelt + (DWORD)pImgThunkData - (DWORD)lpData;
			//init Name
			((PIMAGE_IMPORT_DESCRIPTOR)pbNewSection)->Name = dwDelt + (DWORD)pszDllNamePosition - (DWORD)lpData;

			((PIMAGE_IMPORT_DESCRIPTOR)pbNewSection)->ForwarderChain = 0;//【】必须为0才会显示名字  787878
			((PIMAGE_IMPORT_DESCRIPTOR)pbNewSection)->TimeDateStamp = 0;//【】必须为0才会显示名字  787878


																		//改变导入表
			NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = SectionHeader->VirtualAddress;
			NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = (i + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);



		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			printf("[-] AddNewImportDescriptor  Exception!\n");
			return false;
		}

	_EXIT_:

		if (FileHandle)
		{
			CloseHandle(FileHandle);
		}

		if (lpMemoryModule)
		{
			UnmapViewOfFile(lpMemoryModule);
		}

		if (MappingHandle)
		{
			CloseHandle(MappingHandle);
		}
		return true;
	}

	//
	// calulates the Offset from a RVA
	// Base    - base of the MMF
	// dwRVA - the RVA to calculate
	// returns 0 if an error occurred else the calculated Offset will be returned
	DWORD RVAToOffset(PIMAGE_NT_HEADERS pImageNTHeader, DWORD dwRVA)
	{
		DWORD _offset;
		PIMAGE_SECTION_HEADER section;
		section = ImageRVAToSection(pImageNTHeader, dwRVA);//ImageRvaToSection(pimage_nt_headers,Base,dwRVA);
		if (section == NULL)
		{
			return(0);
		}
		_offset = dwRVA + section->PointerToRawData - section->VirtualAddress;
		return(_offset);
	}

	PIMAGE_SECTION_HEADER ImageRVAToSection(PIMAGE_NT_HEADERS pImageNTHeader, DWORD dwRVA)
	{
		int i;
		PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)(pImageNTHeader + 1);
		for (i = 0; i < pImageNTHeader->FileHeader.NumberOfSections; i++)
		{
			if ((dwRVA >= (pSectionHeader + i)->VirtualAddress) && (dwRVA <= ((pSectionHeader + i)->VirtualAddress + (pSectionHeader + i)->SizeOfRawData)))
			{
				return ((PIMAGE_SECTION_HEADER)(pSectionHeader + i));
			}
		}
		return(NULL);
	}




};

#pragma warning(pop)