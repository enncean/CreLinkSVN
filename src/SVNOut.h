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
