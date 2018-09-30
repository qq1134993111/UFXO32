#include "ufx_connecter.h"

#include "boost/assign.hpp"
#include "boost/format.hpp"
#include "write_file.h"

ErrorCode UfxConnecter::Connect()
{
	std::unique_lock<decltype(mtx_)> lc(mtx_);

	ErrorCode ec;

	if (IsConnect())
	{
		//return { -1,"ufx has been connected" };
		return ec;
	}


	//创建T2SDK配置对象，并设置UFX服务器地址以及License文件
	CConfigInterface * config = NewConfig();
	config->AddRef();
	auto sp_config = UfxResourcesToShared(config);

	config->SetString("t2sdk", "lang", "1033");
	config->SetString("t2sdk", "servers", config_data_.servers.c_str());
	config->SetString("t2sdk", "license_file", config_data_.license_file.c_str());

	config->SetString("safe", "cert_file", config_data_.cert_file.c_str());
	config->SetString("safe", "cert_pwd", config_data_.cert_pwd.c_str());
	config->SetString("safe", "safe_level", config_data_.safe_level.c_str());

	//创建连接对象，并连接UFX服务器
	CConnectionInterface* conn = NewConnection(config);
	conn->AddRef();
	auto sp_conn = UfxResourcesToShared(conn);

	int ret = conn->Create(nullptr);
	if (ret != 0)
	{
		auto info = boost::format("CConnectionInterface::Create failed,%1%,%2%") % ret%conn->GetErrorMsg(ret);
		ec.assign(ret, info.str());
		return  ec;
	}

	//连接UFX服务器，参数3000为超时参数，单位毫秒
	ret = conn->Connect(timeout_ms_);
	if (ret != 0)
	{
		auto info = boost::format("CConnectionInterface::Connect failed,%1%,%2%") % ret%conn->GetErrorMsg(ret);
		ec.assign(ret, info.str());
		return ec;
	}

	sp_conn_ = sp_conn;

	conn_status_ = Status::kConnected;

	ec = { 0,"UfxConnecter::Connect successful" };
	return ec;
}

ErrorCode UfxConnecter::Login()
{
	std::unique_lock<decltype(mtx_)> lc(mtx_);

	ErrorCode ec;
	if (IsLogin())
	{
		//return { -1,"ufx has been login" };
		return ec;
	}


	//std::unordered_map<std::string, UfxFiledVarType>
	//	map_data = boost::assign::map_list_of("operator_no", config_data_.operator_no)
	//	("password", config_data_.password)
	//	("mac_address", sp_conn_->GetSelfMac())
	//	("op_station", sp_conn_->GetSelfMac())
	//	("ip_address", sp_conn_->GetSelfAddress())
	//	("authorization_id", config_data_.authorization_id);

	std::unordered_map<std::string, UfxFiledVarType>
		map_data = boost::assign::map_list_of("operator_no", config_data_.operator_no)
		("password", config_data_.password)
		("mac_address", "000000000000")
		("op_station", config_data_.operator_no)
		("ip_address", "0.0.0.0")
		("authorization_id", config_data_.authorization_id);

	std::stringstream ss;
	ss << "MakePacker->" << (int32_t)UfxFunctionNo::kLogin << "[" << UfxFunctionNoStr(UfxFunctionNo::kLogin) << "]\n";
	auto packer = MakePacker(map_data, &ss);
	std::cout << ss.str();

	std::shared_ptr<IF2UnPacker> response_unpacker;
	ec = CallService(UfxFunctionNo::kLogin, packer, response_unpacker);

	if (!ec)
	{
		WriteHexFile("LoginRep", (char*)response_unpacker->GetPackBuf(), response_unpacker->GetPackLen());
		UnPackerDump(response_unpacker.get(), std::cout);

		if ((ec = ParseUserToken(response_unpacker.get())) == 0)
		{
			conn_status_ = Status::kLogined;
		}
		else
		{
			ResetConnection();
			SetStatus(Status::kDisConnect);

			return ec;
		}
	}

	auto p = sp_conn_->GetSelfAddress();
	auto p2 = sp_conn_->GetSelfMac();


	return ec;
}

ErrorCode UfxConnecter::CallService(UfxFunctionNo function_no, std::shared_ptr<IF2Packer> request_packer, std::shared_ptr<IF2UnPacker>& response_unpacker)
{

	if (!IsConnect())
	{
		return { -1,"Ufx no connection" };
	}

	std::unique_lock<decltype(mtx_)> lc(mtx_);

	ErrorCode ec;
	int h_send = sp_conn_->SendBiz((int32_t)function_no, request_packer.get());
	if (h_send > 0)
	{
		IBizMessage* p_biz_msg_recv = nullptr;
		auto ret = sp_conn_->RecvBizMsg(h_send, &p_biz_msg_recv, timeout_ms_);
		if (ret != 0)
		{
			auto info = boost::format("RecvBizMsg error,%1%,%2%") % ret%sp_conn_->GetErrorMsg(ret);
			ec.assign(-1,info.str());
			return ec;
		}

		if (p_biz_msg_recv->GetReturnCode() != 0)
		{
			auto info = boost::format("IBizMessage error,%1%,%2%") % p_biz_msg_recv->GetErrorNo() % p_biz_msg_recv->GetErrorInfo();
			ec.assign(-1, info.str());
			return ec;
		}

		int i_len = 0;
		const void* p_buffer = p_biz_msg_recv->GetContent(i_len);
		IF2UnPacker* p_unpack = NewUnPacker((void*)p_buffer, i_len);
		p_unpack->AddRef();

		response_unpacker = UfxResourcesToShared(p_unpack);

		ec = GetErrorInfo(response_unpacker.get());
		if (response_unpacker->GetDatasetCount() > 1)
			response_unpacker->SetCurrentDatasetByIndex(1);

	}
	else
	{
		ec.assign(h_send, sp_conn_->GetErrorMsg(h_send));
	}
	return ec;
}

void UfxConnecter::UnPackerDump(IF2UnPacker* unpacker, std::basic_ostream<char>& ostream)
{
	if (unpacker == nullptr)
		return;

	int dataset_count = unpacker->GetDatasetCount();
	ostream << "\n" << "DatasetCount:" << dataset_count;
	for (int i = 0; i < dataset_count; ++i)
	{
		// 设置当前结果集
		unpacker->SetCurrentDatasetByIndex(i);

		ostream << "\n";

		auto row_count = (int)unpacker->GetRowCount();
		auto col_count = unpacker->GetColCount();

		ostream << boost::format("Dataset:%d[%s],RowCount:%d,ColCount:%d\n") % i%unpacker->GetDatasetName() % row_count%col_count;

		// 打印字段
		for (int t = 0; t < col_count; ++t)
		{
			if (strcmp("third_reff", unpacker->GetColName(t)) != 0)
			{
				ostream << boost::format("%-25s") % unpacker->GetColName(t);
			}
			else
			{
				ostream << boost::format("%-256s") % unpacker->GetColName(t);
			}

		}


		ostream << "\n";

		// 打印所有记录
		for (int j = 0; j < row_count; ++j)
		{
			// 打印每条记录
			for (int k = 0; k < col_count; ++k)
			{
				switch (unpacker->GetColType(k))
				{
				case 'I':
					ostream << boost::format("%-25d") % unpacker->GetIntByIndex(k);
					break;

				case 'C':
					ostream << boost::format("%-25c") % unpacker->GetCharByIndex(k);
					break;
				case 's':
				case 'S':
					if (strcmp("third_reff", unpacker->GetColName(k)) != 0)
					{
						ostream << boost::format("%-25s") % unpacker->GetStrByIndex(k);
					}
					else
					{
						ostream << boost::format("%-256s") % unpacker->GetStrByIndex(k);
					}

					break;

				case 'F':
					ostream << boost::format("%-25f") % unpacker->GetDoubleByIndex(k);
					break;

				case 'R':
				{
					int length = 0;
					void *p_data = unpacker->GetRawByIndex(k, &length);

					// 对2进制数据进行处理

					ostream << std::string((char*)p_data, length);

					break;
				}

				default:
					// 未知数据类型
					ostream << "unknown_data," << "col_index:" << k << "[" << unpacker->GetColType(k) << "]";
					break;
				}
			}

			ostream << "\n";

			unpacker->Next();
		}

		ostream << "\n";
	}

	unpacker->SetCurrentDatasetByIndex(0);

}

std::string UfxConnecter::UnPackerToString(IF2UnPacker* unpacker)
{
	std::stringstream ss;
	UnPackerDump(unpacker, ss);
	return ss.str();
}


void UfxConnecter::AutoSendHeartbeat()
{
	GetUfxProcessor().AddTimerWithoutStrand(std::chrono::microseconds(1000 * 1000 * 60 * 2), &UfxConnecter::HeartbeatProc, this);
}

std::shared_ptr<IF2Packer> UfxConnecter::MakePacker(const std::unordered_map<std::string, UfxFiledVarType>& map_data, std::basic_ostream<char>* p_ostream)
{
	IF2Packer* request_packer = NewPacker(2);
	request_packer->AddRef();
	std::shared_ptr<IF2Packer> sp_packer(request_packer, UfxResourcesDeleter<IF2Packer>());

	request_packer->BeginPack();
	for (auto& kv_data : map_data)
	{
		boost::apply_visitor(UfxFiledVarTypeVisitor(request_packer, kv_data.first, UfxFiledVarTypeVisitor::VisitorFalg::kAddFiled, p_ostream), kv_data.second);
	}

	for (auto& kv_data : map_data)
	{
		boost::apply_visitor(UfxFiledVarTypeVisitor(request_packer, kv_data.first, UfxFiledVarTypeVisitor::VisitorFalg::kAddValue, p_ostream), kv_data.second);
	}

	request_packer->EndPack();

	return sp_packer;
}

bool UfxConnecter::HeartbeatProc()
{

	auto func_make_heartbeat_packer = [this]() -> std::shared_ptr<IF2Packer>
	{
		std::stringstream ss;
		ss << "MakePacker->" << (int32_t)UfxFunctionNo::kHeartbeat << "[" << UfxFunctionNoStr(UfxFunctionNo::kHeartbeat) << "]\n";
		auto req_packer = MakePacker({ { "user_token",GetUserToken() } }, &ss);
		std::cout << ss.str();
		return req_packer;
	};

	ErrorCode ec;
	if (IsLogin())
	{
		std::shared_ptr<IF2UnPacker> response_unpacker;
		ec = CallService(UfxFunctionNo::kHeartbeat, func_make_heartbeat_packer(), response_unpacker);
		UnPackerDump(response_unpacker.get(), std::cout);
		//UnPackerToString(response_unpacker.get());
		if (ec)
		{
			ResetConnection();
			SetStatus(Status::kDisConnect);
		}
	}

	return true;

}
