#include <iostream>
#include <string>
#include "curl/curl.h"
#include "easydl_test.h"

int main()
{

	// test_easydl_function();
	upload_data();

#if 0
	std::string image_folder("F:\\调研 - EasyDL\\fishing_data\\JPEGImages");
	std::string label_folder("F:\\调研 - EasyDL\\fishing_data\\Annotations_text");
	std::vector<std::string> image_files = listFiles(image_folder);
	std::vector<std::string> label_files = listFiles(label_folder);
	for (size_t k = 0; k < label_files.size(); ++k)
		std::cout << label_files[k] << std::endl;
	std::cout << label_files.size() << std::endl;
#endif

	std::cout << "finished...\n";
	std::getchar();
}
