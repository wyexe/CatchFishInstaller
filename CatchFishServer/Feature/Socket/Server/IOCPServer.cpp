#include "IOCPServer.h"
#include <LogLib/Log.h>
#include <Feature/Socket/Echo/EchoPacket.h>
#include <Feature/Socket/RemoteClient/RemoteClient.h>

#ifdef _DEBUG
#pragma comment(lib,"SocketServerLib_Debug.lib")
#else
#pragma comment(lib,"SocketServerLib.lib")
#endif // _DEBUG


#define _SELF L"IOCPServer.cpp"
CIOCPServer::CIOCPServer()
{
}

CIOCPServer::~CIOCPServer()
{
}

libTools::CSocketRemoteClient* CIOCPServer::CreateNewSocketClient(_In_ UINT_PTR ClientSock)
{
	return new CRemoteClient(ClientSock);
}

VOID CIOCPServer::ReleaseSocketClient(_In_ libTools::CSocketRemoteClient* pSocketClient)
{
	pSocketClient->DisConnect();
	pSocketClient->Remove();
	delete dynamic_cast<CRemoteClient *>(pSocketClient);
	pSocketClient = nullptr;
}

BOOL CIOCPServer::EchoRecive(_In_ libTools::CSocketRemoteClient* pSocketClient, _In_ libTools::CSocketBuffer* pSocketBuffer)
{
	em_Sock_Msg emSockMsg;
	if (pSocketBuffer->VerDataPtr(emSockMsg) != libTools::CSocketBuffer::em_SocketBuffer_Error_None)
	{
		LOG_C_E(L"VerDataPtr = FALSE!");
		return FALSE;
	}



	switch (emSockMsg)
	{
	case em_Sock_Msg_KeepLive:
		CEchoPacket::KeepALive(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Download_File:
		CEchoPacket::DownloadFile(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Record_Install:
		CEchoPacket::InstallRecord(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Version_File:
		CEchoPacket::SetInstallerVersion(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Generate_TestCode:
		CEchoPacket::GenerateTestCode(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Generate_StandardCode:
		CEchoPacket::GenerateStandardCode(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Get_FishList:
		CEchoPacket::GetFishList(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Get_CodeTypeList:
		CEchoPacket::GetCodeTypeList(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Download_Identifier:
		CEchoPacket::DownloadIdentifier(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_ExcuteSQL:
		//CEchoPacket::ExcuteClientSQL(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Generate_Specify_StandCode:
		CEchoPacket::Generate_Specify_StandCode(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_CreateNew_CatchFishFileContent:
		CEchoPacket::ReleaseNewFishGame(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_CreateNew_ChildSpecifyStandCode:
		CEchoPacket::ReleaseNewChildSpecifyStandCode(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Contact_Detail:
		CEchoPacket::GetContactContent(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Upload_GameFileHead:
		CEchoPacket::UploadGameFileHead(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Upload_GameFileContent:
		CEchoPacket::UploadGameFileContent(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_IsExist_GameFile:
		CEchoPacket::IsExistGameFile(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Get_VecIdentifier:
		CEchoPacket::GetVecIdentifier(pSocketClient, pSocketBuffer);
		break;
	case em_Sock_Msg_Query_CountCode:
		CEchoPacket::QueryCountCode(pSocketClient, pSocketBuffer);
		break;
	default:
		LOG_C_E(L"Invalid SockMsg=[%d]", emSockMsg);
		break;
	}


	return PostSend(pSocketClient, pSocketBuffer);
}

BOOL CIOCPServer::Run(_In_ SHORT shPort, _In_ UINT uMaxAccept)
{
	return RunServer(shPort, uMaxAccept);
}

VOID CIOCPServer::Stop()
{
	StopServer();
}
