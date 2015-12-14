#ifndef BROADCAST_BITSTREAM_WRITER_H
#define BROADCAST_BITSTREAM_WRITER_H

#include <string>
#include <vector>

#include "ppapi/cpp/size.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_codecs.h"
#include "ppapi/cpp/var_array_buffer.h"

class BitstreamWriter {
  std::vector<uint8_t> buffer_;
  bool first_frame_;
  uint64_t timestamp_;
  std::string codec_;
  pp::Size frame_size_;
  std::vector<uint8_t> bytes_;
public:
  explicit BitstreamWriter(PP_BitstreamBuffer *buffer,
                           bool first_frame,
                           uint64_t timestamp,
                           pp::Size frame_size) : first_frame_(first_frame),
                                                  timestamp_(timestamp),
                                                  frame_size_(frame_size),
                                                  bytes_() {
    codec_ = "VP8";
    uint8_t *byte_buffer = static_cast<uint8_t*>(buffer->buffer);
    buffer_ = std::vector<uint8_t>(byte_buffer, byte_buffer + buffer->size);
  }
  ~BitstreamWriter() {}

  uint32_t GetFileHeaderSize() const { return 32; }
  uint32_t GetFrameHeaderSize() const { return 12; }
  uint32_t GetSize();

  uint32_t WriteFileHeader();
  uint32_t WriteFrameHeader();

  std::vector<uint8_t> *GetBytes();
  void WriteTo(uint8_t *destination);

  void Push8(uint8_t value);
  void Push16(uint16_t value);
  void Push32(uint32_t value);
};

#endif
