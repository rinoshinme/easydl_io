/*
 * message definitions
 */
#ifndef DATASET_MESSAGE_H
#define DATASET_MESSAGE_H

#include <string>
#include <vector>
#include <json/json.h>
#include <sstream>
#include <iostream>
#include "curl_manager.h" // for HttpMethod

namespace msg
{
	class MessageBase
	{
	public:
		HttpMethod method; // 
		std::string url;
		std::string access_token; // in constructor, 通过API Key和Secret Key获取，需要定期更新

	public:
		MessageBase(const std::string& access_token);
		virtual inline std::string toJsonString() const { return ""; }
		virtual inline std::string toString(Json::Value& root) const;
		virtual inline void show() const
		{
			std::string method_str;
			switch (method)
			{
			case HttpMethod::METHOD_POST :
				method_str = "METHOD_POST";
				break;
			case HttpMethod::METHOD_GET:
				method_str = "METHOD_GET";
				break;
			case HttpMethod::METHOD_PUT:
				method_str = "METHOD_PUT";
				break;
			case HttpMethod::METHOD_DELETE:
				method_str = "METHOD_DELETE";
				break;
			default:
				method_str = "METHOD_POST";
				break;
			}
			std::cout << "method = " << method_str << std::endl;
			std::cout << "url = " << url << std::endl;
		}
	};

	struct MessageReturnBase
	{
		// return value: 1: parse ok, 0: error message, -1: parse failed.
		virtual int parseReturnMessage(const std::string& message) 
		{
			std::cout << message << std::endl;
			return 1;
		}
		virtual void show() const { std::cout << "show return message:" << std::endl; }
	};

	struct MessageError : public MessageReturnBase
	{
		int error_code;
		std::string error_msg;
		inline int parseReturnMessage(const std::string& message)
		{
			const char* msg = message.c_str();
			size_t length = message.size();

			Json::CharReaderBuilder b;
			Json::CharReader* reader(b.newCharReader());
			Json::Value root;
			std::string err;
			bool ok = reader->parse(msg, msg + length, &root, &err);
			if (ok && err.size() == 0)
			{
				if (root["error_code"].isInt())
					error_code = root["error_code"].asInt();
				if (root["error_msg"].isString())
					error_msg = root["error_msg"].asString();
				return 0;
			}
			return -1; // error message
		}

		virtual inline void show() const
		{
			std::cout << "error_code = " << error_code << std::endl;
			std::cout << "error_msg = " << error_msg << std::endl;
		}
	};

	// Dataset creation
	struct DatasetCreate : public MessageBase
	{
		std::string type;
		std::string dataset_name;

	public:
		DatasetCreate(const std::string& access_token, const std::string& type, const std::string& dataset_name)
			:MessageBase(access_token)
		{
			method = METHOD_POST;
			url = "https://aip.baidubce.com/rpc/2.0/easydl/dataset/create";
			this->type = type;
			this->dataset_name = dataset_name;
		}

		inline std::string toJsonString() const
		{
			Json::Value root;
			root["type"] = type;
			root["dataset_name"] = dataset_name;
			return toString(root);
		}
	};

	struct DatasetCreateRet : public MessageReturnBase
	{
		int log_id;
		int dataset_id;

		inline int parseReturnMessage(const std::string& message)
		{
			const char* msg = message.c_str();
			size_t length = message.size();

			Json::CharReaderBuilder b;
			Json::CharReader* reader(b.newCharReader());
			Json::Value root;
			std::string err;
			bool ok = reader->parse(msg, msg + length, &root, &err);
			if (ok && err.size() == 0)
			{
				if (!root["error_code"].isNull())
					return 0; // error message

				if (root["log_id"].isInt())
					log_id = root["log_id"].asInt();
				if (root["dataset_id"].isInt())
					dataset_id = root["dataset_id"].asInt();
				return 1;
			}
			return -1;
		}
	};

	// Dataset list
	struct DatasetList : public MessageBase
	{
		std::string type;
		int start;
		int num;

		DatasetList(const std::string& access_token, const std::string& type, int start = 0, int num = 20)
			:MessageBase(access_token)
		{
			method = METHOD_POST;
			url = "https://aip.baidubce.com/rpc/2.0/easydl/dataset/list";
			this->type = type;
			this->start = start;
			this->num = num;
		}
		
		std::string toJsonString() const
		{
			Json::Value root;
			root["type"] = type;
			root["start"] = start;
			root["num"] = num;
			return toString(root);
		}
	};

	struct DatasetListRet : public MessageReturnBase
	{
		int log_id;
		int total_num;
		// results;
		std::vector<int> dataset_id;
		std::vector<std::string> dataset_name;
		std::vector<std::string> type;
		std::vector<std::string> status;

		inline int parseReturnMessage(const std::string& message)
		{
			const char* msg = message.c_str();
			size_t length = message.size();

			Json::CharReaderBuilder b;
			Json::CharReader* reader(b.newCharReader());
			Json::Value root;
			std::string err;
			bool ok = reader->parse(msg, msg + length, &root, &err);
			if (ok && err.size() == 0)
			{
				if (!root["error_code"].isNull())
					return 0; // error message
				if (root["log_id"].isInt())
					log_id = root["log_id"].asInt();
				if (root["total_num"].isInt())
					total_num = root["total_num"].asInt();
				// parse datasets
				dataset_id.clear();
				dataset_name.clear();
				type.clear();
				status.clear();
				Json::Value results = root["results"];
				for (int i = 0; i < int(results.size()); ++i)
				{
					Json::Value item = results[i];
					dataset_id.push_back(item["dataset_id"].asInt());
					dataset_name.push_back(item["dataset_name"].asString());
					type.push_back(item["type"].asString());
					status.push_back(item["status"].asString());
				}
				return 1;
			}
			return -1;
		}
	};

	// classification label list
	struct ClassificationLabelList : public MessageBase
	{
		std::string type;
		int dataset_id;
		int start;
		int num;

		ClassificationLabelList(const std::string& access_token, const std::string& type, 
			int dataset_id, int start = 0, int num = 20)
			:MessageBase(access_token)
		{
			method = METHOD_POST;
			url = "https://aip.baidubce.com/rpc/2.0/easydl/label/list";
			this->type = type;
			this->dataset_id = dataset_id;
			this->start = start;
			this->num = num;
		}

		std::string toJsonString() const
		{
			Json::Value root;
			root["type"] = type;
			root["dataset_id"] = dataset_id;
			root["start"] = start;
			root["num"] = num;
			return toString(root);
		}
	};

	struct ClassificationLabelListRet : public MessageReturnBase
	{
		int log_id;
		int total_num;
		// results;
		std::vector<std::string> label_id;
		std::vector<std::string> label_name;
		std::vector<int> entity_count; // label对应的数量，图片，声音，文本。

		inline int parseReturnMessage(const std::string& message)
		{
			const char* msg = message.c_str();
			size_t length = message.size();

			Json::CharReaderBuilder b;
			Json::CharReader* reader(b.newCharReader());
			Json::Value root;
			std::string err;
			bool ok = reader->parse(msg, msg + length, &root, &err);
			if (ok && err.size() == 0)
			{
				if (!root["error_code"].isNull())
					return 0; // error message
				if (root["log_id"].isInt())
					log_id = root["log_id"].asInt();
				if (root["total_num"].isInt())
					total_num = root["total_num"].asInt();
				// parse datasets
				label_id.clear();
				label_name.clear();
				entity_count.clear();

				Json::Value results = root["results"];
				if (results.isNull())
					return -1;

				for (int i = 0; i < int(results.size()); ++i)
				{
					Json::Value item = results[i];
					label_id.push_back(item["label_id"].asString());
					label_name.push_back(item["label_name"].asString());
					entity_count.push_back(item["entity_count"].asInt());
				}
				return 1;
			}
			return -1;
		}

		inline void show() const
		{
			std::cout << "log_id = " << log_id << std::endl;
			std::cout << "total_num = " << total_num << std::endl;
			for (size_t k = 0; k < label_id.size(); ++k)
			{
				std::cout << "\tlabel_id = " << label_id[k] << std::endl;
				std::cout << "\tlabel_name = " << label_name[k] << std::endl;
				std::cout << "\tentity_count = " << entity_count[k] << std::endl;
			}
		}
	};

	// Dataset Add
	// this structure represents a single sample
	struct DatasetAdd : public MessageBase
	{
		std::string type;
		int dataset_id;
		bool append_label; // true为追加，false为替换
		std::string entity_content; // 图像，声音，文本内容，base64
		std::string entity_name; // 文件名
		// labels
		std::vector<std::string> label_name;
		// 以下只有物体检测需要。
		std::vector<int> left;
		std::vector<int> top;
		std::vector<int> width;
		std::vector<int> height;

		DatasetAdd(const std::string& access_token, int dataset_id, bool append_label)
			:MessageBase(access_token)
		{
			method = METHOD_POST;
			url = "https://aip.baidubce.com/rpc/2.0/easydl/dataset/addentity";

			this->type = type;
			this->dataset_id = dataset_id;
			this->append_label = append_label;
			// other fields should be further modified or added.
		}

		std::string toJsonString() const
		{
			Json::Value root;
			root["type"] = type;
			root["dataset_id"] = dataset_id;
			root["appendLabel"] = append_label;
			root["entity_content"] = entity_content;
			root["entity_name"] = entity_name;
			Json::Value labels_root;
			int size = int(label_name.size());
			bool has_box = (left.size() > 0);
			for (int k = 0; k < size; ++k)
			{
				labels_root[k]["label_name"] = label_name[k];
				if (has_box)
				{
					labels_root[k]["left"] = left[k];
					labels_root[k]["top"] = top[k];
					labels_root[k]["width"] = width[k];
					labels_root[k]["height"] = height[k];
				}
			}
			root["labels"] = labels_root;
			return toString(root);
		}

	};

	struct DatasetAddRet : public MessageReturnBase
	{
		int log_id;
		inline int parseReturnMessage(const std::string& message)
		{
			const char* msg = message.c_str();
			size_t length = message.size();

			Json::CharReaderBuilder b;
			Json::CharReader* reader(b.newCharReader());
			Json::Value root;
			std::string err;
			bool ok = reader->parse(msg, msg + length, &root, &err);
			if (ok && err.size() == 0)
			{
				if (!root["error_code"].isNull())
					return 0; // error message
				if (root["log_id"].isInt())
					log_id = root["log_id"].asInt();
				return 1;
			}
			return -1;
		}
	};

	// dataset delete
	struct DatasetDelete : public MessageBase
	{
		std::string type;
		int dataset_id;
		DatasetDelete(const std::string& access_token, const std::string& type, int dataset_id)
			:MessageBase(access_token)
		{
			method = METHOD_POST;
			url = "https://aip.baidubce.com/rpc/2.0/easydl/dataset/delete";

			this->type = type;
			this->dataset_id = dataset_id;
		}

		std::string toJsonString() const
		{
			Json::Value root;
			root["type"] = type;
			root["dataset_id"] = dataset_id;
			return toString(root);
		}
	};

	struct DatasetDeleteRet : MessageReturnBase
	{
		int log_id;

		inline int parseReturnMessage(const std::string& message)
		{
			const char* msg = message.c_str();
			size_t length = message.size();

			Json::CharReaderBuilder b;
			Json::CharReader* reader(b.newCharReader());
			Json::Value root;
			std::string err;
			bool ok = reader->parse(msg, msg + length, &root, &err);
			if (ok && err.size() == 0)
			{
				if (!root["error_code"].isNull())
					return 0; // error message
				if (root["log_id"].isInt())
					log_id = root["log_id"].asInt();
				return 1;
			}
			return -1;
		}
	};

	// classification, label delete
	struct ClassificationLabelDelete : public MessageBase
	{
		std::string type;
		int dataset_id;
		std::string label_name;

		ClassificationLabelDelete(const std::string& access_token, const std::string& type,
			int dataset_id, const std::string& label_name)
			:MessageBase(access_token)
		{
			method = METHOD_POST;
			url = "https://aip.baidubce.com/rpc/2.0/easydl/label/delete";
			this->type = type;
			this->dataset_id = dataset_id;
			this->label_name = label_name;
		}

		std::string toJsonString() const
		{
			Json::Value root;
			root["type"] = type;
			root["dataset_id"] = dataset_id;
			root["label_name"] = label_name;
			return toString(root);
		}
	};

	struct ClassificationLabelDeleteRet : public MessageReturnBase
	{
		int log_id;
		inline int parseReturnMessage(const std::string& message)
		{
			const char* msg = message.c_str();
			size_t length = message.size();

			Json::CharReaderBuilder b;
			Json::CharReader* reader(b.newCharReader());
			Json::Value root;
			std::string err;
			bool ok = reader->parse(msg, msg + length, &root, &err);
			if (ok && err.size() == 0)
			{
				if (!root["error_code"].isNull())
					return 0; // error message
				if (root["log_id"].isInt())
					log_id = root["log_id"].asInt();
				return 1;
			}
			return -1;
		}
	};

}

#endif
