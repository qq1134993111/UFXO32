#pragma once
#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <atomic>
#include <iostream>

#include "boost/variant.hpp"
#include "boost/thread.hpp"
#include "boost/algorithm/string.hpp"

#include "ufx_config.h"
#include "general_processor.h"
#include <t2sdk_interface.h>

#include "ufx_resources.h"
#include "ufx_cursor.h"

enum class UfxFunctionNo :int32_t
{
	kHeartbeat = 10000,//心跳
	kLogin = 10001,//登录
	kEntrust = 91001,//普通买卖委托
	kEntrustWithdraw = 91101,//委托撤单
	kUnitstkQry = 31001,//证券持仓查询
	kEntrustQry = 32001,//证券委托查询
	kRealdealQry = 33001,//证券成交查询
	kCombofundQry = 34001,//账户资金查询
	kUnkown = -1
};

#define UfxFunctionNoStr(func_no) (#func_no)


class UfxConnecter
{
public:
	UfxConnecter() :ufx_processor(3)
	{
	}
	UfxConnecter(const UfxConfig::UfxConfigData& data) :ufx_processor(3), config_data_(data)
	{
	}
	virtual ~UfxConnecter()
	{
		conn_status_ = Status::kExit;
		ufx_processor.Stop();
	}

	void SetConfigData(const UfxConfig::UfxConfigData& data)
	{
		config_data_ = data;
	}

	const UfxConfig::UfxConfigData& GetConfigData()
	{
		return config_data_;
	}

	ErrorCode Connect();

	ErrorCode Login();

	std::string GetUserToken()
	{
		std::unique_lock<decltype(mtx_)> lc(mtx_);
		return user_token_;
	}

	bool IsConnect()
	{
		std::unique_lock<decltype(mtx_)> lc(mtx_);
		return sp_conn_ != nullptr;
	}

	bool IsLogin()
	{
		std::unique_lock<decltype(mtx_)> lc(mtx_);
		return (sp_conn_ != nullptr && !user_token_.empty());
	}

	void ResetConnection()
	{
		std::unique_lock<decltype(mtx_)> lc(mtx_);
		if (sp_conn_)
		{
			sp_conn_.reset();
		}
		user_token_.clear();

	}
	GeneralProcessor& GetUfxProcessor()
	{
		return ufx_processor;
	}
	void StopProcessor()
	{
		ufx_processor.Stop(false);
	}

	void AutoSendHeartbeat();

	enum class Status :int8_t
	{
		kInit = 0,
		kConnected = 1,
		kLogined = 2,
		kDisConnect = 3,
		kExit = 4
	};
	Status GetStatus()
	{
		std::unique_lock<decltype(mtx_)> lc(mtx_);
		return conn_status_;
	}

	void SetStatus(Status s)
	{
		std::unique_lock<decltype(mtx_)> lc(mtx_);
		conn_status_ = s;
	}

	std::shared_ptr<IF2Packer> MakePacker(const std::unordered_map<std::string, UfxFiledVarType>& map_data, std::basic_ostream<char>* p_ostream = nullptr);
	ErrorCode CallService(UfxFunctionNo function_no, std::shared_ptr<IF2Packer> request_packer, std::shared_ptr<IF2UnPacker>& response_unpacker);

	void UnPackerDump(IF2UnPacker* unpacker, std::basic_ostream<char>& ostream);
	std::string UnPackerToString(IF2UnPacker* unpacker);


protected:
	//template<typename T>
	//T GetCursorValue(IF2UnPacker* unpacker, const std::string& col_name)
	//{
	//	static_assert(false, "No matched GetCursorValue");
	//}

	//template<typename T>
	//T GetCursorValue(std::shared_ptr<IF2UnPacker> unpacker, const std::string& col_name)
	//{
	//	return GetCursorValue<T>(unpacker.get(), col_name);
	//}

	//template<>
	//int GetCursorValue(IF2UnPacker* unpacker, const std::string& col_name)
	//{
	//	//auto index = unpacker->FindColIndex(col_name.c_str());
	//	//auto type = unpacker->GetColType(index);
	//	//if (type != 'I')
	//	//{
	//	//	std::cout << "GetCursorValue No matched type," << col_name << ":" << type << "!=I" << std::endl;
	//	//}
	//	return unpacker->GetInt(col_name.c_str());
	//}

	//template<>
	//char GetCursorValue(IF2UnPacker* unpacker, const std::string& col_name)
	//{
	//	//auto index = unpacker->FindColIndex(col_name.c_str());
	//	//auto type = unpacker->GetColType(index);
	//	//if (type != 'C')
	//	//{
	//	//	std::cout << "GetCursorValue No matched type," << col_name << ":" << type << "!=C" << std::endl;
	//	//}
	//	return unpacker->GetChar(col_name.c_str());
	//}

	//template<>
	//double GetCursorValue(IF2UnPacker* unpacker, const std::string& col_name)
	//{
	//	//auto index = unpacker->FindColIndex(col_name.c_str());
	//	//auto type = unpacker->GetColType(index);
	//	//if (type != 'F')
	//	//{
	//	//	std::cout << "GetCursorValue No matched type," << col_name << ":" << type << "!=F" << std::endl;
	//	//}
	//	return unpacker->GetDouble(col_name.c_str());
	//}

	//template<>
	//std::string GetCursorValue(IF2UnPacker* unpacker, const std::string& col_name)
	//{
	//	//auto index = unpacker->FindColIndex(col_name.c_str());
	//	//auto type = unpacker->GetColType(index);
	//	//if (type != 'S'&&type != 's')
	//	//{
	//	//	std::cout << "GetCursorValue No matched type," << col_name << ":" << type << "!=S" << std::endl;
	//	//}

	//	auto value = unpacker->GetStr(col_name.c_str());
	//	if (value == nullptr)
	//	{
	//		return "";
	//	}

	//	return boost::algorithm::trim_copy(std::string(value));
	//}

protected:

	ErrorCode GetErrorInfo(IF2UnPacker* response_unpacker)
	{
		response_unpacker->SetCurrentDataset(0);
		return { response_unpacker->GetInt("ErrorCode"),response_unpacker->GetStr("ErrorMsg") };
	}

	ErrorCode ParseUserToken(IF2UnPacker* response_unpacker)
	{
		if (response_unpacker->GetDatasetCount() > 1)
		{
			response_unpacker->SetCurrentDatasetByIndex(1);
		}

		if (response_unpacker->FindColIndex("user_token") >= 0)
		{
			std::unique_lock<decltype(mtx_)> lc(mtx_);
			user_token_ = response_unpacker->GetStr("user_token");
		}
		else
		{
			return { -100,"The user_token field is not in the reply package" };
		}
		return {};
	}

	bool HeartbeatProc();
protected:
	GeneralProcessor ufx_processor;
	int32_t timeout_ms_ = 3000;
	UfxConfig::UfxConfigData config_data_;
	std::shared_ptr<CConnectionInterface> sp_conn_;
	std::string user_token_;

	std::recursive_mutex mtx_;
	std::atomic<Status> conn_status_ = Status::kInit;//0初始值 1已经连接 2已经登录 3连接断开

};

