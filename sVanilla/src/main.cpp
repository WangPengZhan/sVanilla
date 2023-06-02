#include <QApplication>
#include <QMainWindow>
#include <QFile>

#include <client/windows/handler/exception_handler.h>

bool callback(const wchar_t *dump_path, const wchar_t *id,
	void *context, EXCEPTION_POINTERS *exinfo,
	MDRawAssertionInfo *assertion,
	bool succeeded)
{
	QFile file("test.txt");
	file.open(QIODevice::WriteOnly);
	std::wstring wstr = dump_path;
	wstr += L":";
	wstr += L":";
	wstr += id;
	file.write(QString::fromStdWString(wstr).toLocal8Bit());
	file.close();
	return succeeded;
}

static void crashHare()
{
    int *a = (int *)(NULL);
    *a = 1; // 放心的奔溃吧
}

int main(int argc, char *argv[])
{
    google_breakpad::ExceptionHandler eh(
    L"./dump", NULL, callback, NULL,
    google_breakpad::ExceptionHandler::HANDLER_ALL);
    crashHare();

    QApplication a(argc, argv);

    QMainWindow w;
    w.show();
    
    return a.exec();
}


