#include "RevisionSelector.h"

#include <sstream>
#include <iomanip>

#include "../resource.h"

LRESULT CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

static std::vector<SVNLog::LogItem>* revision_candidates = nullptr;
static int selected_revision = 0;

INT_PTR SelectorWindow(HINSTANCE hInstance, std::vector<SVNLog::LogItem>& commit_log)
{
	::revision_candidates = &commit_log;
	::selected_revision = 0;
	const INT_PTR ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, (DLGPROC)DialogProc);
	::revision_candidates = nullptr;
	return ret;
}

int GetSelectedRevision()
{
	return ::selected_revision;
}

LRESULT CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{	// move window center
			RECT window_rect, desktop_rect;
			GetWindowRect(hWnd, &window_rect);
			GetWindowRect(GetDesktopWindow(), &desktop_rect);
			const LONG window_width = (window_rect.right - window_rect.left);
			const LONG window_height = (window_rect.bottom - window_rect.top);
			MoveWindow(hWnd,
				((desktop_rect.right - desktop_rect.left) / 2) - (window_width / 2),
				((desktop_rect.bottom - desktop_rect.top) / 2) - (window_height / 2),
				window_width, window_height, TRUE);
		}
		if (::revision_candidates != nullptr)
		{
			HWND hListbox = GetDlgItem(hWnd, IDC_LIST1);
			for (SVNLog::LogItem log_item : *revision_candidates)
			{
				std::ostringstream ss;
				ss << std::setfill(' ') << std::setw(5) << log_item.revision
					<< " | " << log_item.commit_datetime
					<< " | " << log_item.author
					<< " | " << log_item.commit_message;
				std::string list_text = ss.str();
				int pos = SendMessage(hListbox, LB_ADDSTRING,
					0, (LPARAM)std::wstring(list_text.begin(), list_text.end()).c_str());
				SendMessage(hListbox, LB_SETITEMDATA, pos, (LPARAM)log_item.revision);
			}
		}
		SetFocus(GetDlgItem(hWnd, IDC_EDIT1));
		break;
		
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDOK:
			{
				char buffer[16];
				GetDlgItemTextA(hWnd, IDC_EDIT1, buffer, 16);
				::selected_revision = atoi(buffer);
				if (::selected_revision > 0)
				{
					EndDialog(hWnd, IDOK);
				}
				else
				{
					EndDialog(hWnd, IDCANCEL);
				}
			}
			break;
			
		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			break;
			
		default: ;
		}
		break;
		
	case WM_CLOSE:
		EndDialog(hWnd, IDCANCEL);
		break;
		
	default: ;
	}
	return 0;
}
