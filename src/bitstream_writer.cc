#include "bitstream_writer.h"

#define fourcc(a, b, c, d)                                               \
  (((uint32_t)(a) << 0) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | \
   ((uint32_t)(d) << 24))

std::vector<uint8_t> *BitstreamWriter::GetBytes() {
  return &bytes_;
}

void BitstreamWriter::Push8(uint8_t val) {
  bytes_.push_back(val & 0xff);
}

void BitstreamWriter::Push16(uint16_t val) {
  Push8(val);
  Push8(val >> 8);
}

void BitstreamWriter::Push32(uint32_t val) {
  Push16(val);
  Push16(val >> 16);
}

uint32_t BitstreamWriter::WriteFrameHeader() {
  Push32(static_cast<uint32_t>(buffer_.size()));
  Push32(static_cast<uint32_t>(timestamp_ & 0xFFFFFFFF));
  Push32(static_cast<uint32_t>(timestamp_ >> 32));

  return 12;
}

uint32_t BitstreamWriter::WriteFileHeader() {
  if (!first_frame_) { return 0; }
  Push8('D');
  Push8('K');
  Push8('I');
  Push8('F');
  Push16(0);                                          // version
  Push16(32);                                         // header size
  Push32(fourcc(codec_[0], codec_[1], codec_[2], '0'));
  Push16(static_cast<uint16_t>(frame_size_.width()));
  Push16(static_cast<uint16_t>(frame_size_.height()));
  Push32(1000);                                       // rate
  Push32(1);                                          // scale
  Push32(0xffffffff);                                 // length
  Push32(0);                                          // unused

  return 32;
}

uint32_t BitstreamWriter::GetSize() {
  uint32_t size = buffer_.size() + GetFrameHeaderSize();
  if (first_frame_) {
    size += GetFileHeaderSize();
  }
  return size;
}

void BitstreamWriter::WriteTo(uint8_t *data_ptr) {
  WriteFileHeader();
  WriteFrameHeader();
  bytes_.insert(bytes_.end(), buffer_.begin(), buffer_.end());
  std::copy(bytes_.begin(), bytes_.end(), data_ptr);
}
