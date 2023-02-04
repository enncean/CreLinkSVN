#pragma once
#include <string>
#include <vector>

class SVNOut
{
public:
	SVNOut();
	virtual void ParseFromXML(const std::string& svn_out_xml) = 0;

	inline bool operator!() const
	{
		return !this->valid;
	}

protected:
	void SetValid();

private:
	bool valid;
};

class SVNInfo : public SVNOut
{
public:
	SVNInfo() = default;
	void ParseFromXML(const std::string& svn_out_xml) override;

	std::string repository_url;
	std::string relative_url;
	std::string repository_root_url;
};

class SVNStatus : public SVNOut
{
public:
	enum class ItemStatus
	{
		ADDED,
		NOT_VERSIONED,
		OTHER,
		UNKNOWN
	};

	SVNStatus() = default;
	SVNStatus(const std::string& target_file);
	void SetTargetFile(const std::string& target_file);
	void ParseFromXML(const std::string& svn_out_xml) override;

	ItemStatus target_status;
	bool IsTargetVersioned() const;

private:
	std::string target_file;
};

class SVNLog : public SVNOut
{
public:
	class LogItem
	{
	public:
		LogItem(int a_revision, const std::string& a_author, const std::string& a_commit_datetime, const std::string& a_commit_message);
		
		const int revision;
		const std::string author;
		const std::string commit_datetime;
		const std::string commit_message;
	};
	
	SVNLog() = default;
	void ParseFromXML(const std::string& svn_out_xml) override;

	std::vector<LogItem> commit_log;
	void ParseAndAddLogItem(const std::string& logentry_xml);
};
