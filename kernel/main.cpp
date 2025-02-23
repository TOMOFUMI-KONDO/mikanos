// Copyright 2025 TOMOFUMI-KONDO.

#include <cstddef>

#include "./frame_buffer_config.hpp"
#include "./pixel_writer.hpp"

void *operator new(size_t size, void *buf) { return buf; }

void operator delete(void *obj) noexcept {}

void Halt(void) {
  while (1)
    __asm__("hlt");
}

extern "C" void KernelMain(const FrameBufferConfig &frame_buffer_config) {
  PixelWriter *pixel_writer;

  switch (frame_buffer_config.pixel_format) {
  case kPixelRGBResv8BitPerColor: {
    char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
    pixel_writer = new (pixel_writer_buf)
        RGBResv8BitPerColorPixelWriter{frame_buffer_config};
    break;
  }
  case kPixelBGRResv8BitPerColor: {
    char pixel_writer_buf[sizeof(BGRResv8BitPerColorPixelWriter)];
    pixel_writer = new (pixel_writer_buf)
        BGRResv8BitPerColorPixelWriter{frame_buffer_config};
    break;
  }
  default:
    Halt();
  }

  for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x) {
    for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y) {
      pixel_writer->Write(x, y, {255, 255, 255});
    }
  }

  for (int x = 0; x < 200; ++x) {
    for (int y = 0; y < 100; ++y) {
      pixel_writer->Write(100 + x, 100 + y, {0, 255, 0});
    }
  }

  Halt();
}
