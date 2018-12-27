#include "utility.h"
#include <fstream>
#include <io.h>
#include "base64.h"

std::vector<std::string> stringSplit(const std::string& line, char sep)
{
	std::vector<std::string> words;
	std::string word;

	for (size_t i = 0; i < line.size(); ++i)
	{
		if (line[i] != sep)
			word.push_back(line[i]);
		else
		{
			if (word.size() > 0)
			{
				words.push_back(word);
				word.clear();
			}
		}
	}
	if (word.size() > 0)
		words.push_back(word);
	return words;
}

std::string stringTrimL(const std::string& input)
{
	size_t i;
	for (i = 0; i < input.size(); ++i)
		if (!isspace(input[i]))
			break;
	return input.substr(i, input.size());
}

std::string stringTrimR(const std::string& input)
{
	int i;
	for (i = int(input.size()) - 1; i >= 0; --i)
		if (!isspace(input[i]))
			break;
	return input.substr(0, i + 1);
}

std::string stringTrim(const std::string& input)
{
	std::string temp = stringTrimL(input);
	return stringTrimR(temp);
}

bool readBase64Content(const std::string& file, std::string& content)
{
	std::ifstream reader;
	reader.open(file, std::ios::in | std::ios::binary);
	if (!reader.is_open())
		return false;

	reader.seekg(0, std::ios::end);
	std::ios::pos_type pos = reader.tellg();
	unsigned int file_size = static_cast<unsigned int>(pos);
	reader.seekg(0, std::ios::beg);
	char* buffer = new char[file_size];
	reader.read(buffer, file_size);

	content = base64::base64_encode((unsigned char*)buffer, file_size);

	return true;
}

std::vector<std::string> listFiles(const std::string& dir)
{
	std::vector<std::string> files;

	intptr_t hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(dir).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (fileinfo.attrib & _A_SUBDIR)
			{
				continue;
			}
			else
				files.push_back(p.assign(dir).append("\\").append(fileinfo.name));
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return files;
}
