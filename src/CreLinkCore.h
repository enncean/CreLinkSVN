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
		FILE_NOT_VERSIONED,
		LOG_NOT_EXIST
	};
	
	static bool CheckSubversionValid();

	CreLinkCore() = default;
	void SetPath(const std::string& path);
	ReadRepositoryResult ReadRepository();
	std::vector<SVNLog::LogItem> *GetCommitLog();
	std::string GenerateURLWithRev(int revision) const;

private:
	std::string dir_path;
	std::string file_name;
	SVNInfo repository_info;
	SVNLog target_commit_log;
};

