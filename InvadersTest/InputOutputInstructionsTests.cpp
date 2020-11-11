#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "mocks/BusMock.hpp"

#include "..//Invaders/CpuImpl.hpp"

using testing::Eq;
using testing::Return;

class InputOutputInstructionsTests : public testing::Test
{
    protected:
        void SetUp() override
        {
            bus = std::make_shared<BusMock>();
            testedCpu = std::make_unique<CpuImpl>(bus);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<BusMock> bus;

        const u8 OUTPUT_INSTRUCTION = 0xD3;
        const u8 INPUT_INSTRUCTION = 0xDB;
};

TEST_F(InputOutputInstructionsTests, testOutput)
{
    const u8 PORT = 0x01;
    auto& regs = testedCpu->getRegisters();
    auto& pc = regs.getPc();
    auto& a = regs.getAf().getHigh();

    pc = 0x101;
    a = 0x74;

    EXPECT_CALL(*bus, readFromMemory(Eq(0x101)))
        .Times(1)
        .WillOnce(Return(PORT));
    EXPECT_CALL(*bus, writeIntoOutputPort(Eq(PORT), Eq(0x74)))
        .Times(1);

    testedCpu->executeInstruction(OUTPUT_INSTRUCTION);
}

TEST_F(InputOutputInstructionsTests, testInput)
{
    const u8 PORT = 0x01;
    auto& regs = testedCpu->getRegisters();
    auto& pc = regs.getPc();
    auto& a = regs.getAf().getHigh();

    pc = 0x101;
    a = 0x00;

    EXPECT_CALL(*bus, readFromMemory(Eq(0x101)))
        .Times(1)
        .WillOnce(Return(PORT));
    EXPECT_CALL(*bus, readFromInputPort(Eq(PORT)))
        .Times(1)
        .WillOnce(Return(0x74));

    testedCpu->executeInstruction(INPUT_INSTRUCTION);

    EXPECT_EQ(0x74, a);
}