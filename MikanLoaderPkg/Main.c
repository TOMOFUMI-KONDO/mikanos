// Copyright 2025 TOMOFUMI-KONDO.

#include <Uefi.h>

#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/LoadedImage.h>

#include "./frame_buffer_config.hpp"

struct MemoryMap {
  UINTN buffer_size;
  VOID *buffer;
  UINTN map_size;
  UINTN map_key;
  UINTN descriptor_size;
  UINT32 descriptor_version;
};

EFI_STATUS GetMemoryMap(struct MemoryMap *map) {
  if (map->buffer == NULL) {
    return EFI_BUFFER_TOO_SMALL;
  }

  map->map_size = map->buffer_size;
  return gBS->GetMemoryMap(&map->map_size, (EFI_MEMORY_DESCRIPTOR *)map->buffer,
                           &map->map_key, &map->descriptor_size,
                           &map->descriptor_version);
}

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root) {
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

  gBS->OpenProtocol(image_handle, &gEfiLoadedImageProtocolGuid,
                    (VOID **)&loaded_image, image_handle, NULL,
                    EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

  gBS->OpenProtocol(loaded_image->DeviceHandle,
                    &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs,
                    image_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

  fs->OpenVolume(fs, root);

  return EFI_SUCCESS;
}

EFI_STATUS OpenGOP(EFI_HANDLE image_handle,
                   EFI_GRAPHICS_OUTPUT_PROTOCOL **gop) {
  UINTN num_gop_handles = 0;
  EFI_HANDLE *gop_handles = NULL;
  gBS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL,
                          &num_gop_handles, &gop_handles);

  gBS->OpenProtocol(gop_handles[0], &gEfiGraphicsOutputProtocolGuid,
                    (VOID **)gop, image_handle, NULL,
                    EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

  FreePool(gop_handles);

  return EFI_SUCCESS;
}

const CHAR16 *GetPixelFormatUnicode(EFI_GRAPHICS_PIXEL_FORMAT fmt) {
  switch (fmt) {
  case PixelRedGreenBlueReserved8BitPerColor:
    return L"PixelRedGreenBlueReserved8BitPerColor";
  case PixelBlueGreenRedReserved8BitPerColor:
    return L"PixelBlueGreenRedReserved8BitPerColor";
  case PixelBitMask:
    return L"PixelBitMask";
  case PixelBltOnly:
    return L"PixelBltOnly";
  case PixelFormatMax:
    return L"PixelFormatMax";
  default:
    return L"InvalidPixelFormat";
  }
}

void Halt(void) {
  while (1)
    __asm__("hlt");
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE image_handle,
                           EFI_SYSTEM_TABLE *system_table) {
  Print(L"Hello, Mikan World!\n");

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  EFI_STATUS status = OpenGOP(image_handle, &gop);
  if (EFI_ERROR(status)) {
    Print(L"faield to open gop: %r\n", status);
    Halt();
  }

  EFI_FILE_PROTOCOL *root_dir;
  status = OpenRootDir(image_handle, &root_dir);
  if (EFI_ERROR(status)) {
    Print(L"failed to open root dir: %r\n", status);
    Halt();
  }

  CHAR16 *kernel_file_name = L"\\kernel.elf";
  Print(L"kernel_file_name len: %d\n", StrLen(kernel_file_name));
  EFI_FILE_PROTOCOL *kernel_file;
  status = root_dir->Open(root_dir, &kernel_file, kernel_file_name,
                          EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR(status)) {
    Print(L"failed to open kernel.elf: %r\n", status);
    Halt();
  }

  // 12 is filename size.
  UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
  UINT8 file_info_buffer[file_info_size];
  status = kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid, &file_info_size,
                                file_info_buffer);
  if (EFI_ERROR(status)) {
    Print(L"failed to get info of kernel file: %r\n", status);
    Halt();
  }
  EFI_FILE_INFO *file_info = (EFI_FILE_INFO *)file_info_buffer;
  UINTN kernel_file_size = file_info->FileSize;

  EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;
  status = gBS->AllocatePages(AllocateAddress, EfiLoaderData,
                              (kernel_file_size + 0xfff) / 0x1000,
                              &kernel_base_addr);
  if (EFI_ERROR(status)) {
    Print(L"failed to allocate pages for kernel: %r\n", status);
    Halt();
  }
  status = kernel_file->Read(kernel_file, &kernel_file_size,
                             (VOID *)kernel_base_addr);
  if (EFI_ERROR(status)) {
    Print(L"failed to load kernel file: %r\n", status);
    Halt();
  }
  Print(L"Kernel: 0x%0lx (%lu bytes)\n", kernel_base_addr, kernel_file_size);

  CHAR8 memmap_buf[4096 * 4];
  struct MemoryMap memmap = {sizeof(memmap_buf), memmap_buf, 0, 0, 0, 0};
  status = GetMemoryMap(&memmap);
  if (EFI_ERROR(status)) {
    Print(L"failed to get memory map: %r\n", status);
    Halt();
  }
  status = gBS->ExitBootServices(image_handle, memmap.map_key);
  if (EFI_ERROR(status)) {
    Print(L"failed to exit boot service: %r\n", status);
    Halt();
  }

  struct FrameBufferConfig config = {(UINT8 *)gop->Mode->FrameBufferBase,
                                     gop->Mode->Info->PixelsPerScanLine,
                                     gop->Mode->Info->HorizontalResolution,
                                     gop->Mode->Info->VerticalResolution, 0};
  switch (gop->Mode->Info->PixelFormat) {
  case PixelRedGreenBlueReserved8BitPerColor:
    config.pixel_format = kPixelRGBResv8BitPerColor;
    break;
  case PixelBlueGreenRedReserved8BitPerColor:
    config.pixel_format = kPixelBGRResv8BitPerColor;
    break;
  default:
    Print(L"Unimplemented pixel format: %d\n", gop->Mode->Info->PixelFormat);
    Halt();
  }

  UINT64 entry_addr = *(UINT64 *)(kernel_base_addr + 24);
  typedef void EntryPointType(const struct FrameBufferConfig *);
  ((EntryPointType *)entry_addr)(&config);

  Print(L"All done\n");

  while (1) {
  }
  return EFI_SUCCESS;
}
