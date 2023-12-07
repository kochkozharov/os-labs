#include <dlfcn.h>
#include <gtest/gtest.h>

#include <cstring>
#include <utility>
std::pair<char *, char *> loadPaths() {
    std::pair<char *, char *> p;
    p.first = getenv("PATH_TO_LIB1");
    p.second = getenv("PATH_TO_LIB2");
    if (p.first == nullptr) {
        std::cerr << "PATH_TO_LIB1 is not specified\n";
        exit(EXIT_FAILURE);
    }
    if (p.second == nullptr) {
        std::cerr << "PATH_TO_LIB2 is not specified\n";
        exit(EXIT_FAILURE);
    }
    return p;
}

TEST(FourthLabTests, GCDTest) {
    auto names = loadPaths();
    int (*GCD1)(int, int);
    int (*GCD2)(int, int);
    auto *handle1 = dlopen(names.first, RTLD_LOCAL | RTLD_LAZY);
    auto *handle2 = dlopen(names.second, RTLD_LOCAL | RTLD_LAZY);
    GCD1 = reinterpret_cast<int (*)(int, int)>(dlsym(handle1, "GCD"));
    GCD2 = reinterpret_cast<int (*)(int, int)>(dlsym(handle2, "GCD"));
    EXPECT_EQ(GCD1(49, 56), 7);
    EXPECT_EQ(GCD1(56, 49), 7);
    EXPECT_EQ(GCD1(57, 49), 1);
    EXPECT_EQ(GCD2(49, 56), 7);
    EXPECT_EQ(GCD2(56, 49), 7);
    EXPECT_EQ(GCD2(57, 49), 1);
    dlclose(handle1);
    dlclose(handle2);
}

TEST(FourthLabTests, TranslationTest) {
    auto names = loadPaths();
    char *(*translationBinary)(long);
    char *(*translationTrinary)(long);
    auto *handle1 = dlopen(names.first, RTLD_LOCAL | RTLD_LAZY);
    auto *handle2 = dlopen(names.second, RTLD_LOCAL | RTLD_LAZY);
    translationBinary =
        reinterpret_cast<char *(*)(long)>(dlsym(handle1, "Translation"));
    translationTrinary =
        reinterpret_cast<char *(*)(long)>(dlsym(handle2, "Translation"));
    char *str = translationBinary(31);
    EXPECT_TRUE(std::strcmp(str, "11111") == 0);
    free(str);
    str = translationBinary(0);
    EXPECT_TRUE(std::strcmp(str, "0") == 0);
    free(str);
    str = translationTrinary(0);
    EXPECT_TRUE(std::strcmp(str, "0") == 0);
    free(str);
    str = translationTrinary(5);
    EXPECT_TRUE(std::strcmp(str, "12") == 0);
    free(str);
    dlclose(handle1);
    dlclose(handle2);
}