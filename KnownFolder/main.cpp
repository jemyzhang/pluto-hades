#include <QtCore/QCoreApplication>
#include <QtGui/QApplication>
#include <QLibrary>
#include <QLabel>
#include <QDebug>

#include <shlobj.h>
#include <shobjidl.h>

typedef HRESULT (*SHGetKnownFolderItem)(REFKNOWNFOLDERID rfid,
										DWORD dwFlags, 
										HANDLE hToken,
										REFIID riid,
										void **ppv
										);

int main(int argc, char *argv[])
{
	QLibrary lib("shell32.dll");
	QApplication a(argc, argv);
	QLabel label;

	if (lib.load())
		qDebug()<<"Load shell32.dll successful.";

	SHGetKnownFolderItem func = (SHGetKnownFolderItem)lib.resolve("SHGetKnownFolderItem");

	if (func)
	{
		::CoInitializeEx(NULL, COINIT_MULTITHREADED);

		qDebug()<<"Resolve function SHGetKnownFolderItem successful.";

		IShellItem* item = NULL;

		HRESULT rt = func(FOLDERID_Documents, NULL, NULL, IID_IShellItem, (void**)&item);

		if (item)
		{
			//qDebug()<<"Get IShellItem successful.";

			LPWSTR name = NULL;

			item->GetDisplayName(SIGDN_NORMALDISPLAY, &name);
			//item->GetDisplayName(SIGDN_PARENTRELATIVEEDITING, &name);
			//item->GetDisplayName(SIGDN_DESKTOPABSOLUTEEDITING, &name);

			if (name)
			{
				QString displayName = QString::fromUtf16(name);

				qDebug()<<"Display name : "<<displayName;

				CoTaskMemFree(name);


				label.setText(displayName);

				label.show();
			}

			item->Release();
		}
	}


	return a.exec();
}
