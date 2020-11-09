#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "..//Invaders/CpuImpl.hpp"

class CarryBitInstructionsTests : public testing::Test
{
    protected:
        void SetUp() override
        {
            testedCpu = std::make_unique<CpuImpl>(nullptr);
        }

        std::unique_ptr<CpuImpl> testedCpu;

        const u8 SET_CARRY_OPCODE = 0x37;
        const u8 COMPLEMENT_CARRY_OPCODE = 0x3F;
};

TEST_F(CarryBitInstructionsTests, testSetCarry)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    flags.C = 0;

    testedCpu->executeInstruction(SET_CARRY_OPCODE);
    EXPECT_EQ(1, flags.C);

    testedCpu->executeInstruction(SET_CARRY_OPCODE);
    EXPECT_EQ(1, flags.C);
}

TEST_F(CarryBitInstructionsTests, testComplementCarry)
{
    auto& flags = testedCpu->getRegisters().getAf().getLow();
    flags.C = 0;

    testedCpu->executeInstruction(COMPLEMENT_CARRY_OPCODE);
    EXPECT_EQ(1, flags.C);

    testedCpu->executeInstruction(COMPLEMENT_CARRY_OPCODE);
    EXPECT_EQ(0, flags.C);
}