/*
 * Adapted from https://blog.csdn.net/u011857683/article/details/53046295
 */
#ifndef CURL_MANAGER_H
#define CURL_MANAGER_H

#include <cstdio>
#include <cstdlib>
#include <curl/curl.h>
#include <string>
#include "mutex_lock.h"

enum HttpMethod
{
	METHOD_GET,
	METHOD_POST,
	METHOD_PUT,
	METHOD_DELETE,
};

enum DataFormat
{
	FORMAT_DEFAULT,
	FORMAT_XML,
	FORMAT_JSON,
};

const unsigned int CONNECT_DEFAULT_TIMEOUT = 10;
const unsigned int DEFAULT_TIMEOUT = 600;

class CurlManager
{
private:
	CURL* m_pCurl;
	struct curl_slist* m_pHeader;
	std::string m_sIP;
	unsigned int m_nPort;
	std::string m_sUser;
	std::string m_sPwd;
	std::string m_sUrlPath;
	unsigned int m_connect_timeout;
	unsigned int m_timeout;

	static bool s_bGlobalInitStatus;
	static MutexLock s_MutexLock;

public:
	CurlManager();
	CurlManager(const std::string& sIP, const unsigned int nPort,
		const std::string& sUser, const std::string& sPwd);
	~CurlManager();

	void setConnectTimeout(unsigned int nTime);
	void setTimeout(unsigned int nTime);

	void setServer(const std::string& sIP, const unsigned int nPort,
		const std::string& sUser, const std::string& sPwd);
	void setUserNameAndPassword(const std::string& user, const std::string& pwd);
	void setSSLVerify(int flag = 0);

	void setUrlPath(const std::string& sUrlPath);

	int sendMsg(const std::string& sMsg, const int nMethod, const int nFormat, std::string& sRec);

	int downloadFile(const std::string& sFilename, const int nFormat);
	int uploadFileContent(const std::string& sFile, const int nFormat, std::string& sRec);
	int uploadFile(const std::string& sFilefullname, std::string& sRec);

	int initCurlResource();
	int releaseCurlResource();

	static int globalInit();
	static int globalCleanup();

private:
	int setUserPwd();
	int setDataFormat(const int nFormat);
	static size_t httpReadFile(void* buffer, size_t size, size_t nmemb, void* file);
	static size_t httpWriteFile(void* buffer, size_t size, size_t nmemb, void* file);
	static size_t httpDataWriter(void* buffer, size_t size, size_t nmemb, void* content);

	int getMsg(const std::string& sMsg, const int nFormat, std::string& sRec);
	int postMsg(const std::string& sMsg, const int nFormat, std::string& sRec);
	int putMsg(const std::string& sMsg, const int nFormat, std::string& sRec);
	int deleteMsg(const std::string& sMsg, const int nFormat, std::string& sRec);

	int messagePublicMethod(const int nFormat);
	int dealResCode(const CURLcode res);
	const char* getFileName(const char* path);
};

#endif
