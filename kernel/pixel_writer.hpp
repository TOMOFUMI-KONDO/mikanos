// Copyright 2025 TOMOFUMI-KONDO.

#pragma once

#include <./frame_buffer_config.hpp>

struct PixelColor {
  uint8_t r, g, b;
};

class PixelWriter {
 public:
  explicit PixelWriter(const FrameBufferConfig &config) : config_{config} {}
  virtual ~PixelWriter() = default;
  virtual void Write(int x, int y, const PixelColor &c) = 0;

 protected:
  uint8_t *PixelAt(int x, int y);

 private:
  const FrameBufferConfig &config_;
};

class RGBResv8BitPerColorPixelWriter : public PixelWriter {
 public:
  using PixelWriter::PixelWriter;
  void Write(int x, int y, const PixelColor &c) override;
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
 public:
  using PixelWriter::PixelWriter;
  void Write(int x, int y, const PixelColor &c) override;
};
