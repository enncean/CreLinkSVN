#include "SVNOut.h"

#include <iomanip>
#include <sstream>
#include <regex>

std::string TranslateToOneLineXML(const std::string& xml)
{
	std::string xml_ol = regex_replace(xml, std::regex("\n( ){2,}"), " ");
	return regex_replace(xml_ol, std::regex("\n"), "");;
}
std::string SubstringXML(
	const std::string& xml, const std::string& tag_start, const std::string& tag_end,
	size_t start_pos = 0, size_t end_pos = std::string::npos)
{
	const size_t tag_start_pos = xml.find(tag_start, start_pos);
	const size_t value_start_pos = tag_start_pos + tag_start.length();
	const size_t value_end_pos = xml.find(tag_end, value_start_pos);
	
	if (tag_start_pos == std::string::npos || value_end_pos == std::string::npos)
	{
		// not found in xml
		return "";
	}
	if (end_pos != std::string::npos && (value_end_pos + tag_end.length()) > end_pos)
	{
		// not found in specified range
		return "";
	}

	return xml.substr(value_start_pos, value_end_pos - value_start_pos);
}

/* ==== SVNOut ==== */

SVNOut::SVNOut() : valid(false)
{
}

void SVNOut::SetValid()
{
	this->valid = true;
}

/* ==== SVNInfo ==== */

void SVNInfo::ParseFromXML(const std::string& svn_out_xml)
{
	const std::string xml_ol = TranslateToOneLineXML(svn_out_xml);
	
	const std::string tag_url_start("<url>");
	const std::string tag_url_end("</url>");
	this->repository_url = SubstringXML(xml_ol, tag_url_start, tag_url_end);

	const std::string tag_relative_url_start("<relative-url>");
	const std::string tag_relative_url_end("</relative-url>");
	this->relative_url = SubstringXML(xml_ol, tag_relative_url_start, tag_relative_url_end).erase(0, 1);
	if (!this->relative_url.empty() && this->relative_url.back() == '/')
	{
		// Unify without trailing '/'
		// Repository root -> empty
		this->relative_url.pop_back();
	}

	const std::string tag_root_start("<root>");
	const std::string tag_root_end("</root>");
	this->repository_root_url = SubstringXML(xml_ol, tag_root_start, tag_root_end);

	if (!this->repository_url.empty() && !this->repository_root_url.empty())
	{
		SetValid();
	}
}

/* ==== SVNStatus ==== */

SVNStatus::SVNStatus(const std::string& target_file) : SVNOut(), target_status(ItemStatus::UNKNOWN)
{
	SetTargetFile(target_file);
}

void SVNStatus::SetTargetFile(const std::string& target_file)
{
	this->target_file = target_file;
}

void SVNStatus::ParseFromXML(const std::string& svn_out_xml)
{
	if (this->target_file.length() <= 0)
	{
		return;
	}
	
	const std::string tag_file_entry_start("<entry path=\"" + this->target_file + "\">");
	const std::string tag_file_entry_end("</entry>");
	const std::string file_entry_xml = SubstringXML(TranslateToOneLineXML(svn_out_xml), tag_file_entry_start, tag_file_entry_end);
	if (file_entry_xml.length() > 0)
	{
		const std::string tag_wcstat_head("<wc-status");
		const std::string tag_wcstat_tail(">");
		const std::string file_wcstat_elements = SubstringXML(file_entry_xml, tag_wcstat_head, tag_wcstat_tail);
		if (file_wcstat_elements.length() > 0)
		{
			const std::string element_item_head("item=\"");
			const std::string element_item_tail("\"");
			const std::string item_status = SubstringXML(file_wcstat_elements, element_item_head, element_item_tail);
			if (item_status == "added")
			{
				this->target_status = ItemStatus::ADDED;
			}
			else if (item_status == "unversioned")
			{
				this->target_status = ItemStatus::NOT_VERSIONED;
			}
			else
			{
				this->target_status = ItemStatus::OTHER;
			}
			SetValid();
		}
		// else > unknown status
	}
	else
	{
		// under version control & not modified.
		this->target_status = ItemStatus::OTHER;
		SetValid();
	}
}

bool SVNStatus::IsTargetVersioned() const
{
	return this->target_status == ItemStatus::OTHER;
}

/* ==== SVNLog ==== */

SVNLog::LogItem::LogItem(int a_revision, const std::string& a_author, const std::string& a_commit_datetime, const std::string& a_commit_message) :
	revision(a_revision), author(a_author), commit_datetime(a_commit_datetime), commit_message(a_commit_message)
{
}

void SVNLog::ParseFromXML(const std::string& svn_out_xml)
{
	const std::string xml_ol = TranslateToOneLineXML(svn_out_xml);
	
	const std::string tag_logentry_start("<logentry ");
	const std::string tag_logentry_end("</logentry>");
	const int substr_key_length = tag_logentry_start.length() + tag_logentry_end.length();
	int read_point = xml_ol.find(tag_logentry_start);
	std::string logentry;
	do
	{
		logentry = SubstringXML(xml_ol, tag_logentry_start, tag_logentry_end, read_point);
		ParseAndAddLogItem(logentry);
		read_point += logentry.length() + substr_key_length;
	} while (!logentry.empty());

	if (!this->commit_log.empty())
	{
		SetValid();
	}
}

void SVNLog::ParseAndAddLogItem(const std::string& logentry_xml)
{
	const std::string element_revision_start("revision=\"");
	const std::string element_revision_end("\">");
	const std::string tag_author_start("<author>");
	const std::string tag_author_end("</author>");
	const std::string tag_date_start("<date>");
	const std::string tag_date_end("</date>");
	const std::string tag_msg_start("<msg>");
	const std::string tag_msg_end("</msg>");
	try
	{
		LogItem item(
			std::stoi(SubstringXML(logentry_xml, element_revision_start, element_revision_end)),
			SubstringXML(logentry_xml, tag_author_start, tag_author_end),
			ParseDayTime(SubstringXML(logentry_xml, tag_date_start, tag_date_end)),
			SubstringXML(logentry_xml, tag_msg_start, tag_msg_end)
		);
		this->commit_log.push_back(item);
	}
	catch (std::invalid_argument& e) // std::stoi() failed
	{
	}
	catch (std::out_of_range& e) // std::stoi() failed
	{
	}
}

std::string SVNLog::ParseDayTime(const std::string& log_daytime)
{
	int year, month, date, hour, min, sec;
	sscanf_s(log_daytime.c_str(), "%d-%d-%dT%d:%d:%dZ",
		&year, &month, &date, &hour, &min, &sec);

	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << year << "/";
	ss << std::setw(2) << std::setfill('0') << month << "/";
	ss << std::setw(2) << std::setfill('0') << date << " ";
	ss << std::setw(2) << std::setfill('0') << hour << ":";
	ss << std::setw(2) << std::setfill('0') << min << ":";
	ss << std::setw(2) << std::setfill('0') << sec;

	return ss.str();
}
