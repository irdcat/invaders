#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "mocks/BusMock.hpp"

#include "..//Invaders/CpuImpl.hpp"

using testing::Eq;
using testing::Return;

class JumpInstructionsTests : public testing::Test
{
    protected:
        void SetUp() override
        {
            bus = std::make_shared<BusMock>();
            testedCpu = std::make_unique<CpuImpl>(bus);
        }

        // Mocks 16 bit immedate value in memory
        void mockImmedate16(u16 addr, u16 value)
        {
            EXPECT_CALL(*bus, readFromMemory(Eq(addr))).WillRepeatedly(Return(value & 0xFF));
            EXPECT_CALL(*bus, readFromMemory(Eq(addr + 1))).WillRepeatedly(Return((value >> 8) & 0xFF));
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<BusMock> bus;

        const u8 LOAD_PC_FROM_HL_OPCODE = 0xE9;
        const u8 UNCONDITIONAL_JUMP_OPCODE = 0xC3;
        const u8 JUMP_NOT_ZERO_OPCODE = 0xC2;
        const u8 JUMP_ZERO_OPCODE = 0xCA;
        const u8 JUMP_NOT_CARRY_OPCODE = 0xD2;
        const u8 JUMP_CARRY_OPCODE = 0xDA;
        const u8 JUMP_PARITY_ODD_OPCODE = 0xE2;
        const u8 JUMP_PARITY_EVEN_OPCODE = 0xEA;
        const u8 JUMP_PLUS_OPCODE = 0xF2;
        const u8 JUMP_MINUS_OPCODE = 0xFA;
};

TEST_F(JumpInstructionsTests, testLoadPcFromHl)
{
    auto& regs = testedCpu->getRegisters();
    auto& hl = regs.getHl();
    auto& pc = regs.getPc();

    hl = 0x3400;
    pc = 0x0000;

    testedCpu->executeInstruction(LOAD_PC_FROM_HL_OPCODE);

    EXPECT_EQ(0x3400, pc);
}

TEST_F(JumpInstructionsTests, testUnconditionalJump)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& pc = regs.getPc();

    pc = 0x101;

    mockImmedate16(0x101, ADDR);

    testedCpu->executeInstruction(UNCONDITIONAL_JUMP_OPCODE);

    EXPECT_EQ(ADDR, pc);
}

TEST_F(JumpInstructionsTests, testJumpNotZero)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& flags = regs.getAf().getLow();
    auto& pc = regs.getPc();

    pc = 0x101;
    flags.Z = 1;
    testedCpu->executeInstruction(JUMP_NOT_ZERO_OPCODE);
    EXPECT_EQ(0x103, pc);

    pc = 0x101;
    flags.Z = 0;
    mockImmedate16(0x101, ADDR);
    testedCpu->executeInstruction(JUMP_NOT_ZERO_OPCODE);
    EXPECT_EQ(ADDR, pc);
}

TEST_F(JumpInstructionsTests, testJumpZero)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& flags = regs.getAf().getLow();
    auto& pc = regs.getPc();

    pc = 0x101;
    flags.Z = 0;
    testedCpu->executeInstruction(JUMP_ZERO_OPCODE);
    EXPECT_EQ(0x103, pc);

    pc = 0x101;
    flags.Z = 1;
    mockImmedate16(0x101, ADDR);
    testedCpu->executeInstruction(JUMP_ZERO_OPCODE);
    EXPECT_EQ(ADDR, pc);
}

TEST_F(JumpInstructionsTests, testJumpNotCarry)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& flags = regs.getAf().getLow();
    auto& pc = regs.getPc();

    pc = 0x101;
    flags.C = 1;
    testedCpu->executeInstruction(JUMP_NOT_CARRY_OPCODE);
    EXPECT_EQ(0x103, pc);

    pc = 0x101;
    flags.C = 0;
    mockImmedate16(0x101, ADDR);
    testedCpu->executeInstruction(JUMP_NOT_CARRY_OPCODE);
    EXPECT_EQ(ADDR, pc);
}

TEST_F(JumpInstructionsTests, testJumpCarry)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& flags = regs.getAf().getLow();
    auto& pc = regs.getPc();

    pc = 0x101;
    flags.C = 0;
    testedCpu->executeInstruction(JUMP_CARRY_OPCODE);
    EXPECT_EQ(0x103, pc);

    pc = 0x101;
    flags.C = 1;
    mockImmedate16(0x101, ADDR);
    testedCpu->executeInstruction(JUMP_CARRY_OPCODE);
    EXPECT_EQ(ADDR, pc);
}

TEST_F(JumpInstructionsTests, testJumpParityOdd)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& flags = regs.getAf().getLow();
    auto& pc = regs.getPc();

    pc = 0x101;
    flags.P = 1;
    testedCpu->executeInstruction(JUMP_PARITY_ODD_OPCODE);
    EXPECT_EQ(0x103, pc);

    pc = 0x101;
    flags.P = 0;
    mockImmedate16(0x101, ADDR);
    testedCpu->executeInstruction(JUMP_PARITY_ODD_OPCODE);
    EXPECT_EQ(ADDR, pc);
}

TEST_F(JumpInstructionsTests, testJumpParityEven)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& flags = regs.getAf().getLow();
    auto& pc = regs.getPc();

    pc = 0x101;
    flags.P = 0;
    testedCpu->executeInstruction(JUMP_PARITY_EVEN_OPCODE);
    EXPECT_EQ(0x103, pc);

    pc = 0x101;
    flags.P = 1;
    mockImmedate16(0x101, ADDR);
    testedCpu->executeInstruction(JUMP_PARITY_EVEN_OPCODE);
    EXPECT_EQ(ADDR, pc);
}

TEST_F(JumpInstructionsTests, testJumpPlus)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& flags = regs.getAf().getLow();
    auto& pc = regs.getPc();

    pc = 0x101;
    flags.S = 1;
    testedCpu->executeInstruction(JUMP_PLUS_OPCODE);
    EXPECT_EQ(0x103, pc);

    pc = 0x101;
    flags.S = 0;
    mockImmedate16(0x101, ADDR);
    testedCpu->executeInstruction(JUMP_PLUS_OPCODE);
    EXPECT_EQ(ADDR, pc);
}

TEST_F(JumpInstructionsTests, testJumpMinus)
{
    const u16 ADDR = 0x9888;
    auto& regs = testedCpu->getRegisters();
    auto& flags = regs.getAf().getLow();
    auto& pc = regs.getPc();

    pc = 0x101;
    flags.S = 0;
    testedCpu->executeInstruction(JUMP_MINUS_OPCODE);
    EXPECT_EQ(0x103, pc);

    pc = 0x101;
    flags.S = 1;
    mockImmedate16(0x101, ADDR);
    testedCpu->executeInstruction(JUMP_MINUS_OPCODE);
    EXPECT_EQ(ADDR, pc);
}