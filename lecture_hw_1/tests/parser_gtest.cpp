#include <cctype>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "lexer.hpp"
#include "parser.hpp"

namespace {

struct ParserCase {
    std::string filePath;
    bool expectedResult;
};

struct InlineParserCase {
    std::string name;
    std::string input;
    bool expectedResult;
};

std::string readInput(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Cannot open test input file: " + path.string());
    }

    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    return content;
}

std::string testNameFromPath(const std::string& filePath) {
    std::string name = std::filesystem::path(filePath).stem().string();
    for (char& c : name) {
        if (!std::isalnum(static_cast<unsigned char>(c))) {
            c = '_';
        }
    }
    if (name.empty()) {
        return "UnnamedCase";
    }
    return name;
}

class ParserTxtCasesTest : public ::testing::TestWithParam<ParserCase> {};
class ParserInlineCasesTest : public ::testing::TestWithParam<InlineParserCase> {};

TEST_P(ParserTxtCasesTest, MatchesExpectedParseResult) {
    const ParserCase& testCase = GetParam();
    const std::filesystem::path filePath = std::filesystem::path(HW1_SOURCE_DIR) / testCase.filePath;

    const std::string input = readInput(filePath);
    Lexer lexer(input);
    Parser parser(lexer);

    EXPECT_EQ(parser.parse(false), testCase.expectedResult) << "Input file: " << filePath.string();
}

INSTANTIATE_TEST_SUITE_P(
    ExistingTxtCases,
    ParserTxtCasesTest,
    ::testing::Values(
        ParserCase{"tests/test_valid.txt", true},
        ParserCase{"tests/test_valid_precedence.txt", true},
        ParserCase{"tests/test_invalid.txt", false},
        ParserCase{"tests/test_invalid_parentheses.txt", false},
        ParserCase{"tests/test_invalid_identifier.txt", false}),
    [](const ::testing::TestParamInfo<ParserCase>& info) {
        return testNameFromPath(info.param.filePath);
    });

TEST_P(ParserInlineCasesTest, MatchesExpectedParseResult) {
    const InlineParserCase& testCase = GetParam();

    Lexer lexer(testCase.input);
    Parser parser(lexer);

    EXPECT_EQ(parser.parse(false), testCase.expectedResult) << "Input: '" << testCase.input << "'";
}

INSTANTIATE_TEST_SUITE_P(
    EdgeCases,
    ParserInlineCasesTest,
    ::testing::Values(
        InlineParserCase{"SingleIdentifier", "a", true},
        InlineParserCase{"NestedParentheses", "((a))", true},
        InlineParserCase{"DanglingPlus", "a+", false},
        InlineParserCase{"OnlyOperator", "+", false},
        InlineParserCase{"EmptyInput", "", false}),
    [](const ::testing::TestParamInfo<InlineParserCase>& info) {
        return info.param.name;
    });

}  // namespace
