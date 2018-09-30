#pragma once
#include <fstream>
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include <boost/exception/all.hpp>

class UfxCustomField
{
public:
	UfxCustomField()
	{

	}

	~UfxCustomField()
	{

	}

	template<typename T>
	bool SetValue(const std::string& key, const T& value)
	{
		try
		{
			pt_root.put<T>(key, value);
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	template<typename T>
	T GetValue(const std::string& key, T default_value = T())
	{
		try
		{
			return pt_root.get<T>(key);
		}
		catch (...)
		{
			return default_value;
		}


	}

	bool FromString(const std::string& data)
	{
		try
		{
			std::stringstream ss(data);
			boost::property_tree::read_json<boost::property_tree::ptree>(ss, pt_root);
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	std::string ToString()
	{
		try
		{
			std::stringstream ss;
			boost::property_tree::write_json(ss, pt_root, false);
			return ss.str();
		}
		catch (...)
		{
			return "";
		}
	}

private:
	boost::property_tree::ptree pt_root;

};
