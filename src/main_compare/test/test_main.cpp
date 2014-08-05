#include <iostream>
#include <gtest/gtest.h>

int plus(int a, int b)
{
    return a + b;
}

TEST(Plus, PlusTest)
{
    EXPECT_EQ(2, plus(1,1));
    EXPECT_EQ(4, plus(2,2));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
