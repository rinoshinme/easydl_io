#ifndef CURL_MANAGER_TEST_H
#define CURL_MANAGER_TEST_H

#include "curl_manager.h"

// send message via GET or DELETE
int curl_get_message();
// send message via POST or PUT
int curl_post_message();
// file upload
int curl_upload_file();
// file download
int curl_download_file();

inline bool getUrl(char *filename)
{
	CURL *curl;
	CURLcode res;
	FILE *fp;
	if ((fp = fopen(filename, "w")) == NULL)  // ���ؽ�����ļ��洢
		return false;
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: Agent-007");
	curl = curl_easy_init();    // ��ʼ��
	if (curl)
	{
		//curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");// ����
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);// ��Э��ͷ
		curl_easy_setopt(curl, CURLOPT_URL, "http://www.baidu.com");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //�����ص�httpͷ�����fpָ����ļ�
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, fp); //�����ص�html�������������fpָ����ļ�
		res = curl_easy_perform(curl);   // ִ��
		if (res != 0) {

			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
		}
		fclose(fp);
		return true;
	}
	return false;
}

inline bool postUrl(char *filename)
{
	CURL *curl;
	CURLcode res;
	FILE *fp;
	if ((fp = fopen(filename, "w")) == NULL)
		return false;
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookie.txt"); // ָ��cookie�ļ�
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "&logintype=uid&u=xieyan&psw=xxx86");    // ָ��post����
		//curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");
		curl_easy_setopt(curl, CURLOPT_URL, " http://mail.sina.com.cn/cgi-bin/login.cgi ");   // ָ��url
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	fclose(fp);
	return true;
}

#endif