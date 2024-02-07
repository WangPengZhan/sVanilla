#pragma once

class PlatformInitializer
{
public:
    virtual ~PlatformInitializer() = default;
    
    virtual void initApplicationBefore() = 0;
    virtual void init() = 0;
    virtual void deinit() = 0;

    static void allApplicationBefore();
    static void allInit();
    static void allDeinit();
};