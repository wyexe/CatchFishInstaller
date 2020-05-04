#ifndef __CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_SOCKET_REMOTECLIENT_REMOTECLIENT_H__
#define __CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_SOCKET_REMOTECLIENT_REMOTECLIENT_H__

#include <SocketCommon/SocketRemoteClient.h>
#include <Feature/Common/Common.h>

class CRemoteClient : public libTools::CSocketRemoteClient
{
public:
	CRemoteClient(_In_ UINT_PTR Sock);
	virtual ~CRemoteClient() = default;

	
	VOID MakeEmptyUploadFileContent();

	UploadFileContent* GetUploadFileContent();
private:
	std::shared_ptr<UploadFileContent> _UploadFileContent = nullptr;
};


#endif // !__CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_SOCKET_REMOTECLIENT_REMOTECLIENT_H__
