#include <gtest/gtest.h>

extern "C" {
#include "lab3.h"
}

#include <array>
#include <filesystem>
#include <fstream>
#include <memory>

namespace fs = std::filesystem;

TEST(FirstLabTests, SimpleTest) {
    const char *childPath = getenv("PATH_TO_CHILD");
    ASSERT_TRUE(childPath);

    const std::string fileWithInput = "input.txt";
    const std::string fileWithOutput1 = "output1.txt";
    const std::string fileWithOutput2 = "output2.txt";

    constexpr int outputSize1 = 5;
    constexpr int outputSize2 = 3;
    constexpr int inputSize = outputSize1 + outputSize2;

    const std::array<std::string, inputSize> input = {
        "rev",
        "revvvvv",
        "162te16782r18223r2",
        "",
        "r",
        "pqwrpqlwfpqwoglqwpglqpgwpqw.,g;q.wg;q.wg;w.qg;.w",
        "12345678900",
        "1234567890",
    };

    const std::array<std::string, outputSize1> expectedOutput1 = {
        "ver",
        "vvvvver",
        "",
        "r",
        "0987654321",
    };

    const std::array<std::string, outputSize2> expectedOutput2 = {
        "2r32281r28761et261",
        "w.;gq.w;gw.q;gw.q;g,.wqpwgpqlgpwqlgowqpfwlqprwqp",
        "00987654321",
    };

    {
        auto inFile = std::ofstream(fileWithInput);
        inFile << fileWithOutput1 << '\n'
               << fileWithOutput2 << '\n';
        for (const auto &line : input) {
            inFile << line << '\n';
        }
    }

    auto deleter = [](FILE *file) {
        fclose(file);
    };

    std::unique_ptr<FILE, decltype(deleter)> inFile(fopen(fileWithInput.c_str(), "r"), deleter);

    ASSERT_EQ(ParentRoutine(childPath, inFile.get()), 0);

    auto outFile1 = std::ifstream(fileWithOutput1);
    auto outFile2 = std::ifstream(fileWithOutput2);
    ASSERT_TRUE(outFile1.good() && outFile2.good());

    std::string result;

    for (const std::string &line : expectedOutput1) {
        std::getline(outFile1, result);
        EXPECT_EQ(result, line);
    }

    for (const std::string &line : expectedOutput2) {
        std::getline(outFile2, result);
        EXPECT_EQ(result, line);
    }

    auto removeIfExists = [](const std::string &path) {
        if (fs::exists(path)) {
            fs::remove(path);
        }
    };

    removeIfExists(fileWithInput);
    removeIfExists(fileWithOutput1);
    removeIfExists(fileWithOutput2);
}

TEST(FirstLabTests, ZeroOutputFileTest) {
    const char *childPath = getenv("PATH_TO_CHILD");
    ASSERT_TRUE(childPath);
    const std::string fileWithInput = "input.txt";

    {
        auto inFile = std::ofstream(fileWithInput);
    }
    auto deleter = [](FILE *file) {
        fclose(file);
    };

    std::unique_ptr<FILE, decltype(deleter)> inFile(fopen(fileWithInput.c_str(), "r"), deleter);

    ASSERT_EQ(ParentRoutine(childPath, inFile.get()), -1);
}

TEST(FirstLabTests, OneOutputFileTest) {
    const char *childPath = getenv("PATH_TO_CHILD");
    ASSERT_TRUE(childPath);
    const std::string fileWithInput = "input.txt";
    const std::string fileWithOutput = "output.txt";

    {
        auto inFile = std::ofstream(fileWithInput);
        inFile << fileWithOutput << '\n';
    }
    auto deleter = [](FILE *file) {
        fclose(file);
    };

    std::unique_ptr<FILE, decltype(deleter)> inFile(fopen(fileWithInput.c_str(), "r"), deleter);

    ASSERT_EQ(ParentRoutine(childPath, inFile.get()), -1);
    ASSERT_FALSE(fs::exists(fileWithOutput));
}

TEST(FirstLabTests, EmptyInputTest) {
    const char *childPath = getenv("PATH_TO_CHILD");
    ASSERT_TRUE(childPath);

    const std::string fileWithInput = "input.txt";
    const std::string fileWithOutput1 = "output1.txt";
    const std::string fileWithOutput2 = "output2.txt";

    {
        auto inFile = std::ofstream(fileWithInput);
        inFile << fileWithOutput1 << '\n'
               << fileWithOutput2 << '\n';
    }

    auto deleter = [](FILE *file) {
        fclose(file);
    };

    std::unique_ptr<FILE, decltype(deleter)> inFile(fopen(fileWithInput.c_str(), "r"), deleter);

    ASSERT_EQ(ParentRoutine(childPath, inFile.get()), 0);

    auto outFile1 = std::ifstream(fileWithOutput1);
    auto outFile2 = std::ifstream(fileWithOutput2);
    ASSERT_TRUE(outFile1.good() && outFile2.good());
    ASSERT_TRUE(fs::is_empty(fileWithOutput1) && fs::is_empty(fileWithOutput2));

    auto removeIfExists = [](const std::string &path) {
        if (fs::exists(path)) {
            fs::remove(path);
        }
    };

    removeIfExists(fileWithInput);
    removeIfExists(fileWithOutput1);
    removeIfExists(fileWithOutput2);
}