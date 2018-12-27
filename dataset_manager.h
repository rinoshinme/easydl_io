#ifndef DATASET_MANAGER_H
#define DATASET_MANAGER_H

#include "curl_manager.h"
#include "dataset_message.h"
// 需要libcurl编译支持HTTPS协议

// definitions
static size_t access_token_callback(void* ptr, size_t size, size_t nmemb, void* stream);

// provide clean api for EasyDL dataset management.
class DatasetManager
{
private:
	std::string access_token;

public:
	// access token
	int getAccessToken(const std::string& api_key, const std::string& secret_key);
	std::string getAT() const { return access_token; }


	int doDatasetCreate(const std::string& access_token, const msg::DatasetCreate& data, msg::DatasetCreateRet& ret);
	int doDatasetList(const std::string& access_token, const msg::DatasetList& data, msg::DatasetListRet& ret);
	int doClassificationLabelList(const std::string& access_token, const msg::ClassificationLabelList& data, msg::ClassificationLabelListRet& ret);
	int doDatasetAdd(const std::string& access_token, const msg::DatasetAdd& data, msg::DatasetAddRet& ret);
	int doDatasetDelete(const std::string& access_token, const msg::DatasetDelete& data, msg::DatasetDeleteRet& ret);
	int doClassificationLabelDelete(const std::string& access_token, const msg::ClassificationLabelDelete& data, msg::ClassificationLabelDeleteRet& ret);

	// generic task completion, preferred.
	template<typename MessageType, typename MessageReturnType>
	int process(const std::string& access_token, const MessageType& data, MessageReturnType& ret)
	{
		CurlManager* pCurl = new CurlManager;
		pCurl->initCurlResource();
		
		std::string url = data.url;
		url = url + "?access_token=" + access_token;
		// std::cout << url << std::endl;
		pCurl->setUrlPath(url);

		int nMethod = data.method;
		int nFormat = FORMAT_JSON;

		std::string msg = data.toJsonString();
		std::string response;
		pCurl->setSSLVerify(0);
		pCurl->sendMsg(msg, nMethod, nFormat, response);
		int code = ret.parseReturnMessage(response);
		delete pCurl;

		if (code == 0)
		{
			// error message
			msg::MessageError error;
			code = error.parseReturnMessage(response);
			if (code == 0)
				error.show();
		}
		else
		{
			// ret.show();
		}
		return code;
	}

};

#endif
