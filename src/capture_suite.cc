// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdlib.h>

#include "gtest/gtest.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/var.h"
#include "ppapi_simple/ps_main.h"

#include "ivf_writer.h"
#include "remote_control.h"
#include "frame_advancer.h"

#if defined(WIN32)
#include <Windows.h>
#undef PostMessage
#endif

TEST(IVFWriter, GetFileHeaderSize) {
  IVFWriter writer = IVFWriter();
  EXPECT_EQ(writer.GetFileHeaderSize(), 32);
}

TEST(IVFWriter, GetFrameHeaderSize) {
  IVFWriter writer = IVFWriter();
  EXPECT_EQ(writer.GetFrameHeaderSize(), 12);
}

TEST(IVFWriter, PutLittleEndian16) {
  IVFWriter writer = IVFWriter();
  uint8_t *mem = (uint8_t *)malloc(sizeof(uint8_t) * 2);
  writer.PutLittleEndian16(mem, 0x1234);
  EXPECT_EQ(mem[0], 0x34);
  EXPECT_EQ(mem[1], 0x12);
}

TEST(IVFWriter, PutLittleEndian32) {
  IVFWriter writer = IVFWriter();
  uint8_t *mem = (uint8_t *)malloc(sizeof(uint8_t) * 4);
  writer.PutLittleEndian32(mem, 0x12345678);
  EXPECT_EQ(mem[0], 0x78);
  EXPECT_EQ(mem[1], 0x56);
  EXPECT_EQ(mem[2], 0x34);
  EXPECT_EQ(mem[3], 0x12);
}

TEST(IVFWriter, WriteFrameHeader) {
  IVFWriter writer = IVFWriter();
  uint8_t *mem = (uint8_t *)malloc(sizeof(uint8_t) * 12);
  EXPECT_EQ(writer.WriteFrameHeader(mem, 0x123456789, 0x1), 12);

  EXPECT_EQ(mem[0], 0x01); // frame size
  EXPECT_EQ(mem[1], 0x00);
  EXPECT_EQ(mem[2], 0x00);
  EXPECT_EQ(mem[3], 0x00);

  EXPECT_EQ(mem[4],  0x89); // pts
  EXPECT_EQ(mem[5],  0x67);
  EXPECT_EQ(mem[6],  0x45);
  EXPECT_EQ(mem[7],  0x23);
  EXPECT_EQ(mem[8],  0x01);
  EXPECT_EQ(mem[9],  0x00);
  EXPECT_EQ(mem[10], 0x00);
  EXPECT_EQ(mem[11], 0x00);
}

TEST(IVFWriter, WriteFileHeader) {
  IVFWriter writer = IVFWriter();
  uint8_t *mem = (uint8_t *)malloc(sizeof(uint8_t) * 12);
  EXPECT_EQ(writer.WriteFileHeader(mem, "MoV", 0x1234, 0x4321), 32);

  EXPECT_EQ(mem[8], 'M'); // codec
  EXPECT_EQ(mem[9], 'o');
  EXPECT_EQ(mem[10], 'V');
  EXPECT_EQ(mem[11], '0');

  EXPECT_EQ(mem[12], 0x34); // width
  EXPECT_EQ(mem[13], 0x12);

  EXPECT_EQ(mem[14], 0x21); // height
  EXPECT_EQ(mem[15], 0x43);
}

TEST(RemoteControl, IsRunning) {
  RemoteControl control = RemoteControl();
  EXPECT_EQ(control.IsRunning(), false);

  control.SetEncoding(true);
  control.SetPaused(false);
  EXPECT_EQ(control.IsRunning(), true);
}

class TestListener: public RemoteControlObserver {
  bool called_;
  bool value_;
public:
  TestListener(): called_(false) {}
  ~TestListener() {}
  bool GetCalled() { return called_; }
  bool GetValue() { return value_; }

  void OnEncodingChange(bool encoding) { called_ = true; value_ = encoding; };
  void OnPausedChange(bool paused) { called_ = true; value_ = paused; };
};

TEST(RemoteControl, OnChangePaused) {
  TestListener listener = TestListener();
  RemoteControl control = RemoteControl();
  control.OnChange(&listener);
  control.SetPaused(false);
  EXPECT_EQ(listener.GetCalled(), true);
}

TEST(RemoteControl, OnChangeEncoding) {
  TestListener listener = TestListener();
  RemoteControl control = RemoteControl();
  control.OnChange(&listener);
  control.SetEncoding(false);
  EXPECT_EQ(listener.GetCalled(), true);
}

PP_Time Timecop() {
  return 0.0;
}

TEST(FrameAdvancer, GetNextFrameTime) {
  PPB_Core core;
  core.GetTime = Timecop;
  FrameAdvancer advancer(25, &core);
  EXPECT_EQ(advancer.GetNextFrameTime(), 40);
}



class GTestEventListener : public ::testing::EmptyTestEventListener {
 public:
  // TestEventListener overrides.
  virtual void OnTestStart(const ::testing::TestInfo& test_info) {
    std::stringstream msg;
    msg << "start:" << test_info.test_case_name() << "." << test_info.name();
    pp::Instance(PSGetInstanceId()).PostMessage(msg.str());
  }

  virtual void OnTestPartResult(
      const ::testing::TestPartResult& test_part_result) {
    if (test_part_result.failed()) {
      std::stringstream msg;
      msg << "fail:" << test_part_result.file_name() << ","
          << test_part_result.line_number() << ","
          << test_part_result.summary();
      pp::Instance(PSGetInstanceId()).PostMessage(msg.str());
    }
  }

  virtual void OnTestEnd(const ::testing::TestInfo& test_info) {
    std::stringstream msg;
    msg << "end:" << test_info.test_case_name() << "." << test_info.name()
        << "," << (test_info.result()->Failed() ? "failed" : "ok");
    pp::Instance(PSGetInstanceId()).PostMessage(msg.str());
  }
};

int example_main(int argc, char* argv[]) {
  setenv("TERM", "xterm-256color", 0);
  ::testing::InitGoogleTest(&argc, argv);
  if (PSGetInstanceId() != 0) {
    ::testing::UnitTest::GetInstance()->listeners()
        .Append(new GTestEventListener());
  }
  return RUN_ALL_TESTS();
}

// Register the function to call once the Instance Object is initialized.
// see: pappi_simple/ps_main.h
PPAPI_SIMPLE_REGISTER_MAIN(example_main);
