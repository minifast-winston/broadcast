#include "test_helper.h"

TEST(Timecop, GetTime) {
  Timecop::ResetTime();
  EXPECT_EQ(Timecop::GetTime(), 0.0);
  Timecop::AdvanceTime(0.1);
  EXPECT_EQ(Timecop::GetTime(), 0.1);
  Timecop::AdvanceTime(0.1);
  EXPECT_EQ(Timecop::GetTime(), 0.2);
  Timecop::RewindTime(0.1);
  EXPECT_EQ(Timecop::GetTime(), 0.1);
  Timecop::ResetTime();
  EXPECT_EQ(Timecop::GetTime(), 0.0);
}
