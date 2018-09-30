#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include"boost/algorithm/hex.hpp"

static inline void WriteHexFile(const std::string &filepath, const char* data,int length) 
{
	auto content=boost::algorithm::hex(std::string(data,length));
	std::ofstream ofs(filepath.c_str());
	ofs << content;
	ofs.close();
	return;
}