#pragma once
#include <string>

class SApplication;

struct CommandLineOption
{
    bool showGui = false;
    bool showHelp = false;
    bool showVersion = false;
    bool autoSelectFirst = false;
    std::string dir;
    std::string filename;
    std::string url;

    std::string cookie;
    int pluginId = -1;

    bool isShowHelp() const;

    void setHelpText(const std::string& text);
    std::string getHelpText() const;

private:
    std::string helpText;
};

void attachConsole();

CommandLineOption parseCommandLineOption(int argc, char* argv[]);

int execCommandLine(const CommandLineOption& commandLine, SApplication& application);
