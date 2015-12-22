#include "test_helper.h"
#include "event.h"

TEST(Event, Send) {
  Event<bool> ExtraterrestrialContact;
  bool WearingRubberPants = false;
  ExtraterrestrialContact.Listen([&](bool GotEvent){
    WearingRubberPants = GotEvent;
  });
  ExtraterrestrialContact.Send(i % 2 == 0);
  EXPECT_EQ(WearingRubberPants, true);
}

TEST(Event, SendAsync) {
  Event<bool> ExtraterrestrialContact;
  bool WearingRubberPants = false;
  ExtraterrestrialContact.Listen([&](bool GotEvent){
    WearingRubberPants = GotEvent;
  });
  ExtraterrestrialContact.SendAsync(i % 2 == 0).wait();
  EXPECT_EQ(WearingRubberPants, true);
}
