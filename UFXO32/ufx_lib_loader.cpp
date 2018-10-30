
#include"ufx_lib_loader.h"

SharedLibraryLoader UfxLibLoader::s_lib;

UfxLibLoader::T2SDK_NewConfig UfxLibLoader::NewConfig = nullptr;

UfxLibLoader::T2SDK_NewConnection UfxLibLoader::NewConnection = nullptr;

UfxLibLoader::T2SDK_NewPacker UfxLibLoader::NewPacker = nullptr;

UfxLibLoader::T2SDK_NewUnPacker UfxLibLoader::NewUnPacker=nullptr;

UfxLibLoader::T2SDK_NewSubscribeParam UfxLibLoader::NewSubscribeParam = nullptr;

UfxLibLoader::T2SDK_NewBizMessage UfxLibLoader::NewBizMessage = nullptr;

UfxLibLoader::T2SDK_NewFilter UfxLibLoader::NewFilter = nullptr;

bool UfxLibLoader::Load(boost::filesystem::path lib_full_path)
{
	bool b_ret = s_lib.Load(lib_full_path);

	NewConfig = s_lib.GetFunction<decltype(NewConfig)>("NewConfig");
	NewConnection = s_lib.GetFunction<decltype(NewConnection)>("NewConnection");
	NewPacker = s_lib.GetFunction<decltype(NewPacker)>("NewPacker");
	NewUnPacker = s_lib.GetFunction<decltype(NewUnPacker)>("NewUnPacker");
	NewSubscribeParam = s_lib.GetFunction<decltype(NewSubscribeParam)>("NewSubscribeParam");
	NewBizMessage = s_lib.GetFunction<decltype(NewBizMessage)>("NewBizMessage");
	NewFilter = s_lib.GetFunction<decltype(NewFilter)>("NewFilter");

	return (b_ret&&NewConfig&&NewConnection&&NewPacker&&NewUnPacker&&NewSubscribeParam&&NewBizMessage&&NewFilter);
}
