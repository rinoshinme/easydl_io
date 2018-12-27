#ifndef EASYDL_TEST_H
#define EASYDL_TEST_H

#include "dataset_manager.h"
#include "dataset_message.h"
using namespace msg;
#include "utility.h"

void upload_data()
{
	// init curl
	curl_global_init(CURL_GLOBAL_ALL);

	DatasetManager manager;
	// 1. get access key
	std::string access_token;
	// get access_token from keys of app.
	std::string api_key("LjP5XdTv89MmBvWM3vp8j3hp");
	std::string secret_key("6yvux7FiHpi5GU0l8lVoZXZIPHDlRaym");
	manager.getAccessToken(api_key, secret_key);
	access_token = manager.getAT();
	std::cout << access_token << std::endl;

#if 0
	// 2. create new object detection dataset
	DatasetCreate dataset_create(access_token, "OBJECT_DETECTION", "fishing_detection");
	DatasetCreateRet dataset_create_ret;
	manager.process(access_token, dataset_create, dataset_create_ret);

#else
	// 3. upload new images and labels
	// -- xml label files should first be converted to text files using python and PascalVOCXML
	std::string image_folder("F:\\调研 - EasyDL\\fishing_data\\JPEGImages");
	std::string label_folder("F:\\调研 - EasyDL\\fishing_data\\Annotations_text");
	std::vector<std::string> image_files = listFiles(image_folder);
	std::vector<std::string> label_files = listFiles(label_folder);
	int num_images = int(image_files.size());
	// add into fishing_detection dataset, id = 22593
	msg::DatasetAdd data_add(access_token, "OBJECT_DETECTION", 22593, true);
	msg::DatasetAddRet data_add_ret;
	for (int i = 0; i < num_images; ++i)
	{
		std::cout << "processing image " << i + 1 << "/" << num_images << std::endl;
		data_add.loadFromTextFile(label_files[i]);
		data_add.loadContentFromFile(image_files[i]);
		manager.process(access_token, data_add, data_add_ret);
	}

#endif

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
