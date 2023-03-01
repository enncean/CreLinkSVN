#include "CreLinkCore.h"

#include <sstream>
#include <string>

bool ExecCmd(const std::string& cmd, std::string& cmd_out)
{
    FILE* fp = _popen(cmd.c_str(), "r");
    if (fp != nullptr)
    {
        char buf[1024];
        std::ostringstream out_ss{};
        while (fgets(buf, sizeof(buf), fp) != nullptr)
        {
            out_ss << buf;
        }
        cmd_out = out_ss.str();
        _pclose(fp);

        return true;
    }

    return false;
}

bool CreLinkCore::CheckSubversionValid()
{
	std::string cmd_ret("0");
	ExecCmd("where /q svn && echo 1 || echo 0", cmd_ret);
	
    return cmd_ret[0] == '1';
}

void CreLinkCore::SetPath(const std::string& path)
{
	const size_t pos_last = path.find_last_of("\\/");
	if (pos_last != std::string::npos)
	{
		this->dir_path = path.substr(0, pos_last);
		this->file_name = path.substr(pos_last + 1, std::string::npos);
	}
    else
    {
	    this->dir_path = "";
	    this->file_name = path;
    }
}

CreLinkCore::ReadRepositoryResult CreLinkCore::ReadRepository(bool skipCheck)
{
    if (!skipCheck)
    {
        // svn info -> repository url, relative path from wc root
        std::string rep_info;
        ExecCmd("cd /d \"" + this->dir_path + "\" && svn info --xml", rep_info);
        this->repository_info.ParseFromXML(rep_info);
        if (!this->repository_info)
        {
            return ReadRepositoryResult::NOT_REPOSITORY;
        }

        // svn status -> check file is versioned
        SVNStatus wc_files_status(this->file_name);
        std::string rep_status;
        ExecCmd("cd /d \"" + this->dir_path + "\" && svn status --xml", rep_status);
        wc_files_status.ParseFromXML(rep_status);
        if (!wc_files_status)
        {
            return ReadRepositoryResult::FILE_CHECK_ERROR;
        }
        if (!wc_files_status.IsTargetVersioned())
        {
            return ReadRepositoryResult::FILE_NOT_VERSIONED;
        }
    }
	
	// svn log
    std::string rep_log;
    ExecCmd("cd /d \"" + this->dir_path + "\" && svn log --xml \"" + this->file_name + "\"", rep_log);
    this->target_commit_log.ParseFromXML(rep_log);
	if (!this->target_commit_log)
	{
        return ReadRepositoryResult::LOG_NOT_EXIST;
	}

    return ReadRepositoryResult::SUCCESS;
}

std::vector<SVNLog::LogItem> *CreLinkCore::GetCommitLog()
{
    return &this->target_commit_log.commit_log;
}

std::string CreLinkCore::GenerateURLWithRev(int revision) const
{
    return
		this->repository_info.repository_root_url +
        "/!svn/bc/" +
        std::to_string(revision) +
        this->repository_info.relative_url +
        "/" + this->file_name;
}
