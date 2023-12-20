#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

#include <gtest/gtest.h>

#include "zipper/Zipper.h"

TEST(Zipper_Test, zip)
{
    Zipper zipper;
    zipper.setZipPath("D:/workfile/sVanilla/test/TestData/Zipper/zipper");
    zipper.setOutputFileName("D:/workfile/sVanilla/test/TestData/Zipper/zipped.zip");
    bool result = zipper.zip();
    EXPECT_EQ(result, true);
}

TEST(Zipper_Test, unzip)
{
    Unzipper unzipper;
    unzipper.setUnzippedFile("D:/workfile/sVanilla/test/TestData/Zipper/unzipper/zipper.zip");
    unzipper.setOutputPath("D:/workfile/sVanilla/test/TestData/Zipper/unzipper/");
    bool result = unzipper.unzip();
    EXPECT_EQ(result, true);
}
