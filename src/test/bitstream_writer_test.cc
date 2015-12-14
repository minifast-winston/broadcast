#include "test_helper.h"
#include "bitstream_writer.h"

TEST(BitstreamWriter, GetFileHeaderSize) {
  PP_BitstreamBuffer buffer;
  buffer.buffer = {};
  buffer.size = 0;
  pp::Size frame_size(1, 1);
  BitstreamWriter writer = BitstreamWriter(&buffer, false, 0, frame_size);
  EXPECT_EQ(writer.GetFileHeaderSize(), 32);
}

TEST(BitstreamWriter, GetFrameHeaderSize) {
  PP_BitstreamBuffer buffer;
  buffer.buffer = {};
  buffer.size = 0;
  pp::Size frame_size(1, 1);
  BitstreamWriter writer = BitstreamWriter(&buffer, false, 0, frame_size);
  EXPECT_EQ(writer.GetFrameHeaderSize(), 12);
}

TEST(BitstreamWriter, GetEmptyBitstream) {
  PP_BitstreamBuffer buffer;
  buffer.buffer = {};
  buffer.size = 0;
  pp::Size frame_size(1, 1);
  BitstreamWriter writer = BitstreamWriter(&buffer, false, 0, frame_size);
  EXPECT_EQ(writer.GetBytes()->size(), 0);
}

TEST(BitstreamWriter, GetBitstreamWithAByte) {
  PP_BitstreamBuffer buffer;
  buffer.buffer = {};
  buffer.size = 0;
  pp::Size frame_size(1, 1);
  BitstreamWriter writer = BitstreamWriter(&buffer, false, 0, frame_size);
  writer.Push8(0x23);
  EXPECT_EQ(writer.GetBytes()->size(), 1);
  EXPECT_EQ(writer.GetBytes()->at(0), 0x23);
}

TEST(BitstreamWriter, GetBitstreamWithTwoBytes) {
  PP_BitstreamBuffer buffer;
  buffer.buffer = {};
  buffer.size = 0;
  pp::Size frame_size(1, 1);
  BitstreamWriter writer = BitstreamWriter(&buffer, false, 0, frame_size);
  writer.Push16(0x1234);
  EXPECT_EQ(writer.GetBytes()->size(), 2);
  EXPECT_EQ(writer.GetBytes()->at(0), 0x34);
  EXPECT_EQ(writer.GetBytes()->at(1), 0x12);
}

TEST(BitstreamWriter, GetBitstreamWithFourBytes) {
  PP_BitstreamBuffer buffer;
  buffer.buffer = {};
  buffer.size = 0;
  pp::Size frame_size(1, 1);
  BitstreamWriter writer = BitstreamWriter(&buffer, false, 0, frame_size);
  writer.Push32(0x12345678);
  EXPECT_EQ(writer.GetBytes()->size(), 4);
  EXPECT_EQ(writer.GetBytes()->at(0), 0x78);
  EXPECT_EQ(writer.GetBytes()->at(1), 0x56);
  EXPECT_EQ(writer.GetBytes()->at(2), 0x34);
  EXPECT_EQ(writer.GetBytes()->at(3), 0x12);
}

TEST(BitstreamWriter, WriteFrameHeader) {
  PP_BitstreamBuffer buffer;
  buffer.size = 0x1234;
  pp::Size frame_size(1, 1);
  BitstreamWriter writer = BitstreamWriter(&buffer, false, 0x123456789, frame_size);
  EXPECT_EQ(writer.WriteFrameHeader(), 12);

  EXPECT_EQ(writer.GetBytes()->size(), 12); // wrote all bytes in the frame

  EXPECT_EQ(writer.GetBytes()->at(0), 0x34); // buffer length
  EXPECT_EQ(writer.GetBytes()->at(1), 0x12);
  EXPECT_EQ(writer.GetBytes()->at(2), 0x00);
  EXPECT_EQ(writer.GetBytes()->at(3), 0x00);

  EXPECT_EQ(writer.GetBytes()->at(4),  0x89); // timestamp
  EXPECT_EQ(writer.GetBytes()->at(5),  0x67);
  EXPECT_EQ(writer.GetBytes()->at(6),  0x45);
  EXPECT_EQ(writer.GetBytes()->at(7),  0x23);
  EXPECT_EQ(writer.GetBytes()->at(8),  0x01);
  EXPECT_EQ(writer.GetBytes()->at(9),  0x00);
  EXPECT_EQ(writer.GetBytes()->at(10), 0x00);
  EXPECT_EQ(writer.GetBytes()->at(11), 0x00);
}

TEST(BitstreamWriter, WriteFileHeaderWhenFrameIsNotTheFirst) {
  PP_BitstreamBuffer buffer;
  buffer.buffer = {};
  buffer.size = 0;
  pp::Size frame_size(1, 1);
  BitstreamWriter writer = BitstreamWriter(&buffer, false, 0, frame_size);
  EXPECT_EQ(writer.WriteFileHeader(), 0);
  EXPECT_EQ(writer.GetBytes()->size(), 0); // did not write anything
}

TEST(BitstreamWriter, WriteFileHeaderWhenFrameIsTheFirst) {
  PP_BitstreamBuffer buffer;
  buffer.buffer = {};
  buffer.size = 0;
  pp::Size frame_size(0x1234, 0x4321);
  BitstreamWriter writer = BitstreamWriter(&buffer, true, 0, frame_size);
  EXPECT_EQ(writer.WriteFileHeader(), 32);

  EXPECT_EQ(writer.GetBytes()->size(), 32); // wrote all bytes in the header

  EXPECT_EQ(writer.GetBytes()->at(8), 'V'); // codec
  EXPECT_EQ(writer.GetBytes()->at(9), 'P');
  EXPECT_EQ(writer.GetBytes()->at(10), '8');
  EXPECT_EQ(writer.GetBytes()->at(11), '0');

  EXPECT_EQ(writer.GetBytes()->at(12), 0x34); //width
  EXPECT_EQ(writer.GetBytes()->at(13), 0x12);

  EXPECT_EQ(writer.GetBytes()->at(14), 0x21); //height
  EXPECT_EQ(writer.GetBytes()->at(15), 0x43);
}
