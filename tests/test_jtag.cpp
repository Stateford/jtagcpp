#include "gtest/gtest.h"
#include "jtag.h"


TEST(TestJtag, BasicAssertions) {
    JTAG jtag;

    EXPECT_EQ(jtag.getCurrentState(), JTAGState::TLR);

    auto result = jtag.move(JTAGState::SELECT_IR_SCAN);
    ASSERT_TRUE(result);
    ASSERT_EQ(result->bits, 0b011);
    ASSERT_EQ(result->size, 3);

    result = jtag.move(JTAGState::TLR);
    ASSERT_TRUE(result);
    ASSERT_EQ(result->bits, 0b01);
    ASSERT_EQ(result->size, 1);

    result = jtag.move(JTAGState::SHIFT_DR);
    ASSERT_TRUE(result);
    ASSERT_EQ(result->bits, 0b0100);
    ASSERT_EQ(result->size, 4);

    result = jtag.move(JTAGState::SHIFT_IR);
    ASSERT_TRUE(result);
    ASSERT_EQ(result->bits, 0b11'1100);
    ASSERT_EQ(result->size, 6);
}
