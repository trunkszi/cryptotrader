//
// Created by Quincy on 2022/9/8.
//

#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}