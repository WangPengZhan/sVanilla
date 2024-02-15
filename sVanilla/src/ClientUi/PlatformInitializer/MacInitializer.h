#ifdef __APPLE__
#pragma once

#include "PlatformInitializer.h"

class MacInitializer : public PlatformInitializer
{
public:
    MacInitializer();
    ~MacInitializer();

    void initApplicationBefore() override;
    void init() override;
    void deinit() override;
};

#endif  // __APPLE__
