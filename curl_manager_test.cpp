#include "curl_manager_test.h"


int curl_get_message()
{
	int nCode = -1;
	std::string sIP = "10.200.0.225";
	unsigned int nPort = 80;
	std::string sUser = "USER";
	std::string sPwd = "PWD";
	CurlManager* pCurl = new CurlManager(sIP, nPort, sUser, sPwd);
	if (pCurl == NULL)
	{
		EPRINT("new object failure!!");
		return -1;
	}

	nCode = pCurl->initCurlResource();
	if (nCode != 0)
	{
		EPRINT("curl init failure!!");
		delete pCurl;
		pCurl = NULL;
		return -1;
	}

	std::string sUrlPath = "/index.html?user=user?name=name";
	pCurl->setUrlPath(sUrlPath);

	int nMethod = METHOD_GET;
	int nFormat = FORMAT_DEFAULT;
	std::string sMsg;
	std::string sRec;
	nCode = pCurl->sendMsg(sMsg, nMethod, nFormat, sRec);
	printf("%d\n", nCode);
	printf("%s\n", sRec.c_str());

	delete pCurl;

	return nCode;
}

int curl_post_message()
{
	return 0;
}

int curl_upload_file()
{
	return 0;
}

int curl_download_file()
{
	return 0;
}
