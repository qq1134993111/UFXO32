#include "ufx_subcribe.h"

#include "boost/format.hpp"

UfxSubcribe::UfxSubcribe(const UfxConfig::UfxConfigData& data) :UfxConnecter(data)
{

}


UfxSubcribe::~UfxSubcribe()
{
	SetStatus(Status::kExit);
}

void FUNCTION_CALL_MODE UfxSubcribe::OnReceived(CSubscribeInterface *p_sub, int subscribe_index, const void *p_data, int length, LPSUBSCRIBE_RECVDATA recv_data)
{
	std::stringstream ss;
	if (p_data == nullptr)
	{
		ss << "CSubCallback::OnReceived p_data is nullptr" << std::endl;
		std::cout << ss.str();
		return;
	}

	auto sp_unpacker_data = std::make_shared<UfxUnpackerData>(std::string((char*)p_data, length));
	auto unpack = sp_unpacker_data->GetUnPacker();
	const char* p_msgtype = unpack->GetStr("msgtype");

	ss << "CSubCallback::OnReceived: SubscribeIndex(" << subscribe_index << "), MsgType(" << (p_msgtype ? p_msgtype : "") << "), DataLength(" << length << ")." << std::endl;

	std::cout << ss.str();

	ss.clear();

	if (recv_data == nullptr)
	{
		ss << "CSubCallback::OnReceived recv_data is nullptr" << std::endl;
		std::cout << ss.str();
	}
	else
	{
		ss << SubscribeToString(subscribe_index, recv_data);
		std::cout << ss.str();
	}

	GetUfxProcessor().Post(&UfxSubcribe::SubscribeHandle, this, sp_unpacker_data);
	//SubscribeHandle(sp_unpack);
}

void FUNCTION_CALL_MODE UfxSubcribe::OnRecvTickMsg(CSubscribeInterface *lpSub, int subscribeIndex, const char* TickMsgInfo)
{

}

ErrorCode UfxSubcribe::Subcribe()
{
	std::unique_lock<decltype(mtx_)> lc(mtx_);

	ErrorCode ec;

	if (IsSubcribe())
		return {};

	CConfigInterface * config = NewConfig();
	config->AddRef();
	auto sp_config = UfxResourcesToShared(config);

	config->SetString("t2sdk", "lang", "1033");
	config->SetString("t2sdk", "servers", config_data_.servers.c_str());
	config->SetString("t2sdk", "license_file", config_data_.license_file.c_str());

	config->SetString("safe", "cert_file", config_data_.cert_file.c_str());
	config->SetString("safe", "cert_pwd", config_data_.cert_pwd.c_str());
	config->SetString("safe", "safe_level", config_data_.safe_level.c_str());

	config->SetString("mc", "client_name", config_data_.client_name.c_str());

	CConnectionInterface* sub_connection = NewConnection(config);
	if (sub_connection == nullptr)
	{
		ec = { -1,"NewConnection is nullptr" };
		return ec;
	}

	sub_connection->AddRef();
	auto sp_sub_connection = UfxResourcesToShared(sub_connection);

	std::shared_ptr<CSubscribeInterface> sp_subscribe_interface;

	int ret = 0;
	if ((ret = sp_sub_connection->Create2BizMsg(this)) == 0)
	{
		//正式开始连接，参数1000为超时参数，单位是ms
		if (sp_sub_connection->Connect(timeout_ms_) == 0)
		{
			CSubscribeInterface *p_subscribe_interface = sp_sub_connection->NewSubscriber(this, (char*)config_data_.biz_name.c_str(), timeout_ms_);
			if (p_subscribe_interface == nullptr)
			{
				auto info = boost::format("NewSubscriber is nullptr,%1%") % sp_sub_connection->GetMCLastError();
				ec = ErrorCode{ -1, info.str() };
				return ec;
			}

			p_subscribe_interface->AddRef();
			sp_subscribe_interface = UfxResourcesToShared(p_subscribe_interface);

			//订阅参数获取
			CSubscribeParamInterface*  p_subscribe_param = NewSubscribeParam();
			if (p_subscribe_param == nullptr)
			{
				ec = { -1,"NewSubscribeParam is null" };
				return ec;
			}

			p_subscribe_param->AddRef();
			auto sp_sub_param = UfxResourcesToShared(p_subscribe_param);

			sp_sub_param->SetTopicName((char*)config_data_.topic_name.c_str());
			sp_sub_param->SetFromNow(config_data_.is_rebulid);
			sp_sub_param->SetReplace(config_data_.is_replace);

			std::string app_data = "huarui";
			sp_sub_param->SetAppData((void*)app_data.c_str(), app_data.size());//添加附加数据

			//添加过滤字段
			for (auto& filrer_pair : config_data_.filter_map)
			{
				sp_sub_param->SetFilter((char*)filrer_pair.first.c_str(), (char*)filrer_pair.second.c_str());
			}

			//添加发送频率
			sp_sub_param->SetSendInterval(config_data_.send_interval);

			std::stringstream ss;
			std::unordered_map<std::string, UfxFiledVarType> data_map =
			{
			{ "login_operator_no",config_data_.operator_no },
			{ "password",config_data_.password }
			};
			auto sp_packer = MakePacker(data_map, &ss);

			//printf("开始订阅\n");

			IF2UnPacker* p_back = nullptr;
			int  i_ret = sp_subscribe_interface->SubscribeTopic(sp_sub_param.get(), timeout_ms_ * 3, &p_back, sp_packer.get());
			if (i_ret > 0)
			{
				auto subscribeIndex = i_ret;
				//printf("SubscribeTopic successful:[%d] \n", i_ret);

				all_subscribe_param_map_[subscribeIndex] = sp_sub_param;//保存到map中，用于以后的取消订阅

				auto info = boost::format("SubscribeTopic successful:[%1%]") % i_ret;
				ec = ErrorCode{ 0,info.str() };
			}
			else
			{
				if (p_back != nullptr)
				{
					auto str_info = UnPackerToString(p_back);
					p_back->Release();
				}

				auto info = boost::format("SubscribeTopic failed:[%1%],%2%") % i_ret%sub_connection->GetErrorMsg(i_ret);
				ec = ErrorCode{ i_ret,info.str() };
				return ec;
			}
		}
		else
		{
			return { sp_sub_connection->GetConnectError(),
				sp_sub_connection->GetErrorMsg(sp_sub_connection->GetConnectError()) };

		}
	}
	else
	{
		return { ret,sp_sub_connection->GetErrorMsg(ret) };
	}

	sp_sub_conn_ = sp_sub_connection;
	sp_sub_interface_ = sp_subscribe_interface;

	return ec;
}

void UfxSubcribe::DumpSubscribe(int sub_index, LPSUBSCRIBE_RECVDATA p_recv_data, std::basic_ostream<char>& ostream)
{
	auto it = all_subscribe_param_map_.find(sub_index);
	if (it == all_subscribe_param_map_.end())
	{
		//printf("没有这个订阅项\n");
		ostream << "There is no such subscription:" << sub_index;
		return;
	}

	CSubscribeParamInterface* p_sub_param = (*it).second.get();
	ostream << "---------------------------------------------------------------------------------\n";
	ostream << "TopicName:" << p_sub_param->GetTopicName() << "\n";
	ostream << "AppData:" << p_recv_data->lpAppData << "\n";
	ostream << "FilterData:\n";
	if (p_recv_data->iFilterDataLen > 0)
	{
		IF2UnPacker* p_unpack = NewUnPacker(p_recv_data->lpFilterData, p_recv_data->iFilterDataLen);
		p_unpack->AddRef();
		UnPackerDump(p_unpack, ostream);
		p_unpack->Release();
	}
	ostream << "----------------------------------------------------------------------------------\n";
}

std::string UfxSubcribe::SubscribeToString(int sub_index, LPSUBSCRIBE_RECVDATA p_recv_data)
{
	std::stringstream ss;
	DumpSubscribe(sub_index, p_recv_data, ss);
	return ss.str();
}
