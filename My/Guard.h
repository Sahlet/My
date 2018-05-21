//Guard.h
#pragma once

#ifndef GUARD_H
#define GUARD_H

#include <functional>

namespace My
{
  struct Guard
  {
    std::function<void()> m_onDestroy;

    template<typename F>
    Guard(F onDestroy)
      : m_onDestroy(onDestroy)
    {}

    ~Guard()
    {
      if (m_onDestroy)
      {
        m_onDestroy();
      }
    }

    inline void Release()
    {
      m_onDestroy = nullptr;
    }
  };
} // namespace My

#endif //GUARD_H