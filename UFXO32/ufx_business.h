#pragma once
#include "ufx_connecter.h"
#include"ufx_subcribe.h"
#include<iostream>

class UfxBusiness :public UfxSubcribe
{
public:
	UfxBusiness(const UfxConfig::UfxConfigData& data);
	~UfxBusiness()
	{
		Stop();
	};

	void AutoCheckConnect();

	//普通买卖委托
	ErrorCode Entrust();
	//委托撤单
	ErrorCode EntrustWithdraw();
	//证券委托查询
	ErrorCode EntrustQry();
	//证券成交查询
	ErrorCode RealdealQry();
	//	kUnitstkQry= 31001,//证券持仓查询
	ErrorCode UnitstkQry();
	//kCombofundQry = 34001,//账户资金查询
	ErrorCode CombofundQry();

	virtual void SubscribeHandle(std::shared_ptr<UfxUnpackerData> unpacker_data);

	void Stop()
	{
		ResetConnection();
		ResetSubcribe();
		StopProcessor();
		SetStatus(Status::kExit);
	}
protected:

private:
	boost::thread thread_check_connect_;
};

