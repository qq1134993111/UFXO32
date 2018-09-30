#pragma once
#include <stdint.h>
#include <string>
#include <tuple>
#include <utility>
using namespace std::rel_ops;

class ErrorCode
{
public:
	ErrorCode(int32_t value = 0, std::string message = "") :value_(value), message_(std::move(message)) {}
	ErrorCode(const ErrorCode& ec) noexcept
	{
		value_ = ec.value_;
		message_ = ec.message_;
	}
	ErrorCode& operator=(const ErrorCode& ec) noexcept
	{
		value_ = ec.value_;
		message_ = ec.message_;

		return *this;
	}

	ErrorCode(ErrorCode&& ec) noexcept
	{
		value_ = ec.value_;
		message_ = std::move(ec.message_);
		ec.value_ = 0;
	}

	ErrorCode& operator=(ErrorCode&& ec) noexcept
	{
		value_ = ec.value_;
		message_ = std::move(ec.message_);
		ec.value_ = 0;

		return *this;
	}

	~ErrorCode() noexcept {}
	void assign(int32_t value, std::string message) noexcept
	{
		value_ = value;
		message_ = std::move(message);
	}
	void clear() noexcept
	{
		value_ = 0;
		message_.clear();
	}

	const int32_t& value() const noexcept
	{
		return value_;
	}

	int32_t& value() noexcept
	{
		return value_;
	}

	const std::string& message() const noexcept
	{
		return message_;
	}

	std::string& message() noexcept
	{
		return message_;
	}

	//explicit operator int() const  noexcept
	//{
	//	return value_;
	//}

	explicit operator bool() const  noexcept
	{
		return (value_ != 0);
	}

	friend inline bool operator==(const ErrorCode& left, const ErrorCode& right) noexcept;
	friend inline bool operator<(const ErrorCode& left, const ErrorCode& right) noexcept;
	//friend inline bool operator==(const ErrorCode& left, int right) noexcept;
	//friend inline bool operator==(int left, const ErrorCode& right) noexcept;
	//friend inline bool operator<(const ErrorCode& left, int right) noexcept;
	//friend inline bool operator<(int left, const ErrorCode& right) noexcept;
private:
	int32_t value_;
	std::string message_;
};


inline bool operator==(const ErrorCode& left, const ErrorCode& right) noexcept
{
	return std::tie(left.value_, left.message_) == std::tie(right.value_, right.message_);
}


//inline bool operator==(const ErrorCode& left, int right) noexcept
//{
//	return (left.value() == right);
//}
//
//inline bool operator==(int left, const ErrorCode& right) noexcept
//{
//	return (left == right.value());
//}

inline bool operator<(const ErrorCode& left, const ErrorCode& right) noexcept
{
	return std::tie(left.value_, left.message_) < std::tie(right.value_, right.message_);
}


//inline bool operator<(const ErrorCode& left, int right) noexcept
//{
//	return (left.value() < right);
//}
//
//inline bool operator<(int left, const ErrorCode& right) noexcept
//{
//	return (left < right.value());
//}


