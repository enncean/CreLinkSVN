#include "RevisionSelector.h"

#include "../resource.h"

LRESULT CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

static int selected_revision = 0;

INT_PTR SelectorWindow(HINSTANCE hInstance)
{
	::selected_revision = 0;
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, (DLGPROC)DialogProc);
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
		break;
		
	case WM_COMMAND:
		if (wparam == IDOK)
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
		else if (wparam == IDCANCEL)
		{
			EndDialog(hWnd, IDCANCEL);
		}
		break;
		
	case WM_CLOSE:
		EndDialog(hWnd, IDCANCEL);
		break;
		
	default: ;
	}
	return 0;
}
