#include "BusImpl.hpp"

u8 BusImpl::readFromMemory(u16 addr) const
{
    return u8();
}

void BusImpl::writeIntoMemory(u16 addr, u8 value)
{
}

u8 BusImpl::readFromInputPort(u8 port) const
{
    return u8();
}

void BusImpl::writeIntoOutputPort(u8 port, u8 value)
{
}

u8& BusImpl::getMemoryLocationRef(u16 addr)
{
    // TODO: tu wstawi� instrukcj� return
}
