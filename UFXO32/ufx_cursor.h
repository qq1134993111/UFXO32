#pragma once
#include <memory>

#include <t2sdk_interface.h>

#include "boost/algorithm/string.hpp"

template<typename T>
class GetCursorValueInner
{
public:
	GetCursorValueInner(IF2UnPacker* unpacker, std::string col_name) :unpacker_(unpacker), col_name_(std::move(col_name))
	{
		static_assert(std::is_same<T, char>::value ||
			std::is_same<T, int>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, std::string>::value, "not support type,GetCursorValueInner");
	}

	GetCursorValueInner(std::shared_ptr<IF2UnPacker> unpacker, std::string col_name) :GetCursorValueInner(unpacker.get(), std::move(col_name))
	{

	}

	operator T()
	{
		static_assert(std::is_same<T, char>::value ||
			std::is_same<T, int>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, std::string>::value, "not support type,GetCursorValueInner");
	}

private:
	IF2UnPacker * unpacker_ = nullptr;
	std::string col_name_;
};

template<>
class GetCursorValueInner<int>
{
public:
	GetCursorValueInner(IF2UnPacker* unpacker, std::string col_name) :unpacker_(unpacker), col_name_(std::move(col_name))
	{
	}

	GetCursorValueInner(std::shared_ptr<IF2UnPacker> unpacker, std::string col_name) :GetCursorValueInner(unpacker.get(), std::move(col_name))
	{

	}

	operator int()
	{
		return unpacker_->GetInt(col_name_.c_str());
	}

private:
	IF2UnPacker * unpacker_ = nullptr;
	std::string col_name_;
};

template<>
class GetCursorValueInner<char>
{
public:
	GetCursorValueInner(IF2UnPacker* unpacker, std::string col_name) :unpacker_(unpacker), col_name_(std::move(col_name))
	{
	}

	GetCursorValueInner(std::shared_ptr<IF2UnPacker> unpacker, std::string col_name) :GetCursorValueInner(unpacker.get(), std::move(col_name))
	{

	}

	operator char()
	{
		return unpacker_->GetChar(col_name_.c_str());
	}

private:
	IF2UnPacker * unpacker_ = nullptr;
	std::string col_name_;
};


template<>
class GetCursorValueInner<double>
{
public:
	GetCursorValueInner(IF2UnPacker* unpacker, std::string col_name) :unpacker_(unpacker), col_name_(std::move(col_name))
	{
	}

	GetCursorValueInner(std::shared_ptr<IF2UnPacker> unpacker, std::string col_name) :GetCursorValueInner(unpacker.get(), std::move(col_name))
	{

	}

	operator double()
	{
		return unpacker_->GetDouble(col_name_.c_str());
	}

private:
	IF2UnPacker * unpacker_ = nullptr;
	std::string col_name_;
};


template<>
class GetCursorValueInner<std::string>
{
public:
	GetCursorValueInner(IF2UnPacker* unpacker, std::string col_name) :unpacker_(unpacker), col_name_(std::move(col_name))
	{
	}

	GetCursorValueInner(std::shared_ptr<IF2UnPacker> unpacker, std::string col_name) :GetCursorValueInner(unpacker.get(), std::move(col_name))
	{

	}

	operator std::string()
	{
		auto value = unpacker_->GetStr(col_name_.c_str());
		if (value == nullptr)
		{
			return "";
		}

		return boost::algorithm::trim_copy(std::string(value));
	}

private:
	IF2UnPacker * unpacker_ = nullptr;
	std::string col_name_;
};


template<typename T>
T GetCursorValue(IF2UnPacker* unpacker, std::string col_name)
{
	return GetCursorValueInner<T>(unpacker, std::move(col_name));
}

template<typename T>
T GetCursorValue(std::shared_ptr<IF2UnPacker> unpacker, std::string col_name)
{
	return GetCursorValueInner<T>(unpacker, std::move(col_name));
}


class UfxCursorField
{
public:
	UfxCursorField(IF2UnPacker* unpacker, std::string col_name = "") :unpacker_(unpacker), col_name_(std::move(col_name))
	{
	}

	UfxCursorField(std::shared_ptr<IF2UnPacker> unpacker, std::string col_name = "") :UfxCursorField(unpacker.get(), std::move(col_name))
	{

	}

	void GoField(std::string col_name)
	{
		col_name_ = std::move(col_name);
	}

	char ToChar()
	{
		return unpacker_->GetChar(col_name_.c_str());
	}

	int ToInt()
	{
		return unpacker_->GetInt(col_name_.c_str());
	}

	double ToDouble()
	{
		return unpacker_->GetDouble(col_name_.c_str());
	}

	std::string ToString()
	{
		auto value = unpacker_->GetStr(col_name_.c_str());
		if (value == nullptr)
		{
			return "";
		}

		return boost::algorithm::trim_copy(std::string(value));
	}

	operator char()
	{
		return ToChar();
	}

	operator int()
	{
		return ToInt();
	}

	operator double()
	{
		return ToDouble();
	}

	operator std::string()
	{
		return ToString();
	}

private:
	IF2UnPacker * unpacker_ = nullptr;
	std::string col_name_;
};


template<typename T>
T GetCursorValue2(IF2UnPacker* unpacker, std::string col_name)
{
	return static_cast<T>(UfxCursorField(unpacker, std::move(col_name)));
}

template<typename T>
T GetCursorValue2(std::shared_ptr<IF2UnPacker> unpacker, std::string col_name)
{
	return static_cast<T>(UfxCursorField(unpacker, std::move(col_name)));
}