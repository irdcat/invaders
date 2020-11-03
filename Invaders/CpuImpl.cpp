#include "CpuImpl.hpp"

CpuImpl::CpuImpl(const std::shared_ptr<Bus>& busPtr)
    : registers()
    , bus(busPtr)
    , interrupt_enable(false)
    , halted(false)
{
}

u8 CpuImpl::fetchOpcode()
{
    u16& pc = registers.getPc();
    u8 opcode = bus->readFromMemory(pc);
    pc += 2;
    return opcode;
}

Registers& CpuImpl::getRegisters()
{
    return registers;
}

void CpuImpl::executeInstruction(u8 opcode)
{
    // Extracting bitfields from opcode in following format
    // 
    // |xx|yy y|zzz|
    // |  |pp|q|   |
    
    const auto x = (opcode >> 6) & 3;
    const auto y = (opcode >> 3) & 7;
    const auto z = opcode & 7;
    const auto p = opcode >> 1;
    const auto q = opcode % 2;

    switch (x)
    {
        case 0:
            executeFirstGroupInstruction(y, z, p, q);
        case 1:
            executeSecondGroupInstruction(y, z);
        case 2:
            executeThirdGroupInstruction(y, z);
        default:
            executeFourthGroupInstruction(y, z, p, q);
    }
}

void CpuImpl::executeFirstGroupInstruction(u8 y, u8 z, u8 p, u8 q)
{
    // x == 0
    if (z == 0)
    {
        nop();
    }
    else if (z == 1)
    {
        u16& reg = resolveRegisterPair1(p);
        if (q == 0)
        {
            u16 immedate = fetchImmedate16();
            lxi(reg, immedate);
        }
        else
        {
            dad(reg);
        }
    }
    else if (z == 2)
    {
        u16& reg = resolveRegisterPair1(p);
        if (p < 2)
        {
            if (q == 0)
                stax(reg);
            else
                ldax(reg);
        }
        else if (p == 2)
        {
            u16 addr = fetchImmedate16();
            if (q == 0)
                shld(addr);
            else
                lhld(addr);
        }
        else
        {
            u16 addr = fetchImmedate16();
            if (q == 0)
                sta(addr);
            else
                lda(addr);
        }
    }
    else if (z == 3)
    {
        u16& reg = resolveRegisterPair1(p);
        if (q == 0)
            inx(reg);
        else
            dcx(reg);
    }
    else if (z == 4)
    {
        u8& reg = resolveRegister(y);
        inr(reg);
    }
    else if (z == 5)
    {
        u8& reg = resolveRegister(y);
        dcr(reg);
    }
    else if (z == 6)
    {
        u8& reg = resolveRegister(y);
        u8 immedate = fetchImmedate8();
        mvi(reg, immedate);
    }
    else // z == 7
    {
        if (y == 0)
            rlc();
        else if (y == 1)
            rrc();
        else if (y == 2)
            ral();
        else if (y == 3)
            rar();
        else if (y == 4)
            daa();
        else if (y == 5)
            cma();
        else if (y == 6)
            stc();
        else if (y == 7)
            cmc();
    }
}

void CpuImpl::executeSecondGroupInstruction(u8 y, u8 z)
{
    // x == 1
    if (z == 6 && y == 6)
    {
        halt();
    }
    else
    {
        u8& destination = resolveRegister(y);
        u8& source = resolveRegister(z);
        mov(destination, source);
    }
}

void CpuImpl::executeThirdGroupInstruction(u8 y, u8 z)
{
    // x == 2
    u8& reg = resolveRegister(z);

    if (y == 0)
        add(reg);
    else if (y == 1)
        adc(reg);
    else if (y == 2)
        sub(reg);
    else if (y == 3)
        sbc(reg);
    else if (y == 4)
        ana(reg);
    else if (y == 5)
        xra(reg);
    else if (y == 6)
        ora(reg);
    else
        cmp(reg);
}

void CpuImpl::executeFourthGroupInstruction(u8 y, u8 z, u8 p, u8 q)
{
    // x == 3
    if (z == 0)
    {
        Condition condition = resolveCondition(y);
        ret(condition);
    }
    else if (z == 1)
    {
        if (q == 0)
        {
            u16& reg = resolveRegisterPair2(p);
            pop(reg);
        }
        else
        {
            if (p < 2)
                ret();
            else if (p == 2)
                pchl();
            else
                sphl();
        }
    }
    else if (z == 2)
    {
        Condition condition = resolveCondition(y);
        u16 addr = fetchImmedate16();
        jmp(condition, addr);
    }
    else if (z == 3)
    {
        if (p == 0)
        {
            u16 addr = fetchImmedate16();
            jmp(addr);
        }
        else if (p == 1)
        {
            u8 port = fetchImmedate8();
            if (q == 0)
                out(port);
            else
                in(port);
        }
        else if (p == 2)
        {
            if (q == 0)
                xthl();
            else
                xchg();
        }
        else if (p == 3)
        {
            if (q == 0)
                di();
            else
                ei();
        }
    }
    else if (z == 4)
    {
        Condition condition = resolveCondition(y);
        u16 addr = fetchImmedate16();
        call(condition, addr);
    }
    else if (z == 5)
    {
        if (q == 0)
        {
            u16& reg = resolveRegisterPair2(p);
            push(reg);
        }
        else
        {
            u16 addr = fetchImmedate16();
            call(addr);
        }
    }
    else if (z == 6)
    {
        u8 immedate = fetchImmedate8();
        if (y == 0)
            adi(immedate);
        else if (y == 1)
            aci(immedate);
        else if (y == 2)
            sui(immedate);
        else if (y == 3)
            sbi(immedate);
        else if (y == 4)
            ani(immedate);
        else if (y == 5)
            xri(immedate);
        else if (y == 6)
            ori(immedate);
        else
            cpi(immedate);
    }
    else
    {
        rst(y * 8);
    }
}

u8& CpuImpl::resolveRegister(unsigned index)
{
    // Register table:
    // B, C, D, E, H, L, (HL), A

    auto& af = registers.getAf();
    auto& bc = registers.getBc();
    auto& de = registers.getDe();
    auto& hl = registers.getHl();

    switch (index)
    {
        case 0: 
            return bc.getHigh();
        case 1: 
            return bc.getLow();
        case 2:
            return de.getHigh();
        case 3:
            return de.getLow();
        case 4: 
            return hl.getHigh();
        case 5: 
            return hl.getLow();
        case 6: 
            return bus->getMemoryLocationRef(hl.getRaw());
        default:
            return af.getHigh();
    }
}

u16& CpuImpl::resolveRegisterPair1(unsigned index)
{
    // Register pair table 1:
    // BC, DE, HL, SP

    return resolveRegisterPair(index, 1);
}

u16& CpuImpl::resolveRegisterPair2(unsigned index)
{
    // Register pair table 2:
    // BC, DE, HL, AF

    return resolveRegisterPair(index, 2);
}

u16& CpuImpl::resolveRegisterPair(unsigned index, unsigned table)
{
    switch (index)
    {
        case 0:
            return registers.getBc().getRaw();
        case 1:
            return registers.getDe().getRaw();
        case 2:
            return registers.getHl().getRaw();
        case 3:
            return table == 1 ? registers.getSp() : registers.getAf().getRaw();
    }
}

Condition CpuImpl::resolveCondition(unsigned index)
{
    if (index > 7)
    {
        index = 7;
    }

    return static_cast<Condition>(index);
}

bool CpuImpl::evaluateCondition(Condition c)
{
    const auto& flags = registers.getAf().getLow();

    switch (c)
    {
        case Condition::NOT_ZERO:
            return !flags.Z;
        case Condition::ZERO:
            return flags.Z;
        case Condition::NOT_CARRY:
            return !flags.C;
        case Condition::CARRY:
            return flags.C;
        case Condition::ODD:
            return !flags.P;
        case Condition::EVEN:
            return flags.P;
        case Condition::POSITIVE:
            return !flags.S;
        case Condition::MINUS:
            return flags.S;
    }
}

u16 CpuImpl::fetchImmedate16()
{
    auto& pc = registers.getPc();
    u8 lsb = fetchImmedate8();
    u8 msb = fetchImmedate8();

    u16 immedate = lsb & (msb << 8);
    return immedate;
}

u8 CpuImpl::fetchImmedate8()
{
    auto& pc = registers.getPc();
    u8 immedate = bus->readFromMemory(pc);
    pc += 2;
    return immedate;
}

void CpuImpl::nop()
{
    // NOP - No Operation
    // Left as a method for possible future implementation of CPU clock
}

void CpuImpl::lxi(u16& reg, u16 immedate)
{
    // LXI - Load Extended Immedate
    reg = immedate;
}

void CpuImpl::dad(u16& reg)
{
    // DAD - Double Add
    auto& hl = registers.getHl().getRaw();
    auto& flags = registers.getAf().getLow();
    unsigned result = hl + reg;
    flags.C = result >> 16;
    hl = result & 0xFFFF;
}

void CpuImpl::stax(u16& reg)
{
    // STAX - Store Accumulator Extended
    u16 addr = reg;
    auto& accumulator = registers.getAf().getHigh();
    bus->writeIntoMemory(addr, accumulator);
}

void CpuImpl::shld(u16 addr)
{
    // SHLD - Store HL Direct
    auto& hl = registers.getHl();
    bus->writeIntoMemory(addr, hl.getLow());
    bus->writeIntoMemory(addr, hl.getHigh());
}

void CpuImpl::sta(u16 addr)
{
    // STA - Store Accumulator
    auto& accumulator = registers.getAf().getHigh();
    bus->writeIntoMemory(addr, accumulator);
}

void CpuImpl::ldax(u16& reg)
{
    // LDAX - Load Accumulator Extended
    u16 addr = reg;
    auto& accumulator = registers.getAf().getHigh();
    accumulator = bus->readFromMemory(addr);
}

void CpuImpl::lhld(u16 addr)
{
    // LHLD - Load HL Direct
    auto& h = registers.getHl().getHigh();
    auto& l = registers.getHl().getLow();
    l = bus->readFromMemory(addr);
    h = bus->readFromMemory(addr + 1);
}

void CpuImpl::lda(u16 addr)
{
    // LDA - Load Accumulator
    auto& accumulator = registers.getAf().getHigh();
    accumulator = bus->readFromMemory(addr);
}

void CpuImpl::inx(u16& reg)
{
    // INX - Increment Extended
    reg++;
}

void CpuImpl::dcx(u16& reg)
{
    // DCX - Decrement Extended
    reg--;
}

void CpuImpl::inr(u8& reg)
{
    // INR - Increment
    reg++;
}

void CpuImpl::dcr(u8& reg)
{
    // DCR - Decrement
    reg--;
}

void CpuImpl::mvi(u8& reg, u8 immedate)
{
    // MVI - Move Immedate
    reg = immedate;
}

void CpuImpl::rlc()
{
    // RLC - Rotate Left with Carry

}

void CpuImpl::rrc()
{
    // RRC - Rotate Right with Carry

}

void CpuImpl::ral()
{
    // RAL - Rotate Accumulator Left

}

void CpuImpl::rar()
{
    // RAR - Rotate Accumulator Right

}

void CpuImpl::daa()
{
    // DAA - Decimal Adjust Accumulator

}

void CpuImpl::cma()
{
    // CMA - Complement Accumulator

}

void CpuImpl::stc()
{
    // STC - Set Carry

}

void CpuImpl::cmc()
{
    // CMC - Complement Carry

}

void CpuImpl::mov(u8& destination, u8& source)
{
    // MOV - Move register

}

void CpuImpl::halt()
{
    // HALT - Halt
    halted = true;
}

void CpuImpl::add(u8& src)
{
    // ADD - Add to accumulator

}

void CpuImpl::adc(u8& src)
{
    // ADC - Add with Carry

}

void CpuImpl::sub(u8& src)
{
    // SUB - Subtract from accumulator

}

void CpuImpl::sbc(u8& src)
{
    // SBC - Subtract with Carry

}

void CpuImpl::ana(u8& src)
{
    // ANA - And Accumulator

}

void CpuImpl::xra(u8& src)
{
    // XRA - Xor Accumulator

}

void CpuImpl::ora(u8& src)
{
    // ORA - Or Accumulator

}

void CpuImpl::cmp(u8& src)
{
    // CMP - Compare with accumulator

}

void CpuImpl::ret(Condition c)
{
    // R cc[y] - Conditional Return

}

void CpuImpl::ret()
{
    // RET - Return

}

void CpuImpl::pop(u16& reg)
{
    // POP - Pop from stack

}

void CpuImpl::push(u16& reg)
{
    // PUSH - Push into stack

}

void CpuImpl::pchl()
{
    // PCHL - Load PC from HL

}

void CpuImpl::sphl()
{
    // SPHL - Load SP from HL

}

void CpuImpl::jmp(Condition c, u16 addr)
{
    // J cc[y] - Conditional Jump

}

void CpuImpl::jmp(u16 addr)
{
    // JMP - Jump

}

void CpuImpl::out(u8 port)
{
    // OUT - Write to output port

}

void CpuImpl::in(u8 port)
{
    // IN - Read from input port

}

void CpuImpl::ei()
{
    // EI - Enable Interrupts

}

void CpuImpl::di()
{
    // DI - Disable Interrupts

}

void CpuImpl::xthl()
{
    // XTHL - Exchange HL with memory contents pointed by SP

}

void CpuImpl::xchg()
{
    // XCHG - Exchange HL with contents of DE

}

void CpuImpl::call(Condition c, u16 addr)
{
    // C cc[y] - Conditional Call

}

void CpuImpl::call(u16 addr)
{
    // CALL - Call

}

void CpuImpl::adi(u8 immedate)
{
    // ADI - Add Immedate

}

void CpuImpl::aci(u8 immedate)
{
    // ACI - Add Immedate with Carry

}

void CpuImpl::sui(u8 immedate)
{
    // SUI - Subtract Immedate

}

void CpuImpl::sbi(u8 immedate)
{
    // SBI - Subtract Immedate with Borrow

}

void CpuImpl::ani(u8 immedate)
{
    // ANI - And Immedate

}

void CpuImpl::xri(u8 immedate)
{
    // XRI - Xor Immedate

}

void CpuImpl::ori(u8 immedate)
{
    // ORI - Or Immedate

}

void CpuImpl::cpi(u8 immedate)
{
    // CPI - Compare Immedate

}

void CpuImpl::rst(u8 vector)
{
    // RST - Reset (jump to reset vector)

}