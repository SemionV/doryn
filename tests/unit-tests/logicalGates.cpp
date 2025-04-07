#include <gtest/gtest.h>
#include <gmock/gmock.h>

bool switchGate(const bool inputA, const bool inputB)
{
    bool output = false;

    if(inputB)
    {
        output = inputA;
    }
    else
    {
        output = false;
    }

    return output;
}

bool inverseSwitchGate(const bool inputA, const bool inputB)
{
    bool output = false;

    if(inputB)
    {
        output = false;
    }
    else
    {
        output = inputA;
    }

    return output;
}

bool andGate(const bool inputA, const bool inputB, const bool inputC)
{
    bool output = false;

    output = switchGate(inputA, inputB);
    output = switchGate(output, inputC);

    return output;
}

bool orGate(const bool inputA, const bool inputB, const bool inputC)
{
    bool output = false;

    bool outputB = switchGate(inputA, inputB);
    bool outputC = switchGate(inputA, inputC);

    if(outputB)
    {
        output = outputB;
    }
    else
    {
        output = outputC;
    }

    return output;
}

bool xorGate(const bool inputA, const bool inputB, const bool inputC)
{
    bool output = false;

    bool iB = inverseSwitchGate(inputA, inputB);
    bool iC = inverseSwitchGate(inputA, inputC);

    bool b = andGate(inputA, inputB, iC);
    bool c = andGate(inputA, inputC, iB);

    output = orGate(inputA, b, c);

    return output;
}

bool halfAdderGate(const bool inputA, const bool inputB, const bool inputC, bool& carryOut)
{
    bool output = false;

    output = xorGate(inputA, inputB, inputC);
    carryOut = andGate(inputA, inputB, inputC);

    return output;
}

bool fullAdderGate(const bool carryIn, const bool inputA, const bool inputB, const bool inputC, bool& carryOut)
{
    bool output = false;

    bool a = xorGate(inputA, inputB, inputC);
    output = xorGate(inputA, carryIn, a);

    bool b = andGate(inputA, inputB, inputC);
    bool c = andGate(inputA, carryIn, a);

    carryOut = orGate(inputA, b, c);

    return output;
}

TEST(LogicalGates, switchGate)
{
    bool output = switchGate(true, true);
    EXPECT_TRUE(output);

    output = switchGate(true, false);
    EXPECT_FALSE(output);

    output = switchGate(false, true);
    EXPECT_FALSE(output);

    output = switchGate(false, false);
    EXPECT_FALSE(output);
}

TEST(LogicalGates, inverseSwitchGate)
{
    bool output = inverseSwitchGate(true, true);
    EXPECT_FALSE(output);

    output = inverseSwitchGate(true, false);
    EXPECT_TRUE(output);

    output = inverseSwitchGate(false, true);
    EXPECT_FALSE(output);

    output = inverseSwitchGate(false, false);
    EXPECT_FALSE(output);
}

TEST(LogicalGates, andGate)
{
    bool output = andGate(true, true, true);
    EXPECT_TRUE(output);

    output = andGate(true, false, true);
    EXPECT_FALSE(output);

    output = andGate(true, true, false);
    EXPECT_FALSE(output);

    output = andGate(true, false, false);
    EXPECT_FALSE(output);

    output = andGate(false, true, true);
    EXPECT_FALSE(output);

    output = andGate(false, false, true);
    EXPECT_FALSE(output);

    output = andGate(false, true, false);
    EXPECT_FALSE(output);

    output = andGate(false, false, false);
    EXPECT_FALSE(output);
}

TEST(LogicalGates, orGate)
{
    bool output = orGate(true, true, true);
    EXPECT_TRUE(output);

    output = orGate(true, false, true);
    EXPECT_TRUE(output);

    output = orGate(true, true, false);
    EXPECT_TRUE(output);

    output = orGate(true, false, false);
    EXPECT_FALSE(output);

    output = orGate(false, true, true);
    EXPECT_FALSE(output);

    output = orGate(false, false, true);
    EXPECT_FALSE(output);

    output = orGate(false, true, false);
    EXPECT_FALSE(output);

    output = orGate(false, false, false);
    EXPECT_FALSE(output);
}

TEST(LogicalGates, xorGate)
{
    bool output = xorGate(true, true, true);
    EXPECT_FALSE(output);

    output = xorGate(true, false, true);
    EXPECT_TRUE(output);

    output = xorGate(true, true, false);
    EXPECT_TRUE(output);

    output = xorGate(true, false, false);
    EXPECT_FALSE(output);

    output = xorGate(false, true, true);
    EXPECT_FALSE(output);

    output = xorGate(false, false, true);
    EXPECT_FALSE(output);

    output = xorGate(false, true, false);
    EXPECT_FALSE(output);

    output = xorGate(false, false, false);
    EXPECT_FALSE(output);
}

TEST(LogicalGates, halfAdderGate)
{
    bool carryOut = false;

    bool output = halfAdderGate(true, true, true, carryOut);
    EXPECT_FALSE(output);
    EXPECT_TRUE(carryOut);

    output = halfAdderGate(true, false, true, carryOut);
    EXPECT_TRUE(output);
    EXPECT_FALSE(carryOut);

    output = halfAdderGate(true, true, false, carryOut);
    EXPECT_TRUE(output);
    EXPECT_FALSE(carryOut);

    output = halfAdderGate(true, false, false, carryOut);
    EXPECT_FALSE(output);
    EXPECT_FALSE(carryOut);

    output = halfAdderGate(false, true, true, carryOut);
    EXPECT_FALSE(output);
    EXPECT_FALSE(carryOut);

    output = halfAdderGate(false, false, true, carryOut);
    EXPECT_FALSE(output);
    EXPECT_FALSE(carryOut);

    output = halfAdderGate(false, true, false, carryOut);
    EXPECT_FALSE(output);
    EXPECT_FALSE(carryOut);

    output = halfAdderGate(false, false, false, carryOut);
    EXPECT_FALSE(output);
    EXPECT_FALSE(carryOut);
}

TEST(LogicalGates, fullAdderGate)
{
    bool carryOut = false;
    bool carryIn = false;
    bool power = true;

    const bool inputA[] = {false, true, true, false};
    const bool inputB[] = {true, true, false, false};
    bool output[] = {false, false, false, false};

    output[0] = fullAdderGate(carryIn, power, inputA[0], inputB[0], carryOut);
    output[1] = fullAdderGate(carryOut, power, inputA[1], inputB[1], carryOut);
    output[2] = fullAdderGate(carryOut, power, inputA[2], inputB[2], carryOut);
    output[3] = fullAdderGate(carryOut, power, inputA[3], inputB[3], carryOut);

    EXPECT_TRUE(output[0]);
    EXPECT_FALSE(output[1]);
    EXPECT_FALSE(output[2]);
    EXPECT_TRUE(output[3]);
    EXPECT_FALSE(carryOut);
}
TEST(MemoryTests, sizes)
{
    std::cout << "long int: " << sizeof(long int) << std::endl;
}
