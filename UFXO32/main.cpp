#include <t2sdk_interface.h>
#include <iostream>
#include <string>
using namespace std;

//#include "boost/algorithm/hex.hpp"

//#include "ufx_custom_field.h"

#include "singleton.hpp"
#include "ufx_business.h"

int main(int argc, char** argv)
{
	ErrorCode ec;
	ec = Singleton<UfxConfig>::Instance()->Load(".");
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}

	Singleton<UfxBusiness>::Instance(Singleton<UfxConfig>::GetInstance()->GetConfigData());

	UfxBusiness* ufx_business = Singleton<UfxBusiness>::GetInstance();


	ufx_business->AutoCheckConnect();
	ufx_business->AutoSendHeartbeat();


	ec = ufx_business->Subcribe();
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}

	ec = ufx_business->Connect();
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}

	ec = ufx_business->Login();
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}


	ec = ufx_business->Entrust();
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}


	ufx_business->GetUfxProcessor().AddTimer(std::chrono::microseconds(1000 * 1000 * 10), [&]()
	{
		ec = ufx_business->Entrust();
		if (ec)
		{
			std::cout << ec.message() << std::endl;
			//return 0;
		}
		return true;
	});

	ec = ufx_business->EntrustWithdraw();
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}


	ec = ufx_business->EntrustQry();
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}

	ec = ufx_business->UnitstkQry();
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}

	ec = ufx_business->CombofundQry();
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		//return 0;
	}

	ec = ufx_business->RealdealQry();
	if (ec)
	{
		//std::cout << ec.message() << std::endl;
		//return 0;
	}

	system("pause");

	return 0;
}

