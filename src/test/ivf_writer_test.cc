#include "test_helper.h"
#include "ivf_writer.h"

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
