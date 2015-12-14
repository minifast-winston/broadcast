#include "test_helper.h"
Timecop *Timecop::instance_ = 0; // initialize timecop instance

#include "ppapi/cpp/var.h"
#include "ppapi_simple/ps_main.h"
#include "ppapi/cpp/instance.h"

#if defined(WIN32)
#include <Windows.h>
#undef PostMessage
#endif

int example_main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

PPAPI_SIMPLE_REGISTER_MAIN(example_main);
