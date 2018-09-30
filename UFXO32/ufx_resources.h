#pragma once

#include<string>
#include<memory>

#include "boost/variant.hpp"

#include <t2sdk_interface.h>



template<typename T>
class UfxResourcesDeleter
{
public:
	void operator()(T* resources)
	{
		resources->Release();
	}
};

template<>
class UfxResourcesDeleter<IF2Packer>
{
public:
	void operator()(IF2Packer* resources)
	{
		resources->FreeMem(resources->GetPackBuf());
		resources->Release();
	}
};

template<typename T>
std::shared_ptr<T> UfxResourcesToShared(T* ptr)
{
	return std::shared_ptr<T>(ptr, UfxResourcesDeleter<T>());
}


using UfxFiledVarType = boost::variant<int, double, char, std::string>;
class UfxFiledVarTypeVisitor :boost::static_visitor<void>
{
public:
	enum class VisitorFalg :int32_t
	{
		kAddFiled = 1,
		kAddValue = 2
	};

	UfxFiledVarTypeVisitor(IF2Packer* packer, std::string name, VisitorFalg flag, std::basic_ostream<char>* p_ostream = nullptr)
	{
		packer_ = packer;
		field_name_ = std::move(name);
		flag_ = flag;

		p_ostream_ = p_ostream;
	}

public:
	void operator() (const double& var) const
	{
		if (flag_ == VisitorFalg::kAddFiled)
		{
			packer_->AddField(field_name_.c_str(), 'F');
		}
		else if (flag_ == VisitorFalg::kAddValue)
		{
			if (p_ostream_ != nullptr)
			{
				*p_ostream_ << field_name_ << ":" << var << "\n";
			}
			packer_->AddDouble(var);
		}
	}
	void operator()(const char& var) const
	{
		if (flag_ == VisitorFalg::kAddFiled)
		{
			packer_->AddField(field_name_.c_str(), 'C', sizeof(var), 0);
		}
		else if (flag_ == VisitorFalg::kAddValue)
		{
			if (p_ostream_ != nullptr)
			{
				*p_ostream_ << field_name_ << ":" << var << "\n";
			}
			packer_->AddChar(var);
		}
	}

	void operator()(const int& var) const
	{
		if (flag_ == VisitorFalg::kAddFiled)
		{
			packer_->AddField(field_name_.c_str(), 'I');
		}
		else if (flag_ == VisitorFalg::kAddValue)
		{
			if (p_ostream_ != nullptr)
			{
				*p_ostream_ << field_name_ << ":" << var << "\n";
			}
			packer_->AddInt(var);
		}

	}

	void operator()(const std::string& var) const
	{
		if (flag_ == VisitorFalg::kAddFiled)
		{
			packer_->AddField(field_name_.c_str(), 'S', var.size(), 0);
		}
		else if (flag_ == VisitorFalg::kAddValue)
		{
			if (p_ostream_ != nullptr)
			{
				*p_ostream_ << field_name_ << ":" << var << "\n";
			}
			packer_->AddStr(var.c_str());
		}
	}

private:
	IF2Packer * packer_ = nullptr;
	std::string field_name_;
	VisitorFalg flag_;//1添加Field，2添加数据

	std::basic_ostream<char>* p_ostream_ = nullptr;
};


class UfxUnpackerData
{
public:
	UfxUnpackerData(std::string recv_data)
	{
		data_ = std::move(recv_data);
		unpack_ = NewUnPacker((void*)data_.data(), data_.size());
	}
	~UfxUnpackerData()
	{
		if (unpack_ != nullptr)
		{
			unpack_->Release();
		}
	}
	const std::string& GetSourceData()
	{
		return data_;
	}

	IF2UnPacker* GetUnPacker()
	{
		return unpack_;
	}

	UfxUnpackerData(const UfxUnpackerData& unpacker_data) = delete;
	UfxUnpackerData(UfxUnpackerData&& unpacker_data) = delete;
	UfxUnpackerData& operator=(const UfxUnpackerData& unpacker_data) = delete;
	UfxUnpackerData& operator=(UfxUnpackerData&& unpacker_data) = delete;
private:
	std::string data_;
	IF2UnPacker* unpack_ = nullptr;
};