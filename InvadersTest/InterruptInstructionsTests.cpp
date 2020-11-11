#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../Invaders/CpuImpl.hpp"

class InterruptInstructionsTests : public testing::Test
{
    protected:
        void SetUp() override
        {
            testedCpu = std::make_unique<CpuImpl>(nullptr);
        }

        std::unique_ptr<CpuImpl> testedCpu;

        const u8 DISABLE_INTERRUPTS_OPCODE = 0xF3;
        const u8 ENABLE_INTERRUPTS_OPCODE = 0xFB;
};

TEST_F(InterruptInstructionsTests, testDisableInterrupts)
{
    testedCpu->executeInstruction(DISABLE_INTERRUPTS_OPCODE);
    EXPECT_FALSE(testedCpu->interruptsEnabled());
}

TEST_F(InterruptInstructionsTests, testEnableInterrupts)
{
    testedCpu->executeInstruction(ENABLE_INTERRUPTS_OPCODE);
    EXPECT_TRUE(testedCpu->interruptsEnabled());
}