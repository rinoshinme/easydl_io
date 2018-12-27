#include "dataset_message.h"

namespace msg
{
	// message base
	MessageBase::MessageBase(const std::string& access_token)
	{
		this->access_token = access_token;
	}

	std::string MessageBase::toString(Json::Value& root) const
	{
		Json::StreamWriterBuilder b;
		Json::StreamWriter* writer(b.newStreamWriter());
		std::ostringstream stream;
		writer->write(root, &stream);
		std::string str = stream.str();
		delete writer;
		return str;
	}
}
