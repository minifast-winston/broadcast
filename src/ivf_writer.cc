#include "ivf_writer.h"

void IVFWriter::PutLittleEndian16(uint8_t* mem, int val) {
  mem[0] = (val >> 0) & 0xff;
  mem[1] = (val >> 8) & 0xff;
}

void IVFWriter::PutLittleEndian32(uint8_t* mem, int val) {
  mem[0] = (val >> 0) & 0xff;
  mem[1] = (val >> 8) & 0xff;
  mem[2] = (val >> 16) & 0xff;
  mem[3] = (val >> 24) & 0xff;
}

uint32_t IVFWriter::WriteFileHeader(uint8_t* mem,
                                    const std::string& codec,
                                    int32_t width,
                                    int32_t height) {
  mem[0] = 'D';
  mem[1] = 'K';
  mem[2] = 'I';
  mem[3] = 'F';
  PutLittleEndian16(mem + 4, 0);                               // version
  PutLittleEndian16(mem + 6, 32);                              // header size
  PutLittleEndian32(mem + 8, fourcc(codec[0], codec[1], codec[2], '0'));  // fourcc
  PutLittleEndian16(mem + 12, static_cast<uint16_t>(width));   // width
  PutLittleEndian16(mem + 14, static_cast<uint16_t>(height));  // height
  PutLittleEndian32(mem + 16, 1000);                           // rate
  PutLittleEndian32(mem + 20, 1);                              // scale
  PutLittleEndian32(mem + 24, 0xffffffff);                     // length
  PutLittleEndian32(mem + 28, 0);                              // unused

  return 32;
}

uint32_t IVFWriter::WriteFrameHeader(uint8_t* mem,
                                     uint64_t pts,
                                     size_t frame_size) {
  PutLittleEndian32(mem, (int)frame_size);
  PutLittleEndian32(mem + 4, (int)(pts & 0xFFFFFFFF));
  PutLittleEndian32(mem + 8, (int)(pts >> 32));

  return 12;
}
