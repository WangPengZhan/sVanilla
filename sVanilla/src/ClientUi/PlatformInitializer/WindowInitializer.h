#pragma once
#ifdef _WIN32

#include "PlatformInitializer.h"

class WindowInitializer : public PlatformInitializer
{
public:
    WindowInitializer();
    ~WindowInitializer();

    void initApplicationBefore() override;
    void init() override;
    void deinit() override;
};

#endif  // _WIN32
