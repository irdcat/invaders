#pragma once

#include <memory>

#include "Cpu.hpp"
#include "Bus.hpp"

enum class Condition
{
    NOT_ZERO = 0,
    ZERO = 1,
    NOT_CARRY = 2,
    CARRY = 3,
    ODD = 4,
    EVEN = 5,
    POSITIVE = 6,
    MINUS = 7
};

class CpuImpl : public Cpu
{
    public:
        CpuImpl(const std::shared_ptr<Bus>& busPtr);

        ~CpuImpl() = default;

        u8 fetchOpcode() override;

        Registers& getRegisters() override;

        void executeInstruction(u8 opcode) override;

    private:
        void executeFirstGroupInstruction(u8 y, u8 z, u8 p, u8 q);
        void executeSecondGroupInstruction(u8 y, u8 z);
        void executeThirdGroupInstruction(u8 y, u8 z);
        void executeFourthGroupInstruction(u8 y, u8 z, u8 p, u8 q);

        u8& resolveRegister(unsigned index);
        u16& resolveRegisterPair1(unsigned index);
        u16& resolveRegisterPair2(unsigned index);
        u16& resolveRegisterPair(unsigned index, unsigned table);
        Condition resolveCondition(unsigned index);
        bool evaluateCondition(Condition c);
        u16 fetchImmedate16();
        u8 fetchImmedate8();

        // Instructions
        void nop();
        void lxi(u16& reg16, u16 immedate);
        void dad(u16& reg16);
        void stax(u16& reg16);
        void shld(u16 addr);
        void sta(u16 addr);
        void ldax(u16& reg16);
        void lhld(u16 addr);
        void lda(u16 addr);
        void inx(u16& reg16);
        void dcx(u16& reg16);
        void inr(u8& reg8);
        void dcr(u8& reg8);
        void mvi(u8& reg8, u8 immedate);
        void rlc();
        void rrc();
        void ral();
        void rar();
        void daa();
        void cma();
        void stc();
        void cmc();
        void mov(u8& dest, u8& src);
        void halt();
        void add(u8& src);
        void adc(u8& src);
        void sub(u8& src);
        void sbc(u8& src);
        void ana(u8& src);
        void xra(u8& src);
        void ora(u8& src);
        void cmp(u8& src);
        void ret(Condition c);
        void pop(u16& reg16);
        void ret();
        void pchl();
        void sphl();
        void jmp(Condition c, u16 addr);
        void jmp(u16 addr);
        void out(u8 port);
        void xthl();
        void di();
        void in(u8 port);
        void xchg();
        void ei();
        void call(Condition c, u16 addr);
        void push(u16& reg);
        void call(u16 addr);
        void adi(u8 immedate);
        void aci(u8 immedate);
        void sui(u8 immedate);
        void sbi(u8 immedate);
        void ani(u8 immedate);
        void xri(u8 immedate);
        void ori(u8 immedate);
        void cpi(u8 immedate);
        void rst(u8 vector);
            
        bool interrupt_enable;
        bool halted;
        Registers registers;
        std::shared_ptr<Bus> bus;
};