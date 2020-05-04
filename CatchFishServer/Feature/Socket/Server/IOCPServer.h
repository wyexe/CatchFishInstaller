#ifndef __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_SOCKET_SERVER_IOCPSERVER_H__
#define __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_SOCKET_SERVER_IOCPSERVER_H__


#include <SocketServerLib/SocketBaseServerService.h>
#include <SocketCommon/SocketRemoteClient.h>
#include <SocketCommon/SocketBuffer.h>
#include <ProcessLib/Lock/Lock.h>


class CIOCPServer final : public libTools::CSocketBaseServerService, public libTools::CThreadLock
{
public:
	CIOCPServer();
	~CIOCPServer();

	virtual BOOL Run(_In_ SHORT shPort, _In_ UINT uMaxAccept) override;

	virtual VOID Stop() override;
private:
	virtual libTools::CSocketRemoteClient* CreateNewSocketClient(_In_ UINT_PTR ClientSock) override;

	virtual VOID ReleaseSocketClient(_In_ libTools::CSocketRemoteClient* pSocketClient) override;

	virtual BOOL EchoRecive(_In_ libTools::CSocketRemoteClient* pSocketClient, _In_ libTools::CSocketBuffer* pSocketBuffer) override;
private:
	HANDLE _hTread = NULL;
};



#endif // !__CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_SOCKET_SERVER_IOCPSERVER_H__
