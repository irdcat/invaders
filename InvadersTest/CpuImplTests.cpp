#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "mocks/BusMock.hpp"

#include "..//Invaders/CpuImpl.hpp"

using testing::Eq;
using testing::Return;

class CpuImplTests : public testing::Test
{
    protected:
        void SetUp() override
        {
            bus = std::make_shared<BusMock>();
            testedCpu = std::make_unique<CpuImpl>(bus);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<BusMock> bus;
};

TEST_F(CpuImplTests, testFetchOpcode)
{
    auto& pc = testedCpu->getRegisters().getPc();
    pc = 0x100;
    const auto expectedResult = 0x77;
    EXPECT_CALL(*bus, readFromMemory(Eq(0x100))).Times(1).WillOnce(Return(expectedResult));

    auto result = testedCpu->fetchOpcode();
    EXPECT_EQ(expectedResult, result);
}