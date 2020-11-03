#pragma once

#include "Types.hpp"

class Bus
{
    public:
        virtual ~Bus() = default;

        virtual u8 readFromMemory(u16 addr) const;

        virtual void writeIntoMemory(u16 addr, u8 value);

        virtual u8 readFromInputPort(u8 port) const;

        virtual void writeIntoOutputPort(u8 port, u8 value);

        virtual u8& getMemoryLocationRef(u16 addr);
};