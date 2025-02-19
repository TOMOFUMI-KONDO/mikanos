// Copyright 2025 TOMOFUMI-KONDO.

extern "C" void KernelMain() {
  while (1) {
    __asm__("hlt");
  }
}
