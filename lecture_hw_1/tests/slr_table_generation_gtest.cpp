#include <gtest/gtest.h>

#include "slr_table.hpp"
#include "slr_tables_generated.hpp"

namespace {

TEST(SlrTableGenerationTest, GeneratedActionTableMatchesRuntimeTable) {
    for (int state = 0; state < kNumStates; ++state) {
        for (int term = 0; term < kNumTerminals; ++term) {
            const Action expected = GENERATED_ACTION_TABLE[state][term];
            const Action actual = ACTION_TABLE[state][term];
            EXPECT_EQ(actual.type, expected.type) << "state=" << state << ", term=" << term;
            EXPECT_EQ(actual.value, expected.value) << "state=" << state << ", term=" << term;
        }
    }
}

TEST(SlrTableGenerationTest, GeneratedGotoTableMatchesRuntimeTable) {
    for (int state = 0; state < kNumStates; ++state) {
        for (int nonterm = 0; nonterm < kNumNonTerminals; ++nonterm) {
            EXPECT_EQ(GOTO_TABLE[state][nonterm], GENERATED_GOTO_TABLE[state][nonterm])
                << "state=" << state << ", nonterm=" << nonterm;
        }
    }
}

}  // namespace
