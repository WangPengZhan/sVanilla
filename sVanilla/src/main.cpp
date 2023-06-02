#include <QApplication>
#include <QMainWindow>

#include <client/windows/handler/exception_handler.h>

bool callback(const wchar_t *dump_path, const wchar_t *id,
	void *context, EXCEPTION_POINTERS *exinfo,
	MDRawAssertionInfo *assertion,
	bool succeeded)
{
	if (succeeded) {
		printf("dump guid is %ws\n", id);
	}
	else {
		printf("dump failed\n");
	}
	system("pause");
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


