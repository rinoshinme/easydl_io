#include "dataset_manager.h"
#include <json/json.h>
#include <iostream>

size_t callback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	// std::string s((char*)ptr, size * nmemb);
	// Json::Reader reader;
	Json::CharReaderBuilder b;
	Json::CharReader* reader(b.newCharReader());
	Json::Value root;
	JSONCPP_STRING errs;
	char* s = static_cast<char*>(ptr);
	bool ok = reader->parse(s, s + size * nmemb, &root, &errs);
	if (ok && errs.size() == 0)
	{
		std::string* access_token_result = static_cast<std::string*>(stream);
		*access_token_result = root["access_token"].asString();
	}
	delete reader;
	return size * nmemb;
}

int DatasetManager::getAccessToken(const std::string& api_key, const std::string& secret_key)
{
	CURL *curl;
	CURLcode result_code;
	int error_code = 0;
	curl = curl_easy_init();
	if (curl) {
		const std::string access_token_url = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials";
		std::string url = access_token_url + "&client_id=" + api_key + "&client_secret=" + secret_key;

		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		
		std::string access_token_result;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &access_token_result);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		result_code = curl_easy_perform(curl);
		if (result_code != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(result_code));
			return 1;
		}
		access_token = access_token_result;
		curl_easy_cleanup(curl);
		error_code = 0;
	}
	else {
		fprintf(stderr, "curl_easy_init() failed.");
		error_code = 1;
	}
	return error_code;
}

int DatasetManager::doDatasetCreate(const std::string& access_token, const msg::DatasetCreate& data, msg::DatasetCreateRet& ret)
{
	CurlManager* pCurl = new CurlManager;
	pCurl->initCurlResource();
	const std::string dataset_create_url = "https://aip.baidubce.com/rpc/2.0/easydl/dataset/create?";
	std::string url = dataset_create_url + "access_token=" + access_token;
	
	pCurl->setUrlPath(url);

	int nMethod = METHOD_POST;
	int nFormat = FORMAT_JSON;

	std::string msg = data.toJsonString();
	std::string response;
	pCurl->setSSLVerify(0);
	pCurl->sendMsg(msg, nMethod, nFormat, response);

	try
	{
		ret.parseReturnMessage(response);
	}
	catch (...)
	{
		msg::MessageError error;
		error.parseReturnMessage(response);
	}
	
	delete pCurl;
	return 0;
}

int DatasetManager::doDatasetList(const std::string& access_token, const msg::DatasetList& data, msg::DatasetListRet& ret)
{
	CurlManager* pCurl = new CurlManager;
	pCurl->initCurlResource();
	const std::string dataset_create_url = "https://aip.baidubce.com/rpc/2.0/easydl/dataset/list?";
	std::string url = dataset_create_url + "access_token=" + access_token;
	std::cout << url << std::endl;
	pCurl->setUrlPath(url);

	int nMethod = METHOD_POST;
	int nFormat = FORMAT_JSON;

	std::string msg = data.toJsonString();
	std::string response;
	pCurl->setSSLVerify(0);
	pCurl->sendMsg(msg, nMethod, nFormat, response);
	try
	{
		ret.parseReturnMessage(response);
	}
	catch (...)
	{
		msg::MessageError error;
		error.parseReturnMessage(response);
	}

	delete pCurl;
	return 0;
}

