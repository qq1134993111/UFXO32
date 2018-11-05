#pragma once
#include "function_traits.hpp"
#include "boost/dll.hpp"
#include "boost/filesystem.hpp"
#include "boost/optional.hpp"
#include <exception>

class SharedLibraryLoader
{
public:
	SharedLibraryLoader() {}
	SharedLibraryLoader(const boost::filesystem::path& lib_full_path, boost::dll::load_mode::type mode = boost::dll::load_mode::default_mode)
	{
		Load(lib_full_path, mode);
	}
	~SharedLibraryLoader()
	{
		UnLoad();
	}

	SharedLibraryLoader(const SharedLibraryLoader& o) = delete;
	SharedLibraryLoader(SharedLibraryLoader&& o) = delete;
	SharedLibraryLoader& operator=(const SharedLibraryLoader& o) = delete;
	SharedLibraryLoader& operator=(SharedLibraryLoader&& o) = delete;

	bool Load(const boost::filesystem::path& lib_full_path, boost::dll::load_mode::type mode = boost::dll::load_mode::default_mode)
	{
		try
		{
			lib_.load(lib_full_path, mode);
		}
		catch (std::exception& e)
		{
			return false;
		}

		return true;
	}


	//template <typename T>
	//boost::optional<std::reference_wrapper<T>> Get(const std::string& symbol_name) const
	//{
	//	try
	//	{
	//		return boost::optional<std::reference_wrapper<T>>{ lib_.get<T>(symbol_name) };
	//	}
	//	catch (std::exception& e)
	//	{

	//	}

	//	return {};
	//}

	template <typename T>
	boost::optional<T&> Get(const std::string& symbol_name) const
	{
		try
		{
			//return boost::optional<T&>{ lib_.get<T>(symbol_name) };
			return  lib_.get<T>(symbol_name);
		}
		catch (std::exception& e)
		{

		}

		return {};
	}


	template<typename FUNC>
	FUNC GetFunction(const std::string& function_name)
	{
		auto op_func = Get<typename function_traits<FUNC>::function_type>(function_name);

		if (op_func)
		{
			return *op_func;
		}

		return nullptr;
	}

	void UnLoad()
	{
		lib_.unload();
	}

	bool IsLoad()
	{
		return lib_.is_loaded();
	}

	bool Has(const std::string& symbol_name)
	{
		return lib_.has(symbol_name);
	}

	boost::filesystem::path location()
	{
		try
		{
			lib_.location();
		}
		catch (...)
		{
		}
		return {};
	}

	const boost::dll::shared_library& GetSharedLibrary()
	{
		return lib_;
	}

private:
	boost::dll::shared_library lib_;
};