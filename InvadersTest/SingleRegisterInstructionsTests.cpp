#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "mocks/BusMock.hpp"

#include "..//Invaders/CpuImpl.hpp"

using testing::Eq;
using testing::ReturnRef;

class SingleRegisterInstructionsTests : public testing::Test
{
    protected:
        void SetUp() override
        {
            bus = std::make_shared<BusMock>();
            testedCpu = std::make_unique<CpuImpl>(bus);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<BusMock> bus;

        const u8 DECIMAL_ADJUST_ACCUMULATOR_OPCODE = 0x27;
        const u8 COMPLEMENT_ACCUMULATOR_OPCODE = 0x2F;

        const u8 INCREMENT_B_OPCODE = 0x04;
        const u8 INCREMENT_C_OPCODE = 0x0C;
        const u8 INCREMENT_D_OPCODE = 0x14;
        const u8 INCREMENT_E_OPCODE = 0x1C;
        const u8 INCREMENT_H_OPCODE = 0x24;
        const u8 INCREMENT_L_OPCODE = 0x2C;
        const u8 INCREMENT_MEM_OPCODE = 0x34;
        const u8 INCREMENT_A_OPCODE = 0x3C;

        const u8 DECREMENT_B_OPCODE = 0x05;
        const u8 DECREMENT_C_OPCODE = 0x0D;
        const u8 DECREMENT_D_OPCODE = 0x15;
        const u8 DECREMENT_E_OPCODE = 0x1D;
        const u8 DECREMENT_H_OPCODE = 0x25;
        const u8 DECREMENT_L_OPCODE = 0x2D;
        const u8 DECREMENT_MEM_OPCODE = 0x35;
        const u8 DECREMENT_A_OPCODE = 0x3D;
};

TEST_F(SingleRegisterInstructionsTests, testDecimalAdjustAccumulator)
{
    auto& af = testedCpu->getRegisters().getAf();
    auto& accumulator = af.getHigh();
    auto& flags = af.getLow();

    accumulator = 0x9B;
    flags.raw = 0;

    testedCpu->executeInstruction(DECIMAL_ADJUST_ACCUMULATOR_OPCODE);

    EXPECT_EQ(0x01, accumulator);
    EXPECT_EQ(1, flags.AC);
    EXPECT_EQ(1, flags.C);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testComplementAccumulator)
{
    auto& accumulator = testedCpu->getRegisters().getAf().getHigh();

    accumulator = 0x51;

    testedCpu->executeInstruction(COMPLEMENT_ACCUMULATOR_OPCODE);

    EXPECT_EQ(0xAE, accumulator);
}

TEST_F(SingleRegisterInstructionsTests, testIncrementB)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& b = testedCpu->getRegisters().getBc().getHigh();

    b = 0x8F;
    flags.raw = 0;

    testedCpu->executeInstruction(INCREMENT_B_OPCODE);

    EXPECT_EQ(0x90, b);
    EXPECT_EQ(1, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(1, flags.S);
    EXPECT_EQ(1, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testIncrementC)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& c = testedCpu->getRegisters().getBc().getLow();

    c = 0xFF;
    flags.raw = 0;

    testedCpu->executeInstruction(INCREMENT_C_OPCODE);

    EXPECT_EQ(0x00, c);
    EXPECT_EQ(1, flags.AC);
    EXPECT_EQ(1, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(1, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testIncrementD)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& d = testedCpu->getRegisters().getDe().getHigh();

    d = 0x03;
    flags.raw = 0;

    testedCpu->executeInstruction(INCREMENT_D_OPCODE);

    EXPECT_EQ(0x04, d);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testIncrementE)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& e = testedCpu->getRegisters().getDe().getLow();

    e = 0x14;
    flags.raw = 0;

    testedCpu->executeInstruction(INCREMENT_E_OPCODE);

    EXPECT_EQ(0x15, e);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testIncrementH)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& h = testedCpu->getRegisters().getHl().getHigh();

    h = 0x14;
    flags.raw = 0;

    testedCpu->executeInstruction(INCREMENT_H_OPCODE);

    EXPECT_EQ(0x15, h);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testIncrementL)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& l = testedCpu->getRegisters().getHl().getLow();

    l = 0x14;
    flags.raw = 0;

    testedCpu->executeInstruction(INCREMENT_L_OPCODE);

    EXPECT_EQ(0x15, l);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testIncrementMem)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& hl = testedCpu->getRegisters().getHl();
    
    u8 memoryLocation = 0x79;
    flags.raw = 0;
    hl = 0x3400;

    EXPECT_CALL(*bus, getMemoryLocationRef(Eq(0x3400)))
        .Times(1)
        .WillOnce(ReturnRef(memoryLocation));

    testedCpu->executeInstruction(INCREMENT_MEM_OPCODE);

    EXPECT_EQ(0x7A, memoryLocation);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testIncrementA)
{
    auto& af = testedCpu->getRegisters().getAf();
    auto& flags = af.getLow();
    auto& a = af.getHigh();

    flags.raw = 0;
    a = 0x66;

    testedCpu->executeInstruction(INCREMENT_A_OPCODE);

    EXPECT_EQ(0x67, a);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testDecrementB)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& b = testedCpu->getRegisters().getBc().getHigh();

    flags.raw = 0;
    b = 0x00;

    testedCpu->executeInstruction(DECREMENT_B_OPCODE);

    EXPECT_EQ(0xFF, b);
    EXPECT_EQ(1, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(1, flags.S);
    EXPECT_EQ(1, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testDecrementC)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& c = testedCpu->getRegisters().getBc().getLow();

    flags.raw = 0;
    c = 0x13;

    testedCpu->executeInstruction(DECREMENT_C_OPCODE);

    EXPECT_EQ(0x12, c);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(1, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testDecrementD)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& d = testedCpu->getRegisters().getDe().getHigh();

    flags.raw = 0;
    d = 0x00;

    testedCpu->executeInstruction(DECREMENT_D_OPCODE);

    EXPECT_EQ(0xFF, d);
    EXPECT_EQ(1, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(1, flags.S);
    EXPECT_EQ(1, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testDecrementE)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& e = testedCpu->getRegisters().getDe().getLow();

    flags.raw = 0;
    e = 0x13;

    testedCpu->executeInstruction(DECREMENT_E_OPCODE);

    EXPECT_EQ(0x12, e);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(1, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testDecrementH)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& h = testedCpu->getRegisters().getHl().getHigh();

    flags.raw = 0;
    h = 0x89;

    testedCpu->executeInstruction(DECREMENT_H_OPCODE);

    EXPECT_EQ(0x88, h);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(1, flags.S);
    EXPECT_EQ(1, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testDecrementL)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& l = testedCpu->getRegisters().getHl().getLow();

    flags.raw = 0;
    l = 0x24;

    testedCpu->executeInstruction(DECREMENT_L_OPCODE);

    EXPECT_EQ(0x23, l);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testDecrementMem)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    auto& hl = testedCpu->getRegisters().getHl();

    u8 memoryLocation = 0x87;
    flags.raw = 0;
    hl = 0x3400;

    EXPECT_CALL(*bus, getMemoryLocationRef(Eq(0x3400)))
        .Times(1)
        .WillOnce(ReturnRef(memoryLocation));

    testedCpu->executeInstruction(DECREMENT_MEM_OPCODE);

    EXPECT_EQ(0x86, memoryLocation);
    EXPECT_EQ(0, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(1, flags.S);
    EXPECT_EQ(0, flags.P);
}

TEST_F(SingleRegisterInstructionsTests, testDecrementA)
{
    auto& af = testedCpu->getRegisters().getAf();
    auto& flags = af.getLow();
    auto& a = af.getHigh();

    flags.raw = 0;
    a = 0xF0;

    testedCpu->executeInstruction(DECREMENT_A_OPCODE);

    EXPECT_EQ(0xEF, a);
    EXPECT_EQ(1, flags.AC);
    EXPECT_EQ(0, flags.Z);
    EXPECT_EQ(0, flags.S);
    EXPECT_EQ(0, flags.P);
}