#include "ufx_config.h"


#include <fstream>
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include <boost/exception/all.hpp>

ErrorCode UfxConfig::Load(const std::string& data_path)
{
	try
	{
		boost::filesystem::path file_path(data_path);
		if (boost::filesystem::is_directory(file_path))
		{
			file_path /= "ufx_config.xml";
		}

		if (!boost::filesystem::exists(file_path))
		{
			auto fmt = boost::format("Load config data error,file %1% not exist") % file_path;
			return { -1,fmt.str() };
		}

		std::fstream f;
		f.open(file_path.c_str());
		if (!f.is_open())
		{
			boost::format fmt("Load config data error,open file %1% failed");
			fmt%file_path;
			return { -1,fmt.str() };
		}

		return Load(f);
	}
	catch (std::exception& e)
	{
		auto fmt = boost::format("Load config data error,exception:%1%,%2% ") % e.what() % boost::diagnostic_information(e);
		return { -1,fmt.str() };
	}
	catch (...)
	{
		return { -1,boost::current_exception_diagnostic_information() };
	}
}

ErrorCode UfxConfig::Load(std::basic_istream<char>& stream)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml(stream, pt);
	boost::optional<boost::property_tree::ptree&> pt_config = pt.get_child_optional("ufx_config");
	if (pt_config)
	{

		auto operator_no = pt_config->get_optional<std::string>("operator_no");
		if (!operator_no)
		{
			return { -1,"Load config data error,can not find operator_no node" };
		}
		boost::trim(*operator_no);
		config_data_.operator_no = *operator_no;

		auto password = pt_config->get_optional<std::string>("password");
		if (!password)
		{
			return { -1,"Load config data error,can not find password node" };
		}
		boost::trim(*password);
		config_data_.password = *password;


		auto authorization_id = pt_config->get_optional<std::string>("authorization_id");
		if (!authorization_id)
		{
			return { -1,"Load config data error,can not find authorization_id node" };
		}
		boost::trim(*authorization_id);
		config_data_.authorization_id = *authorization_id;

		auto account_code = pt_config->get_optional<std::string>("account_code");
		if (!account_code)
		{
			return { -1,"Load config data error,can not find account_code node" };
		}
		boost::trim(*account_code);
		config_data_.account_code = *account_code;

		auto asset_no = pt_config->get_optional<std::string>("asset_no");
		if (!asset_no)
		{
			return { -1,"Load config data error,can not find asset_no node" };
		}
		boost::trim(*asset_no);
		config_data_.asset_no = *asset_no;

		auto combi_no = pt_config->get_optional<std::string>("combi_no");
		if (!combi_no)
		{
			return { -1,"Load config data error,can not find combi_no node" };
		}
		boost::trim(*combi_no);
		config_data_.combi_no = *combi_no;

		auto cust_id = pt_config->get_optional<std::string>("cust_id");
		if (!cust_id)
		{
			return { -1,"Load config data error,can not find cust_id node" };
		}
		boost::trim(*cust_id);
		config_data_.cust_id = *cust_id;

		auto pbuid = pt_config->get_optional<std::string>("pbuid");
		if (!pbuid)
		{
			return { -1,"Load config data error,can not find pbuid node" };
		}
		boost::trim(*pbuid);
		config_data_.pbuid = *pbuid;

		auto servers = pt_config->get_optional<std::string>("t2sdk.servers");
		if (!servers)
		{
			return { -1,"Load config data error,can not find servers node" };
		}
		boost::trim(*servers);
		config_data_.servers = *servers;

		auto license_file = pt_config->get_optional<std::string>("t2sdk.license_file");
		if (!license_file)
		{
			return { -1,"Load config data error,can not find license_file node" };
		}
		boost::trim(*license_file);
		config_data_.license_file = *license_file;

		auto cert_file = pt_config->get_optional<std::string>("safe.cert_file");
		if (!cert_file)
		{
			return { -1,"Load config data error,can not find cert_file node" };
		}
		boost::trim(*cert_file);
		config_data_.cert_file = *cert_file;

		auto cert_pwd = pt_config->get_optional<std::string>("safe.cert_pwd");
		if (!cert_pwd)
		{
			return { -1,"Load config data error,can not find cert_pwd node" };
		}
		boost::trim(*cert_pwd);
		config_data_.cert_pwd = *cert_pwd;

		auto safe_level = pt_config->get_optional<std::string>("safe.safe_level");
		if (!safe_level)
		{
			return { -1,"Load config data error,can not find safe_level node" };
		}
		boost::trim(*safe_level);
		config_data_.safe_level = *safe_level;

		auto client_name = pt_config->get_optional<std::string>("mc.client_name");
		if (!client_name)
		{
			return { -1,"Load config data error,can not find client_name node" };
		}
		boost::trim(*client_name);
		config_data_.client_name = *client_name;


		auto biz_name = pt_config->get_optional<std::string>("subcribe.biz_name");
		if (!biz_name)
		{
			return { -1,"Load config data error,can not find biz_name node" };
		}
		boost::trim(*biz_name);
		config_data_.biz_name = *biz_name;

		auto topic_name = pt_config->get_optional<std::string>("subcribe.topic_name");
		if (!topic_name)
		{
			return { -1,"Load config data error,can not find topic_name node" };
		}
		boost::trim(*topic_name);
		config_data_.topic_name = *topic_name;

		auto is_rebulid = pt_config->get_optional<bool>("subcribe.is_rebulid");
		if (!is_rebulid)
		{
			return { -1,"Load config data error,can not find is_rebulid node" };
		}

		config_data_.is_rebulid = *is_rebulid;


		auto is_replace = pt_config->get_optional<bool>("subcribe.is_replace");
		if (!is_replace)
		{
			return { -1,"Load config data error,can not find is_replace node" };
		}
		config_data_.is_replace = *is_replace;


		auto send_interval = pt_config->get_optional<int32_t>("subcribe.send_interval");
		if (!send_interval)
		{
			return { -1,"Load config data error,can not find send_interval node" };
		}
		config_data_.send_interval = *send_interval;


		auto filter_list = pt_config->get_child_optional("subcribe.filter_list");
		if (filter_list)
		{
			for (auto& pt_fillter_pair : *filter_list)
			{
				auto name = pt_fillter_pair.second.get_optional<std::string>("name");
				auto value = pt_fillter_pair.second.get_optional<std::string>("value");
				if (name&&value)
				{
					config_data_.filter_map.insert(std::make_pair(boost::trim_copy(*name), boost::trim_copy(*value)));
				}
			}
		}


	}
	else
	{
		return { -1,"Load config data error,cannot get ufx_config node" };
	}


	/*
	<ufx_config>
	<ip_list>
	<ip enable="0">
	<address>127.0.0.1</address>
	<port>8080</port>
	<info name="test1">info1</info>
	</ip>
	<ip enable="1">
	<address>127.0.0.1</address>
	<port>8080</port>
	<info name="test2">info2</info>
	</ip>
	</ip_list>
	</ufx_config>
	*/

	//boost::optional<boost::property_tree::ptree&> pt_ip_list = pt.get_child_optional("ufx_config.ip_list");
	//if (pt_ip_list)
	//{
	//	for (auto& pt_ip_pair : *pt_ip_list)
	//	{
	//		auto enable_attr = pt_ip_pair.second.get_optional<int32_t>("<xmlattr>.enable");
	//		auto address = pt_ip_pair.second.get_optional<std::string>("address");
	//		auto port = pt_ip_pair.second.get_optional<uint16_t>("port");
	//		auto info_name_attr = pt_ip_pair.second.get_optional<std::string>("info.<xmlattr>.name");
	//		auto t1 = enable_attr.value_or(-1);
	//		auto t2 = info_name_attr.value_or("");
	//		assert(enable_attr);
	//		assert(address);
	//		assert(port);
	//		assert(info_name_attr);
	//	}
	//}


	return {};
}
