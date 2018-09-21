#pragma once
//! \file

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Common/Types.h"

//! Representation of a floppy drive
namespace Core::HW::FloppyDrive
{
//! Insert an image into the drive
bool Insert(const std::string& path);

//! Check if a disc is present
bool HasDisc();

//! Get the size of the inserted disc
u32 GetSize();

//! Check if the provided image is bootable
bool IsBootable();

//! Get size of a floppy
u32 GetSectorSize();

//! Get sectors per track
u32 GetSectorsPerTrack();

//! Get head count
u32 GetHeadCount();

//! Eject the image
void Eject();

//! Read data from the image
bool Read(u32 offset, u32 size, u8* buffer);
bool Read(u8 cylinder, u8 head, u8 sector, u8 count, u8* buffer);

bool GuessFormat();
} // namespace Core::HW::FloppyDrive
