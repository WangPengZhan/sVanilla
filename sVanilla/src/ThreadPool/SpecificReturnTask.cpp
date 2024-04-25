
#include "SpecificReturnTask.h"

auto SpecificReturnTask::runner() -> SpecificReturnTask&
{
    static SpecificReturnTask instance;
    return instance;
}