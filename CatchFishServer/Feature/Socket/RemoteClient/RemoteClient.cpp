#include "RemoteClient.h"

CRemoteClient::CRemoteClient(_In_ UINT_PTR Sock) : libTools::CSocketRemoteClient(Sock)
{

}

VOID CRemoteClient::MakeEmptyUploadFileContent()
{
	if (_UploadFileContent == nullptr)
	{
		_UploadFileContent = std::make_shared<UploadFileContent>();
	}
}

UploadFileContent* CRemoteClient::GetUploadFileContent()
{
	return _UploadFileContent == nullptr ? nullptr : _UploadFileContent.get();
}
