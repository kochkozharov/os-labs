#include <gtest/gtest.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <memory>

namespace fs = std::filesystem;

extern "C" {
#include "lab1.h"
}

TEST(FirstLabTests, SimpleTest) {
    const std::string fileWithInput = "input.txt";
    const std::string fileWithOutput1 = "output1.txt";
    const std::string fileWithOutput2 = "output2.txt";

    constexpr int outputSize1 = 5;
    constexpr int outputSize2 = 3;
    constexpr int inputSize = outputSize1 + outputSize2;

    std::array<std::string, inputSize> input = {
        "rev",
        "revvvvv",
        "162te16782r18223r2",
        "",
        "r",
        "pqwrpqlwfpqwoglqwpglqpgwpqw.,g;q.wg;q.wg;w.qg;.w",
        "12345678900",
        "1234567890",
    };

    std::array<std::string, outputSize1> expectedOutput1 = {
        "ver",
        "vvvvver",
        "",
        "r",
        "0987654321",
    };

    std::array<std::string, outputSize2> expectedOutput2 = {
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

    ASSERT_TRUE(ParentRoutine(getenv("PATH_TO_CHILD"), inFile.get()) == 0);
    
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