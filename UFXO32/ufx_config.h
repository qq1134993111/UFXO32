#pragma once

#include<string>
#include <sstream>
#include <unordered_map>

#include"error_code.h"

//#include "boost/filesystem.hpp"
//#include "boost/algorithm/string.hpp"
//#include "boost/format.hpp"
//#include "boost/property_tree/ptree.hpp"
//#include "boost/property_tree/json_parser.hpp"
//#include "boost/property_tree/xml_parser.hpp"
//#include <boost/exception/all.hpp>

class UfxConfig
{
public:
	UfxConfig() {}
	virtual ~UfxConfig() {}
public:
	struct UfxConfigData
	{
		std::string operator_no;
		std::string password;
		std::string authorization_id;
		std::string account_code;
		std::string asset_no;
		std::string combi_no;
		std::string cust_id;
		std::string pbuid;


		std::string servers;
		std::string license_file;

		std::string cert_file;
		std::string cert_pwd;
		std::string safe_level;

		std::string client_name;
		std::string biz_name;
		std::string topic_name;
		bool is_rebulid;
		bool is_replace;
		int32_t send_interval;
		std::unordered_map<std::string, std::string> filter_map;

	};
	ErrorCode Load(const std::string& data_path);

	ErrorCode Load(std::basic_istream<char>& stream);

	const UfxConfigData& GetConfigData()
	{
		return config_data_;
	}
private:
	UfxConfigData config_data_;
};
