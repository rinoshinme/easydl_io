#include "curl_manager.h"
#include <errno.h>
#include <sys/stat.h>

// static members
bool CurlManager::s_bGlobalInitStatus = false;
MutexLock CurlManager::s_MutexLock;

CurlManager::CurlManager()
	:m_pCurl(NULL), m_pHeader(NULL),
	m_connect_timeout(CONNECT_DEFAULT_TIMEOUT), 
	m_timeout(DEFAULT_TIMEOUT) {}

CurlManager::CurlManager(const std::string& sIP, const unsigned int nPort, const std::string& sUser, const std::string& sPwd)
	: m_pCurl(NULL), m_pHeader(NULL),
	m_sIP(sIP), m_nPort(nPort), m_sUser(sUser), m_sPwd(sPwd),
	m_connect_timeout(CONNECT_DEFAULT_TIMEOUT),
	m_timeout(DEFAULT_TIMEOUT) {}

CurlManager::~CurlManager()
{
	if (this->m_pHeader != NULL)
	{
		curl_slist_free_all(this->m_pHeader);
		this->m_pHeader = NULL;
	}
	if (this->m_pCurl != NULL)
		releaseCurlResource();
}

int CurlManager::globalInit()
{
	int ncode = -1;
	int nlockJudge = -1;
	int nunlockJudge = 1;

	try
	{
		// prevent two threads init simultaneously
		nlockJudge = CurlManager::s_MutexLock.lock();
		ncode = nlockJudge;
		if (ncode != 0)
		{
			EPRINT("global init mutex lock error(%d)\n", errno);
			return ncode;
		}
		// init if not already initialized.
		if (s_bGlobalInitStatus == 0)
		{
			if (CURLE_OK == curl_global_init(CURL_GLOBAL_ALL))
				s_bGlobalInitStatus = 1;
			else
			{
				EPRINT("global init error(%d)\n", errno);
				ncode = -1;
			}
		}
		nunlockJudge = CurlManager::s_MutexLock.unlock();
		return ncode;
	}
	catch (...)
	{
		if (nlockJudge == 0 && nunlockJudge != 0)
			CurlManager::s_MutexLock.unlock();
		EPRINT("global init api exception(%d)\n", errno);
		return -1;
	}
}

int CurlManager::globalCleanup()
{
	int nLockJudge = -1;
	int nUnlockJudge = -1;
	int nCode = -1;

	try
	{
		nLockJudge = CurlManager::s_MutexLock.lock();
		nCode = nLockJudge;
		if (0 != nCode)
		{
			EPRINT("globalCleanup mutex lock error(%d)\n", errno);
			return nCode;
		}

		if (1 == s_bGlobalInitStatus)
		{
			curl_global_cleanup();
			s_bGlobalInitStatus = 0;
		}

		nUnlockJudge = CurlManager::s_MutexLock.unlock();
		nCode = nUnlockJudge;

		return nCode;

	}
	catch (...)
	{
		if (0 == nLockJudge && 0 != nUnlockJudge)
		{
			CurlManager::s_MutexLock.unlock();
		}
		EPRINT("globalCleanup api exception(%d)\n", errno);
		return -1;                           //Òì³£½Ó¿Ú
	}

}

int CurlManager::initCurlResource()
{
	this->m_pCurl = curl_easy_init();
	if (NULL == this->m_pCurl)
	{
		EPRINT("curl easy init failure \n");
		return -1;
	}
	else
	{
		return 0;
	}

}

int CurlManager::releaseCurlResource()
{
	if (NULL == this->m_pCurl)
	{
		EPRINT("releaseCurlResource curl ptr is null \n");
		return -1;
	}

	curl_easy_cleanup(this->m_pCurl);
	this->m_pCurl = NULL;

	return 0;
}

int CurlManager::setUserPwd()
{
	if (NULL == this->m_pCurl)
	{
		EPRINT("setUserPwd curl ptr is null \n");
		return -1;
	}

	std::string sUserPwd;
	if ((!(this->m_sUser.empty())) && (!(this->m_sPwd.empty())))
	{
		sUserPwd = this->m_sUser + ":" + this->m_sPwd;
		curl_easy_setopt(this->m_pCurl, CURLOPT_USERPWD, (char*)sUserPwd.c_str());
	}
	return 0;
}

void CurlManager::setConnectTimeout(unsigned int nTime)
{
	this->m_connect_timeout = nTime;
}

void CurlManager::setTimeout(unsigned int nTime)
{
	this->m_timeout = nTime;
}

#define  XML_FORMAT_STRING        "Content-Type: application/xml;charset=UTF-8"
#define  JSON_FORMAT_STRING       "Content-Type: application/json;charset=UTF-8"

int CurlManager::setDataFormat(const int nFormat)
{
	if (NULL == this->m_pCurl)
	{
		EPRINT("setDataFormat curl ptr is null \n");
		return -1;
	}

	std::string sFormatStr;
	if (FORMAT_XML == nFormat)
	{
		sFormatStr = XML_FORMAT_STRING;
	}
	else if (FORMAT_JSON == nFormat)
	{
		sFormatStr = JSON_FORMAT_STRING;
	}

	if (!sFormatStr.empty())
	{
		this->m_pHeader = curl_slist_append(NULL, (char*)sFormatStr.c_str());
		if (NULL == this->m_pHeader)
		{
			EPRINT("setDataFormat set format error(%d) \n", errno);
			return -1;
		}
		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPHEADER, this->m_pHeader);
	}

	return 0;
}

void CurlManager::setServer(const std::string& sIP,
	const unsigned int nPort,
	const std::string& sUser,
	const std::string& sPwd)
{
	this->m_sIP = sIP;
	this->m_nPort = nPort;
	this->m_sUser = sUser;
	this->m_sPwd = sPwd;
}

void CurlManager::setUrlPath(const std::string& sUrlPath)
{
	this->m_sUrlPath = sUrlPath;
}

size_t CurlManager::httpDataWriter(void* buffer, size_t size, size_t nmemb, void* content)
{
	long totalSize = static_cast<long>(size * nmemb);
	std::string* symbolBuffer = (std::string*)content;
	if (symbolBuffer)
	{
		symbolBuffer->append((char *)buffer, ((char*)buffer) + totalSize);
		return totalSize;
	}
	else
	{
		EPRINT("CurlManager httpDataWriter data error(%d) \n", errno);
		return 0;
	}

}

size_t CurlManager::httpReadFile(void* buffer, size_t size, size_t nmemb, void* file)
{
	return fread(buffer, size, nmemb, (FILE *)file);
}

size_t CurlManager::httpWriteFile(void* buffer, size_t size, size_t nmemb, void* file)
{
	return fwrite(buffer, size, nmemb, (FILE *)file);
}

void CurlManager::setUserNameAndPassword(const std::string& user, const std::string& pwd)
{
	m_sUser = user;
	m_sPwd = pwd;
}

void CurlManager::setSSLVerify(int flag)
{
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, flag);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, flag);
}

int CurlManager::sendMsg(const std::string& sMsg,
	const int nMethod,
	const int nFormat,
	std::string& sRec)
{
	int nCode = -1;
	sRec = "";
	switch (nMethod)
	{
	case METHOD_GET:
	{
		nCode = getMsg(sMsg, nFormat, sRec);
		return nCode;
	}
	case METHOD_POST:
	{
		nCode = postMsg(sMsg, nFormat, sRec);
		return nCode;
	}
	case METHOD_PUT:
	{
		nCode = putMsg(sMsg, nFormat, sRec);
		return nCode;
	}
	case METHOD_DELETE:
	{
		nCode = deleteMsg(sMsg, nFormat, sRec);
		return nCode;
	}
	default:
	{
		EPRINT("sendMsg method error\n");
		return -1;
	}
	}
}

int CurlManager::messagePublicMethod(const int nFormat)
{
	int nCode = -1;
	try
	{
		if (NULL == this->m_pCurl)
		{
			EPRINT("messagePublicMethod curl ptr is null\n");
			return -1;
		}

		if (0 > nFormat)
		{
			EPRINT("messagePublicMethod params error, nFormat=%d \n", nFormat);
			return -1;
		}
		std::string sUrl;
		if (this->m_sIP.empty())
			sUrl = this->m_sUrlPath;
		else
			sUrl = sUrl + "http://" + this->m_sIP + this->m_sUrlPath;

		// DPRINT("sUrl: %s\n", sUrl.c_str());
		curl_easy_setopt(this->m_pCurl, CURLOPT_URL, (char*)sUrl.c_str());
		curl_easy_setopt(this->m_pCurl, CURLOPT_PORT, this->m_nPort);

		nCode = setUserPwd();
		if (0 != nCode)
		{
			EPRINT("messagePublicMethod setUserPwd error(%d) \n", errno);
			return -1;
		}

		nCode = setDataFormat(nFormat);
		if (0 != nCode)
		{
			EPRINT("messagePublicMethod setDataFormat error(%d) \n", errno);
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_NOSIGNAL, 1);

		if (0 >= this->m_connect_timeout)
		{
			this->m_connect_timeout = CONNECT_DEFAULT_TIMEOUT;
		}
		curl_easy_setopt(this->m_pCurl, CURLOPT_CONNECTTIMEOUT, this->m_connect_timeout);

		if (0 >= this->m_timeout)
		{
			this->m_timeout = DEFAULT_TIMEOUT;
		}
		curl_easy_setopt(this->m_pCurl, CURLOPT_TIMEOUT, this->m_timeout);

		curl_easy_setopt(this->m_pCurl, CURLOPT_FORBID_REUSE, 1);

		return 0;

	}
	catch (...)
	{
		EPRINT("messagePublicMethod api exception(%d)\n", errno);
		return -1;
	}
}

int CurlManager::dealResCode(const CURLcode res)
{
	int nCode = 0;
	const char* pRes = NULL;
	pRes = curl_easy_strerror(res);
	DPRINT("%s\n", pRes);

	long lResCode = 0;
	curl_easy_getinfo(this->m_pCurl, CURLINFO_RESPONSE_CODE, &lResCode);

	if (CURLE_OK != res || 200 != lResCode)
	{
		if (CURLE_OPERATION_TIMEOUTED == res)
		{
			nCode = 1;
		}
		else
		{
			nCode = -1;
		}
		EPRINT("curl send msg error: pRes=%s, lResCode=%ld \n", pRes, lResCode);
	}

	return nCode;
}

int CurlManager::putMsg(const std::string& sMsg, const int nFormat, std::string& sRec)
{
	CURLcode res = CURLE_OK;
	int nCode = -1;

	try
	{
		if (NULL == this->m_pCurl)
		{
			EPRINT("putMsg curl ptr is null\n");
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_CUSTOMREQUEST, "PUT");
		if (sMsg.empty())
		{
			curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, "");
		}
		else
		{
			curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, (char*)sMsg.c_str());
		}

		nCode = messagePublicMethod(nFormat);
		if (0 != nCode)
		{
			EPRINT("putMsg call messagePublicMethod error(%d) \n", errno);
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, httpDataWriter);
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, (void*)&sRec);
		res = curl_easy_perform(this->m_pCurl);

		nCode = dealResCode(res);
		if (0 > nCode)
		{
			EPRINT("deal response code error \n");
		}

		return nCode;

	}
	catch (...)
	{
		EPRINT("putMsg api exception(%d)\n", errno);
		return -1;
	}
}

int CurlManager::deleteMsg(const std::string& sMsg, const int nFormat, std::string& sRec)
{
	CURLcode res = CURLE_OK;
	int nCode = -1;

	try
	{
		if (NULL == this->m_pCurl)
		{
			EPRINT("deleteMsg curl ptr is null\n");
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_CUSTOMREQUEST, "DELETE");

		nCode = messagePublicMethod(nFormat);
		if (0 != nCode)
		{
			EPRINT("deleteMsg call messagePublicMethod error(%d) \n", errno);
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, httpDataWriter);
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, (void*)&sRec);
		res = curl_easy_perform(this->m_pCurl);

		nCode = dealResCode(res);
		if (0 > nCode)
		{
			EPRINT("deal response code error \n");
		}

		return nCode;
	}
	catch (...)
	{
		EPRINT("deleteMsg api exception(%d)\n", errno);
		return -1;
	}

}

int CurlManager::postMsg(const std::string& sMsg, const int nFormat, std::string& sRec)
{
	CURLcode res = CURLE_OK;
	int nCode = -1;

	try
	{
		if (NULL == this->m_pCurl)
		{
			EPRINT("postMsg curl ptr is null\n");
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_POST, 1);
		if (sMsg.empty())
		{
			curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, "");
		}
		else
		{
			curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, (char*)sMsg.c_str());
		}

		nCode = messagePublicMethod(nFormat);
		if (0 != nCode)
		{
			EPRINT("postMsg call messagePublicMethod error(%d) \n", errno);
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, httpDataWriter);
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, (void*)&sRec);
		res = curl_easy_perform(this->m_pCurl);

		nCode = dealResCode(res);
		if (0 > nCode)
		{
			EPRINT("deal response code error \n");
		}

		return nCode;
	}
	catch (...)
	{
		EPRINT("postMsg api exception(%d)\n", errno);
		return -1;
	}
}

int CurlManager::getMsg(const std::string& sMsg, const int nFormat, std::string& sRec)
{
	CURLcode res = CURLE_OK;
	int nCode = -1;

	try
	{
		if (NULL == this->m_pCurl)
		{
			EPRINT("getMsg curl ptr is null\n");
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPGET, 1);

		nCode = messagePublicMethod(nFormat);
		if (0 != nCode)
		{
			EPRINT("getMsg call messagePublicMethod error(%d) \n", errno);
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, httpDataWriter);
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, (void*)&sRec);
		res = curl_easy_perform(this->m_pCurl);

		nCode = dealResCode(res);
		if (0 > nCode)
		{
			EPRINT("deal response code error \n");
		}

		return nCode;

	}
	catch (...)
	{
		EPRINT("getMsg api exception(%d)\n", errno);
		return -1;
	}

}

int CurlManager::downloadFile(const std::string& sFileName, const int nFormat)
{
	CURLcode res = CURLE_OK;
	FILE* pFile = NULL;
	int nCode = -1;

	try
	{
		if (sFileName.empty())
		{
			EPRINT("downloadFile filename is empty\n");
			return -1;
		}
		pFile = fopen((char*)sFileName.c_str(), "w");
		if (NULL == pFile)
		{
			EPRINT("downloadFile open file error(%d), %s\n", errno, (char*)sFileName.c_str());
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPGET, 1);

		nCode = messagePublicMethod(nFormat);
		if (0 != nCode)
		{
			if (NULL != pFile)
			{
				fclose(pFile);
				pFile = NULL;
			}

			EPRINT("downloadFile call messagePublicMethod error(%d) \n", errno);
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, httpWriteFile);
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, pFile);
		res = curl_easy_perform(this->m_pCurl);

		nCode = dealResCode(res);
		if (0 > nCode)
		{
			EPRINT("deal response code error \n");
		}

		fclose(pFile);
		pFile = NULL;

		return nCode;
	}
	catch (...)
	{
		if (NULL != pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
		EPRINT("downloadFile api exception(%d)\n", errno);
		return -1;
	}

}

int CurlManager::uploadFileContent(const std::string& sFileName, const int nFormat, std::string& sRec)
{
	CURLcode res = CURLE_OK;
	FILE* pFile = NULL;
	struct stat file_info;
	curl_off_t fsize;
	int nCode = 0;

	try
	{
		if (sFileName.empty())
		{
			EPRINT("uploadFileContent filename is empty\n");
			return -1;
		}

		if (stat((char*)sFileName.c_str(), &file_info))
		{
			EPRINT("uploadFileContent get file info error(%d), %s\n", errno, (char*)sFileName.c_str());
			return -1;
		}
		fsize = (curl_off_t)file_info.st_size;

		pFile = fopen((char*)sFileName.c_str(), "rb");
		if (NULL == pFile)
		{
			EPRINT("uploadFileContent open file error(%d), %s\n", errno, (char*)sFileName.c_str());
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_POST, 1);

		nCode = messagePublicMethod(nFormat);
		if (0 != nCode)
		{
			if (NULL != pFile)
			{
				fclose(pFile);
				pFile = NULL;
			}

			EPRINT("uploadFileContent call messagePublicMethod error(%d) \n", errno);
			return -1;
		}

		curl_easy_setopt(this->m_pCurl, CURLOPT_READFUNCTION, httpReadFile);
		curl_easy_setopt(this->m_pCurl, CURLOPT_READDATA, pFile);
		curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDSIZE, (curl_off_t)fsize);

		sRec = "";
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, httpDataWriter);
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, (void*)&sRec);
		res = curl_easy_perform(this->m_pCurl);

		nCode = dealResCode(res);
		if (0 > nCode)
		{
			EPRINT("deal response code error \n");
		}

		fclose(pFile);
		pFile = NULL;

		return nCode;

	}
	catch (...)
	{
		if (NULL != pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
		EPRINT("uploadFileContent api exception(%d)\n", errno);
		return -1;
	}

}

#define FILE_NAME_MAX_SIZE      (128)

const char* CurlManager::getFileName(const char *path)
{
	if (!path)
	{
		return NULL;
	}

	const char *pname = strrchr(path, '/');
	if (!pname)
	{
		return path;
	}

	return (char*)(pname + 1);
}

int CurlManager::uploadFile(const std::string& sFileFullname, std::string& sRec)
{
	CURLcode res = CURLE_OK;
	int nCode = 0;
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	try
	{
		if (sFileFullname.empty())
		{
			EPRINT("uploadFile sFileFullname is empty\n");
			return -1;
		}

		const char* pFileName = getFileName(sFileFullname.c_str());
		if (NULL == pFileName || '\0' == pFileName[0])
		{
			EPRINT("uploadFileContent call getFileName failure, sFileFullname=%s \n", sFileFullname.c_str());
			return -1;
		}
		DPRINT("uploadFile pFileName=%s \n", pFileName);

		nCode = messagePublicMethod(FORMAT_DEFAULT);
		if (0 != nCode)
		{
			EPRINT("uploadFile call messagePublicMethod error(%d) \n", errno);
			return -1;
		}

		/*
		Fill in the file upload field. This makes libcurl load data from
		the given file name when curl_easy_perform() is called.
		*/
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "sendfile",
			CURLFORM_FILE, sFileFullname.c_str(),
			CURLFORM_END);

		/* Fill in the filename field */
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "filename",
			CURLFORM_COPYCONTENTS, pFileName,
			CURLFORM_END);

		/* Fill in the submit field too, even if this is rarely needed */
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "submit",
			CURLFORM_COPYCONTENTS, "send",
			CURLFORM_END);

		headerlist = curl_slist_append(headerlist, buf);

		/* only disable 100-continue header if explicitly requested */
		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(this->m_pCurl, CURLOPT_HTTPPOST, formpost);

		sRec = "";
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, httpDataWriter);
		curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, (void*)&sRec);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(this->m_pCurl);

		/* then cleanup the formpost chain */
		if (NULL != formpost)
		{
			curl_formfree(formpost);
			formpost = NULL;
		}

		/* free slist */
		if (NULL != headerlist)
		{
			curl_slist_free_all(headerlist);
			headerlist = NULL;
		}

		nCode = dealResCode(res);
		if (0 > nCode)
		{
			EPRINT("deal response code error \n");
		}

		return nCode;

	}
	catch (...)
	{
		if (NULL != formpost)
		{
			curl_formfree(formpost);
			formpost = NULL;
		}

		if (NULL != headerlist)
		{
			curl_slist_free_all(headerlist);
			headerlist = NULL;
		}

		EPRINT("uploadFile api exception(%d)\n", errno);
		return -1;
	}

}
