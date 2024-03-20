#pragma once
#ifdef __APPLE__

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
