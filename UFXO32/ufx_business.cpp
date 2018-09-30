#include "ufx_business.h"
#include <utility>

#include "boost/assign.hpp"
#include "write_file.h"
#include "ufx_custom_field.h"

UfxBusiness::UfxBusiness(const UfxConfig::UfxConfigData& data) :UfxSubcribe(data)
{

}

void UfxBusiness::AutoCheckConnect()
{

	GetUfxProcessor().AddTimerWithoutStrand(std::chrono::microseconds(1000 * 1000 * 10), [this]()->bool
	{
		ErrorCode ec;
		if (!IsConnect())
		{
			do
			{
				ec = Connect();
				if (ec)
				{
					std::cout << "Connect error:" << ec.message() << std::endl;
					break;
				}


			} while (0);

		}

		if (IsConnect() && IsLogin())
		{
			do
			{
				ec = Login();
				if (ec)
				{
					std::cout << "Login error:" << ec.message() << std::endl;
					break;
				}

			} while (0);

		}

		if (!IsSubcribe())
		{
			do
			{
				ec = Subcribe();
				if (ec)
				{
					std::cout << "Subcribe error:" << ec.message() << std::endl;
					break;
				}
			} while (0);

		}

		return true;
	});

}

ErrorCode UfxBusiness::Entrust()
{
	using namespace boost::assign;

	std::unordered_map<std::string, UfxFiledVarType> map_data;
	boost::assign::insert(map_data)
		("user_token", GetUserToken())//令牌号
		("account_code", GetConfigData().account_code)//账户编号
		("asset_no", GetConfigData().asset_no)//资产单元编号
		("combi_no", GetConfigData().combi_no)//组合编号
		//("stockholder_id", "")//股东代码
		//("report_seat", GetConfigData().pbuid)//申报席位
		("market_no", "1")//交易市场
		("stock_code", "510050")//证券代码
		("entrust_direction", "1")//委托方向
		("price_type", "a")//委托价格类型
		("entrust_price", double(2.4780))//委托价格
		("entrust_amount", 100)//委托数量
		("extsystem_id", 0)//第三方系统自定义号
		("third_reff", "")//第三方系统自定义说明
		("mac_address", "000000000000")
		("ip_address", "0.0.0.0")
		("hd_volserial", "00000000-0000-0000-0000-000000000000")
		("op_station", config_data_.operator_no)
		("terminal_info", "0;0;0;0;0;0");
	;

	UfxCustomField third_reff;
	third_reff.SetValue("user", "user1");
	third_reff.SetValue("account_id", "10001");
	third_reff.SetValue("fund_aacount_id", "hello");
	third_reff.SetValue("test", "test");
	third_reff.SetValue("client_id", 1234455);
	third_reff.SetValue("agw_seq_id", 1234567890);



	std::stringstream ss;
	ss << "MakePacker->" << (int32_t)UfxFunctionNo::kEntrust << "[" << UfxFunctionNoStr(UfxFunctionNo::kEntrust) << "]\n";
	auto packer = MakePacker(map_data, &ss);
	std::cout << ss.str();

	std::shared_ptr<IF2UnPacker> response_unpacker;
	auto ec = CallService(UfxFunctionNo::kEntrust, packer, response_unpacker);

	auto str = UnPackerToString(response_unpacker.get());
	std::cout << str;


	if (1000 == ec.value() || ec.value() <= 0)
	{
		return ec;
	}

	WriteHexFile("EntrustRep",(char*)response_unpacker->GetPackBuf(), response_unpacker->GetPackLen());

	if (response_unpacker->GetDatasetCount() > 1)
	{
		response_unpacker->SetCurrentDatasetByIndex(1);
	}

	for (int i = 0; i < (int)response_unpacker->GetRowCount(); ++i)
	{
		response_unpacker->Go(i + 1);

		//auto entrust_no =response_unpacker->GetInt("entrust_no");
		//auto extsystem_id=response_unpacker->GetInt("extsystem_id");
		//std::string entrust_fail_code = response_unpacker->GetStr("entrust_fail_code");

		auto entrust_no = GetCursorValue<int>(response_unpacker, "entrust_no");
		auto extsystem_id = GetCursorValue<int>(response_unpacker, "extsystem_id");
		auto entrust_fail_code = GetCursorValue<int>(response_unpacker, "entrust_fail_code");

	}


	return {};
}

ErrorCode UfxBusiness::EntrustWithdraw()
{

	std::unordered_map<std::string, UfxFiledVarType> map_data;
	boost::assign::insert(map_data)
		("user_token", GetUserToken())//令牌号
		("entrust_no", 24173)//委托序号
		("mac_address", "000000000000")
		("ip_address", "0.0.0.0")
		("hd_volserial", "00000000-0000-0000-0000-000000000000")
		("op_station", config_data_.operator_no)
		("terminal_info", "0;0;0;0;0;0");
	;

	std::stringstream ss;
	ss << "MakePacker->" << (int32_t)UfxFunctionNo::kEntrustWithdraw << "[" << UfxFunctionNoStr(UfxFunctionNo::kEntrustWithdraw) << "]\n";
	auto packer = MakePacker(map_data, &ss);
	std::cout << ss.str();

	std::shared_ptr<IF2UnPacker> response_unpacker;
	auto ec = CallService(UfxFunctionNo::kEntrustWithdraw, packer, response_unpacker);
	auto str = UnPackerToString(response_unpacker.get());
	std::cout << str;

	if (1000 == ec.value() || ec.value() <= 0)
	{
		return ec;
	}


	WriteHexFile("EntrustWithdrawRep", (char*)response_unpacker->GetPackBuf(), response_unpacker->GetPackLen());

	if (response_unpacker->GetDatasetCount() > 1)
	{
		response_unpacker->SetCurrentDatasetByIndex(1);
	}

	for (int i = 0; i < (int)response_unpacker->GetRowCount(); ++i)
	{
		response_unpacker->Go(i + 1);

		//auto entrust_no =response_unpacker->GetInt("entrust_no");
		//auto extsystem_id=response_unpacker->GetInt("extsystem_id");
		//std::string entrust_fail_code = response_unpacker->GetStr("entrust_fail_code");

		auto entrust_no = GetCursorValue<int>(response_unpacker, "entrust_no");//委托序号
		auto market_no = GetCursorValue<std::string>(response_unpacker, "market_no");//交易市场
		auto stock_code = GetCursorValue<std::string>(response_unpacker, "stock_code");//证券代码
		auto success_flag = GetCursorValue<std::string>(response_unpacker, "success_flag");//撤单成功标志
		auto fail_cause = GetCursorValue<std::string>(response_unpacker, "fail_cause");//失败原因

	}


	return {};
}

ErrorCode UfxBusiness::EntrustQry()
{

	using namespace boost::assign;

	std::unordered_map<std::string, UfxFiledVarType> map_data;
	boost::assign::insert(map_data)
		("user_token", GetUserToken())//令牌号
		("account_code", GetConfigData().account_code)//账户编号
		("asset_no", GetConfigData().asset_no)//资产单元编号
		("combi_no", GetConfigData().combi_no)//组合编号
		//("entrust_no", 0)//委托序号
		("stockholder_id", "")//股东代码
		("market_no", "")//交易市场
		("stock_code", "")//证券代码
		("entrust_direction", "")//委托方向
		("entrust_state_list", "")//委托状态
		("extsystem_id", 0)//第三方系统自定义号
		("third_reff", "")//第三方系统自定义说明
		("position_str", "21117")//定位串
		("request_num", 2)//请求数
		;

	std::stringstream ss;
	ss << "MakePacker->" << (int32_t)UfxFunctionNo::kEntrustQry << "[" << UfxFunctionNoStr(UfxFunctionNo::kEntrustQry) << "]\n";
	auto packer = MakePacker(map_data, &ss);
	std::cout << ss.str();

	std::shared_ptr<IF2UnPacker> response_unpacker;
	auto ec = CallService(UfxFunctionNo::kEntrustQry, packer, response_unpacker);
	auto str = UnPackerToString(response_unpacker.get());
	std::cout << str;

	if (1000 == ec || ec < 0)
	{
		return ec;
	}

	WriteHexFile("EntrustQryRep", (char*)response_unpacker->GetPackBuf(), response_unpacker->GetPackLen());


	if (response_unpacker->GetDatasetCount() > 1)
	{
		response_unpacker->SetCurrentDatasetByIndex(1);
	}

	for (int i = 0; i < (int)response_unpacker->GetRowCount(); ++i)
	{
		response_unpacker->Go(i + 1);

		auto entrust_date = GetCursorValue<int>(response_unpacker, "entrust_date");//委托日期
		auto entrust_time = GetCursorValue<int>(response_unpacker, "entrust_time");//委托时间
		auto operator_no = GetCursorValue<std::string>(response_unpacker, "operator_no");//操作员编号
		auto batch_no = GetCursorValue<int>(response_unpacker, "batch_no");//委托批号
		auto entrust_no = GetCursorValue<int>(response_unpacker, "entrust_no");//委托序号
		auto report_no = GetCursorValue<std::string>(response_unpacker, "report_no");//申报编号
		auto extsystem_id = GetCursorValue<int>(response_unpacker, "extsystem_id");//第三方系统自定义号
		auto third_reff = GetCursorValue<std::string>(response_unpacker, "third_reff");//第三方系统自定义说明
		auto account_code = GetCursorValue<std::string>(response_unpacker, "account_code");//账户编号
		auto asset_no = GetCursorValue<std::string>(response_unpacker, "asset_no");//资产单元编号
		auto combi_no = GetCursorValue<std::string>(response_unpacker, "combi_no");//组合编号
		auto stockholder_id = GetCursorValue<std::string>(response_unpacker, "stockholder_id");//股东代码
		auto report_seat = GetCursorValue<std::string>(response_unpacker, "report_seat");//申报席位
		auto market_no = GetCursorValue<std::string>(response_unpacker, "market_no");//交易市场
		auto stock_code = GetCursorValue<std::string>(response_unpacker, "stock_code");//证券代码
		auto entrust_direction = GetCursorValue<std::string>(response_unpacker, "entrust_direction"); //委托方向
		auto price_type = GetCursorValue<std::string>(response_unpacker, "price_type");//委托价格类型
		auto entrust_price = GetCursorValue<double>(response_unpacker, "entrust_price");//委托价格
		auto entrust_amount = GetCursorValue<double>(response_unpacker, "entrust_amount");//委托数量
		auto pre_buy_frozen_balance = GetCursorValue<double>(response_unpacker, "pre_buy_frozen_balance");//预买冻结金额
		auto pre_sell_balance = GetCursorValue<double>(response_unpacker, "pre_sell_balance");//预卖金额
		auto confirm_no = GetCursorValue<std::string>(response_unpacker, "confirm_no");//委托确认号
		auto entrust_state = GetCursorValue<std::string>(response_unpacker, "entrust_state");//委托状态
		auto first_deal_time = GetCursorValue<int>(response_unpacker, "first_deal_time");//首次成交时间
		auto deal_amount = GetCursorValue<double>(response_unpacker, "deal_amount");//成交数量
		auto deal_balance = GetCursorValue<double>(response_unpacker, "deal_balance");//成交金额
		auto deal_price = GetCursorValue<double>(response_unpacker, "deal_price");//成交均价
		auto deal_times = GetCursorValue<int>(response_unpacker, "deal_times");//分笔成交次数
		auto withdraw_amount = GetCursorValue<double>(response_unpacker, "withdraw_amount");//撤单数量
		auto withdraw_cause = GetCursorValue<std::string>(response_unpacker, "withdraw_cause");//撤单原因
		auto position_str = GetCursorValue<std::string>(response_unpacker, "position_str");//定位串
		auto exchange_report_no = GetCursorValue<std::string>(response_unpacker, "exchange_report_no");//分仓申报编号
		auto engaged_no = GetCursorValue<std::string>(response_unpacker, "engaged_no");//约定号




	}

	return ErrorCode();
}

ErrorCode UfxBusiness::RealdealQry()
{
	using namespace boost::assign;

	std::unordered_map<std::string, UfxFiledVarType> map_data;
	boost::assign::insert(map_data)
		("user_token", GetUserToken())//令牌号
		("account_code", GetConfigData().account_code)//账户编号
		("asset_no", GetConfigData().asset_no)//资产单元编号
		("combi_no", GetConfigData().combi_no)//组合编号
		("entrust_no", 0)//委托序号
		("deal_no", "")//成交编号
		("stockholder_id", "")//股东代码
		("market_no", "")//交易市场
		("stock_code", "")//证券代码
		("entrust_direction", "")//委托方向
		("extsystem_id", 0)//第三方系统自定义号
		("third_reff", "")//第三方系统自定义说明
		("position_str", "")//定位串
		("request_num", 0)//请求数
		;

	std::stringstream ss;
	ss << "MakePacker->" << (int32_t)UfxFunctionNo::kRealdealQry << "[" << UfxFunctionNoStr(UfxFunctionNo::kRealdealQry) << "]\n";
	auto packer = MakePacker(map_data, &ss);
	std::cout << ss.str();

	std::shared_ptr<IF2UnPacker> response_unpacker;
	auto ec = CallService(UfxFunctionNo::kRealdealQry, packer, response_unpacker);
	auto str = UnPackerToString(response_unpacker.get());
	std::cout << str;
	if (ec == 1000 || ec < 0)
	{
		return ec;
	}

	WriteHexFile("RealdealQryRep", (char*)response_unpacker->GetPackBuf(), response_unpacker->GetPackLen());


	if (response_unpacker->GetDatasetCount() > 1)
	{
		response_unpacker->SetCurrentDatasetByIndex(1);
	}

	for (int i = 0; i < (int)response_unpacker->GetRowCount(); ++i)
	{
		response_unpacker->Go(i + 1);


		//auto deal_date = GetCursorValue<int>(response_unpacker, "deal_date");//成交日期
		auto deal_date = GetCursorValue<std::string>(response_unpacker, "deal_date");//成交日期
		auto deal_no = GetCursorValue<std::string>(response_unpacker, "deal_no");//成交编号
		//auto entrust_no = GetCursorValue<int>(response_unpacker, "entrust_no");//委托序号
		auto entrust_no = GetCursorValue<std::string>(response_unpacker, "entrust_no");//委托序号
		//auto extsystem_id = GetCursorValue<int>(response_unpacker, "extsystem_id");//第三方系统自定义号
		auto extsystem_id = GetCursorValue<std::string>(response_unpacker, "extsystem_id");//第三方系统自定义号
		auto third_reff = GetCursorValue<std::string>(response_unpacker, "third_reff");//第三方系统自定义说明
		auto account_code = GetCursorValue<std::string>(response_unpacker, "account_code");//账户编号
		auto asset_no = GetCursorValue<std::string>(response_unpacker, "asset_no");//资产单元编号
		auto combi_no = GetCursorValue<std::string>(response_unpacker, "combi_no");//组合编号
		auto instance_no = GetCursorValue<std::string>(response_unpacker, "instance_no");//交易实例编号
		auto stockholder_id = GetCursorValue<std::string>(response_unpacker, "stockholder_id");//股东代码
		auto market_no = GetCursorValue<std::string>(response_unpacker, "market_no");//交易市场
		auto stock_code = GetCursorValue<std::string>(response_unpacker, "stock_code");//证券代码
		auto entrust_direction = GetCursorValue<std::string>(response_unpacker, "entrust_direction"); //委托方向
		//auto deal_amount = GetCursorValue<double>(response_unpacker, "deal_amount");//成交数量
		auto deal_amount = GetCursorValue<std::string>(response_unpacker, "deal_amount");//成交数量
		//auto deal_balance = GetCursorValue<double>(response_unpacker, "deal_balance");//成交金额
		auto deal_balance = GetCursorValue<std::string>(response_unpacker, "deal_balance");//成交金额
		//auto deal_price = GetCursorValue<double>(response_unpacker, "deal_price");//成交价格
		auto deal_price = GetCursorValue<std::string>(response_unpacker, "deal_price");//成交价格
		//auto total_fee = GetCursorValue<double>(response_unpacker, "total_fee");//总费用
		auto total_fee = GetCursorValue<std::string>(response_unpacker, "total_fee");//总费用
		//auto deal_time = GetCursorValue<int>(response_unpacker, "deal_time");//成交时间
		auto deal_time = GetCursorValue<std::string>(response_unpacker, "deal_time");//成交时间
		auto position_str = GetCursorValue<std::string>(response_unpacker, "position_str");//定位串



	}

	return ErrorCode();
}

ErrorCode UfxBusiness::UnitstkQry()
{
	using namespace boost::assign;

	std::unordered_map<std::string, UfxFiledVarType> map_data;
	boost::assign::insert(map_data)
		("user_token", GetUserToken())//令牌号
		("account_code", GetConfigData().account_code)//账户编号
		("asset_no", GetConfigData().asset_no)//资产单元编号
		("combi_no", GetConfigData().combi_no)//组合编号
		("market_no", "")//交易市场
		("stock_code", "")//证券代码
		("stockholder_id", "")//股东代码
		("hold_seat", GetConfigData().pbuid);//持仓席位
	("stock_type", "")//证券类型
		;

	std::stringstream ss;
	ss << "MakePacker->" << (int32_t)UfxFunctionNo::kUnitstkQry << "[" << UfxFunctionNoStr(UfxFunctionNo::kUnitstkQry) << "]\n";
	auto packer = MakePacker(map_data, &ss);
	std::cout << ss.str();

	std::shared_ptr<IF2UnPacker> response_unpacker;
	auto ec = CallService(UfxFunctionNo::kUnitstkQry, packer, response_unpacker);
	auto str = UnPackerToString(response_unpacker.get());
	std::cout << str;
	if (ec == 1000 || ec < 0)
	{
		return ec;
	}

	WriteHexFile("UnitstkQryRep", (char*)response_unpacker->GetPackBuf(), response_unpacker->GetPackLen());


	if (response_unpacker->GetDatasetCount() > 1)
	{
		response_unpacker->SetCurrentDatasetByIndex(1);
	}

	for (int i = 0; i < (int)response_unpacker->GetRowCount(); ++i)
	{
		response_unpacker->Go(i + 1);

		auto account_code = GetCursorValue<std::string>(response_unpacker, "account_code");//账户编号
		auto asset_no = GetCursorValue<std::string>(response_unpacker, "asset_no");//资产单元编号
		auto combi_no = GetCursorValue<std::string>(response_unpacker, "combi_no");//组合编号
		auto market_no = GetCursorValue<std::string>(response_unpacker, "market_no");//交易市场
		auto stock_code = GetCursorValue<std::string>(response_unpacker, "stock_code");//证券代码
		auto stockholder_id = GetCursorValue<std::string>(response_unpacker, "stockholder_id");//股东代码
		auto hold_seat = GetCursorValue<std::string>(response_unpacker, "hold_seat");//持仓席位

		auto invest_type = GetCursorValue<std::string>(response_unpacker, "invest_type");//投资类型
		auto current_amount = GetCursorValue<std::string>(response_unpacker, "current_amount");//当前数量
		auto enable_amount = GetCursorValue<std::string>(response_unpacker, "enable_amount");//可用数量
		auto begin_amount = GetCursorValue<std::string>(response_unpacker, "begin_amount");//期初数量
		auto redeemable_amount = GetCursorValue<std::string>(response_unpacker, "redeemable_amount");//可赎回数量
		auto begin_cost = GetCursorValue<std::string>(response_unpacker, "begin_cost");//期初成本
		auto current_cost = GetCursorValue<std::string>(response_unpacker, "current_cost");//当前成本
		auto pre_buy_amount = GetCursorValue<std::string>(response_unpacker, "pre_buy_amount");//买挂单数量
		auto pre_sell_amount = GetCursorValue<std::string>(response_unpacker, "pre_sell_amount");//卖挂单数量
		auto pre_buy_balance = GetCursorValue<std::string>(response_unpacker, "pre_buy_balance");//买挂单金额
		auto pre_sell_balance = GetCursorValue<std::string>(response_unpacker, "pre_sell_balance");//卖挂单金额
		auto today_buy_amount = GetCursorValue<std::string>(response_unpacker, "today_buy_amount");//当日买入数量
		auto today_sell_amount = GetCursorValue<std::string>(response_unpacker, "today_sell_amount");//当日卖出数量
		auto today_buy_balance = GetCursorValue<std::string>(response_unpacker, "today_buy_balance");//当日买入金额
		auto today_sell_balance = GetCursorValue<std::string>(response_unpacker, "today_sell_balance");//当日卖出金额
		auto today_buy_fee = GetCursorValue<std::string>(response_unpacker, "today_buy_fee");//当日买费用
		auto today_sell_fee = GetCursorValue<std::string>(response_unpacker, "today_sell_fee");//当日卖费用
		auto stock_type = GetCursorValue<std::string>(response_unpacker, "today_sell_fee");//证券类型



	}

	return ErrorCode();
}

ErrorCode UfxBusiness::CombofundQry()
{
	using namespace boost::assign;

	std::unordered_map<std::string, UfxFiledVarType> map_data;
	boost::assign::insert(map_data)
		("user_token", GetUserToken())//令牌号
		("account_code", GetConfigData().account_code)//账户编号
		("asset_no", GetConfigData().asset_no)//资产单元编号
		("combi_no", GetConfigData().combi_no)//组合编号
		;

	std::stringstream ss;
	ss << "MakePacker->" << (int32_t)UfxFunctionNo::kCombofundQry << "[" << UfxFunctionNoStr(UfxFunctionNo::kCombofundQry) << "]\n";
	auto packer = MakePacker(map_data, &ss);
	std::cout << ss.str();

	std::shared_ptr<IF2UnPacker> response_unpacker;
	auto ec = CallService(UfxFunctionNo::kCombofundQry, packer, response_unpacker);
	auto str = UnPackerToString(response_unpacker.get());
	std::cout << str;

	if (ec == 1000 || ec < 0)
	{
		return ec;
	}

	WriteHexFile("CombofundQryRep", (char*)response_unpacker->GetPackBuf(), response_unpacker->GetPackLen());

	if (response_unpacker->GetDatasetCount() > 1)
	{
		response_unpacker->SetCurrentDatasetByIndex(1);
	}

	for (int i = 0; i < (int)response_unpacker->GetRowCount(); ++i)
	{
		response_unpacker->Go(i + 1);
		//auto str_enable_balance_t0 = GetCursorValue<std::string>(response_unpacker, "enable_balance_t0");//T+0可用资金


		auto account_code = GetCursorValue<std::string>(response_unpacker, "account_code");//账户编号
		auto asset_no = GetCursorValue<std::string>(response_unpacker, "asset_no");//资产单元编号
		auto enable_balance_t0 = GetCursorValue<double>(response_unpacker, "enable_balance_t0");//T+0可用资金
		auto enable_balance_t1 = GetCursorValue<double>(response_unpacker, "enable_balance_t1");//T+1可用资金
		auto current_balance = GetCursorValue<double>(response_unpacker, "current_balance");//当前资金余额


	}

	return ErrorCode();
}

void UfxBusiness::SubscribeHandle(std::shared_ptr<UfxUnpackerData> unpacker_data)
{

	if (unpacker_data == nullptr || unpacker_data->GetUnPacker() == nullptr)return;

	UfxSubcribe::SubscribeHandle(unpacker_data);

	auto unpacker = unpacker_data->GetUnPacker();

	std::string msg_type = GetCursorValue<std::string>(unpacker, "msgtype");

	WriteHexFile(msg_type, (char*)unpacker->GetPackBuf(), unpacker->GetPackLen());

	for (int i = 0; i < (int)unpacker->GetRowCount(); ++i)
	{
		unpacker->Go(i + 1);

		if (msg_type == "a")//委托下达
		{
			//a

		}
		else if (msg_type == "b")//委托确认
		{
			//b

		}
		else if (msg_type == "c")//委托废单
		{
			//c


		}
		else if (msg_type == "d")//委托撤单
		{
			//d


		}
		else if (msg_type == "e")//委托撤成
		{
			//e


		}
		else if (msg_type == "f")//委托撤废
		{
			//f

		}
		else if (msg_type == "g")//委托成交
		{
			//g

		}
		//

	}
}
