#include "ppapi/c/pp_stdint.h"
#include <string>

#define fourcc(a, b, c, d)                                               \
  (((uint32_t)(a) << 0) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | \
   ((uint32_t)(d) << 24))

// IVF container writer. It is possible to parse H264 bitstream using
// NAL units but for VP8 we need a container to at least find encoded
// pictures as well as the picture sizes.
class IVFWriter {
 public:
  IVFWriter() {}
  ~IVFWriter() {}

  uint32_t GetFileHeaderSize() const { return 32; }
  uint32_t GetFrameHeaderSize() const { return 12; }
  uint32_t WriteFileHeader(uint8_t* mem,
                           const std::string& codec,
                           int32_t width,
                           int32_t height);
  uint32_t WriteFrameHeader(uint8_t* mem, uint64_t pts, size_t frame_size);
  void PutLittleEndian16(uint8_t *mem, int val);
  void PutLittleEndian32(uint8_t *mem, int val);
};
