// Copyright 2025 TOMOFUMI-KONDO.

#pragma once

#include <pixel_writer.hpp>

#include "./frame_buffer_config.hpp"

uint8_t *PixelWriter::PixelAt(int x, int y) {
  return config_.frame_buffer + 4 * (config_.horizontal_resolution * y + x);
}

void RGBResv8BitPerColorPixelWriter::Write(int x, int y, const PixelColor &c) {
  auto p = PixelAt(x, y);
  p[0] = c.r;
  p[1] = c.g;
  p[2] = c.b;
}

void BGRResv8BitPerColorPixelWriter::Write(int x, int y, const PixelColor &c) {
  auto p = PixelAt(x, y);
  p[0] = c.b;
  p[1] = c.g;
  p[2] = c.r;
}
