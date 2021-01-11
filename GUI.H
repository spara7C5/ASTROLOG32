#ifndef GuiH
#define GuiH
#include "Language.h"
#include "res_strings.h"

// Windows interface
#define CheckMenu(cmd, f) \
  CheckMenuItem(wi.hmenu, (uint)cmd, f ? MF_CHECKED : MF_UNCHECKED);
#define WiCheckMenu(cmd, f) CheckMenu(cmd, f); wi.fMenu = TRUE
#define WiDoDialog(pfn, dlg) DialogBox(Lang.resource, MAKEINTRESOURCE(dlg), wi.hwnd, pfn);
#define SetCheck(id, f) CheckDlgButton(hdlg, id, f)
#define SetRadio(id, idLo, idHi) CheckRadioButton(hdlg, idLo, idHi, id)
#define SetEdit(id, sz) SetDlgItemText(hdlg, id, (LPCSTR)sz)
#define SetEditN(id, n) SetDlgItemInt(hdlg, id, n, TRUE)
#define SetCombo(id, sz) \
  SendDlgItemMessage(hdlg, id, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)sz)
#define ClearCombo(id) SendDlgItemMessage(hdlg, id, CB_RESETCONTENT, 0, 0);
#define GetCheck(id) IsDlgButtonChecked(hdlg, id)
#define GetEdit(id, sz) GetDlgItemText(hdlg, id, sz, cchSzDef)
#define GetEditN(id) GetDlgItemInt(hdlg, id, NULL, TRUE)

// validation
#define EnsureN(n, f, sz) if (!(f)) { WarnWrongValue(n, sz); return TRUE; }
#define EnsureR(r, f, sz) EnsureN((int)r, f, sz)

#endif