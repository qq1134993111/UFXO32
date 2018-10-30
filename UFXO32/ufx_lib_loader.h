#pragma once
#include "shared_library_loader.h"
#include "t2sdk_interface.h"
#include<functional>

struct UfxLibLoader
{
	using  T2SDK_NewConfig = std::function<CConfigInterface* FUNCTION_CALL_MODE()>;
	using  T2SDK_NewConnection = std::function<CConnectionInterface* FUNCTION_CALL_MODE(CConfigInterface *lpConfig)>;
	using  T2SDK_NewPacker = std::function<IF2Packer * FUNCTION_CALL_MODE(int iVersion)>;
	using  T2SDK_NewUnPacker = std::function<IF2UnPacker * FUNCTION_CALL_MODE(void * lpBuffer, unsigned int iLen)>;
	using  T2SDK_NewSubscribeParam = std::function<CSubscribeParamInterface* FUNCTION_CALL_MODE() >;
	using  T2SDK_NewBizMessage = std::function<IBizMessage* FUNCTION_CALL_MODE()>;
	using  T2SDK_NewFilter = std::function<CFilterInterface* FUNCTION_CALL_MODE()>;

	static SharedLibraryLoader s_lib;
	static T2SDK_NewConfig NewConfig;
	static T2SDK_NewConnection NewConnection;
	static T2SDK_NewPacker NewPacker;
	static T2SDK_NewUnPacker NewUnPacker;
	static T2SDK_NewSubscribeParam NewSubscribeParam;
	static T2SDK_NewBizMessage NewBizMessage;
	static T2SDK_NewFilter NewFilter;

	static bool Load(boost::filesystem::path lib_full_path);
};


