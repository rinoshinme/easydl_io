#ifndef EASYDL_TEST_H
#define EASYDL_TEST_H

#include "dataset_manager.h"
#include "dataset_message.h"

void upload_data()
{
	// init curl
	curl_global_init(CURL_GLOBAL_ALL);

	// 1. get access key

	// 2. create new object detection dataset

	// 3. upload new images and labels
	// -- xml label files should first be converted to text files using python and PascalVOCXML





	// clean curl
	curl_global_cleanup();
}





void test_easydl_function()
{
	// init curl
	curl_global_init(CURL_GLOBAL_ALL);

#if 0
	// std::string fname = "F:/a.txt";
	getUrl("F:/get.html");
	postUrl("F:/post.html");
#endif

	DatasetManager manager;
	std::string access_token;
	// get access_token from keys of app.
	manager.getAccessToken("LjP5XdTv89MmBvWM3vp8j3hp", "6yvux7FiHpi5GU0l8lVoZXZIPHDlRaym");
	access_token = manager.getAT();
	std::cout << access_token << std::endl;

#if 1
	msg::DatasetList data_list(access_token, "OBJECT_DETECTION");
	msg::DatasetListRet ret_list;
	// manager.doDatasetList(access_token, data_list, ret_list);
	// manager.process(access_token, data_list, ret_list);

	msg::ClassificationLabelList label_list(access_token, "OBJECT_DETECTION", 21997);
	msg::ClassificationLabelListRet ret_label_list;
	manager.process(access_token, label_list, ret_label_list);

#endif

	// clean curl
	curl_global_cleanup();
}


#endif
