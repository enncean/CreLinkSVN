#pragma once
#include <string>

#include "SVNOut.h"

class CreLinkCore
{
public:
	enum class ReadRepositoryResult
	{
		SUCCESS,
		NOT_REPOSITORY,
		FILE_CHECK_ERROR,
		FILE_NOT_VERSIONED
	};
	
	static bool CheckSubversionValid();

	CreLinkCore() = default;
	void SetPath(const std::string& path);
	ReadRepositoryResult ReadRepository();
	std::string GenerateURLWithRev(int revision) const;

private:
	std::string dir_path;
	std::string file_name;
	SVNInfo repository_info;
};

