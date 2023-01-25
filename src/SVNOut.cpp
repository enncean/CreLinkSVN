#include "SVNOut.h"

#include <regex>

std::string SubstringXML(
	const std::string& xml, const std::string& tag_start, const std::string& tag_end,
	size_t start_pos = 0, size_t end_pos = std::string::npos)
{
	std::string xml_ignore_nl = regex_replace(xml, std::regex("\n( ){2,}"), " ");
	xml_ignore_nl = regex_replace(xml_ignore_nl, std::regex("\n"), "");

	const size_t tag_start_pos = xml_ignore_nl.find(tag_start, start_pos);
	const size_t value_start_pos = tag_start_pos + tag_start.length();
	const size_t value_end_pos = xml_ignore_nl.find(tag_end, start_pos + value_start_pos);
	
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

	return xml_ignore_nl.substr(value_start_pos, value_end_pos - value_start_pos);
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
	const std::string tag_url_start("<url>");
	const std::string tag_url_end("</url>");
	this->repository_url = SubstringXML(svn_out_xml, tag_url_start, tag_url_end);

	const std::string tag_relative_url_start("<relative-url>");
	const std::string tag_relative_url_end("</relative-url>");
	this->relative_url = SubstringXML(svn_out_xml, tag_relative_url_start, tag_relative_url_end).erase(0, 1);
	if (!this->relative_url.empty() && this->relative_url.back() == '/')
	{
		// Unify without trailing '/'
		// Repository root -> empty
		this->relative_url.pop_back();
	}

	const std::string tag_root_start("<root>");
	const std::string tag_root_end("</root>");
	this->repository_root_url = SubstringXML(svn_out_xml, tag_root_start, tag_root_end);

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
	const std::string file_entry_xml = SubstringXML(svn_out_xml, tag_file_entry_start, tag_file_entry_end);
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
