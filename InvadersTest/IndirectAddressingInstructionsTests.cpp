#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "mocks/BusMock.hpp"

#include "..//Invaders/CpuImpl.hpp"

using testing::Eq;
using testing::Return;

class IndirectAddressingInstructionsTests : public testing::Test
{
    protected:
        void SetUp() override
        {
            bus = std::make_shared<BusMock>();
            testedCpu = std::make_unique<CpuImpl>(bus);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<BusMock> bus;

        const u8 STORE_ACCUMULATOR_EXTENDED_BC_OPCODE = 0x02;
        const u8 STORE_ACCUMULATOR_EXTENDED_DE_OPCODE = 0x12;
        const u8 LOAD_ACCUMULATOR_EXTENDED_BC_OPCODE = 0x0A;
        const u8 LOAD_ACCUMULATOR_EXTENDED_DE_OPCODE = 0x1A;
};

TEST_F(IndirectAddressingInstructionsTests, testStoreAccumulatorBc)
{
    const auto ADDR = 0x3400;
    const auto VALUE = 0x79;

    auto& regs = testedCpu->getRegisters();
    auto& bc = regs.getBc();
    auto& a = regs.getAf().getHigh();

    bc = ADDR;
    a = VALUE;

    EXPECT_CALL(*bus, writeIntoMemory(Eq(ADDR), Eq(VALUE)))
        .Times(1);

    testedCpu->executeInstruction(STORE_ACCUMULATOR_EXTENDED_BC_OPCODE);
}

TEST_F(IndirectAddressingInstructionsTests, testStoreAccumulatorDe)
{
    const auto ADDR = 0x2300;
    const auto VALUE = 0x34;

    auto& regs = testedCpu->getRegisters();
    auto& de = regs.getDe();
    auto& a = regs.getAf().getHigh();

    de = ADDR;
    a = VALUE;

    EXPECT_CALL(*bus, writeIntoMemory(Eq(ADDR), Eq(VALUE)))
        .Times(1);

    testedCpu->executeInstruction(STORE_ACCUMULATOR_EXTENDED_DE_OPCODE);
}

TEST_F(IndirectAddressingInstructionsTests, testLoadAccumulatorBc)
{
    const auto ADDR = 0x1300;
    const auto VALUE = 0x99;

    auto& regs = testedCpu->getRegisters();
    auto& bc = regs.getBc();
    auto& a = regs.getAf().getHigh();

    bc = ADDR;
    a = 0;

    EXPECT_CALL(*bus, readFromMemory(Eq(ADDR)))
        .Times(1)
        .WillOnce(Return(VALUE));

    testedCpu->executeInstruction(LOAD_ACCUMULATOR_EXTENDED_BC_OPCODE);

    EXPECT_EQ(VALUE, a);
}

TEST_F(IndirectAddressingInstructionsTests, testLoadAccumulatorDe)
{
    const auto ADDR = 0x1000;
    const auto VALUE = 0x72;

    auto& regs = testedCpu->getRegisters();
    auto& de = regs.getDe();
    auto& a = regs.getAf().getHigh();

    de = ADDR;
    a = 0;

    EXPECT_CALL(*bus, readFromMemory(Eq(ADDR)))
        .Times(1)
        .WillOnce(Return(VALUE));

    testedCpu->executeInstruction(LOAD_ACCUMULATOR_EXTENDED_DE_OPCODE);

    EXPECT_EQ(VALUE, a);
}