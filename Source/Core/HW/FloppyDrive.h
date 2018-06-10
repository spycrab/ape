#pragma once
//! \file

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Common/Types.h"

namespace Core::HW
{
//! Representation of a floppy drive
class FloppyDrive
{
public:
  //! Insert an image into the drive
  bool Insert(const std::string& path);

  //! Check if a disc is present
  bool HasDisc() const;

  //! Check if the provided image is bootable
  bool IsBootable();

  //! Eject the image
  void Eject();

  //! Read data from the image
  bool Read(u32 offset, u32 size, u8* buffer);

private:
  std::unique_ptr<std::ifstream> m_file;
};
}
