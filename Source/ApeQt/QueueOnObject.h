// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

template <typename T, typename F> static void QueueOnObject(T* obj, F&& func)
{
  QObject src;
  QObject::connect(&src, &QObject::destroyed, obj, std::forward<F>(func),
                   Qt::QueuedConnection);
}
