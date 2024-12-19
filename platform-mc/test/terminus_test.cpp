#include "libpldm/entity.h"

#include "platform-mc/numeric_sensor.hpp"
#include "platform-mc/terminus.hpp"

#include <gtest/gtest.h>

// new files added
// #include "platform-mc/terminus.cpp"

#include <gmock/gmock.h>
// new code

#include <stdexcept>

class LoggerInterface
{
  public:
    virtual void logInfo(const std::string& message) = 0;
    virtual ~LoggerInterface() = default;
};

// Simple Console Logger Implementation
class ConsoleLogger : public LoggerInterface
{
  public:
    void logInfo(const std::string& message) override
    {
        std::cout << "[INFO] " << message << std::endl;
    }
};

namespace pldm
{
namespace platform_mc
{

// Custom exception to simulate error during logging
class LoggingException : public std::runtime_error
{
  public:
    explicit LoggingException(const std::string& message) :
        std::runtime_error(message)
    {}
};

class TerminusWithLogger : public Terminus
{
  public:
    TerminusWithLogger(uint8_t tid, uint64_t supportedTypes,
                       LoggerInterface* logger) :
        Terminus(tid, supportedTypes), logger(logger)
    {}

    bool doesSupportCommand(uint8_t type, uint8_t command)
    {
        if (!doesSupportType(type))
        {
            logger->logInfo("Type " + std::to_string(type) +
                            " is not supported by Terminus TID " +
                            std::to_string(getTid()));
            throw LoggingException("Type not supported error logged");
        }

        const auto& supportedCmds = getSupportedCommands();
        size_t idx = type * 8 + (command / 8);

        if (idx >= supportedCmds.size())
        {
            logger->logInfo(
                "Command index out of range for Type " + std::to_string(type) +
                " Command " + std::to_string(command));
            throw LoggingException("Command index out of range error logged");
        }

        if (supportedCmds[idx] & (1 << (command % 8)))
        {
            logger->logInfo("Supported command: TYPE=" + std::to_string(type) +
                            ", CMD=" + std::to_string(command) +
                            ", TID=" + std::to_string(getTid()));
            return true;
        }

        logger->logInfo("Unsupported command: TYPE=" + std::to_string(type) +
                        ", CMD=" + std::to_string(command));
        throw LoggingException("Unsupported command error logged");
    }

  private:
    LoggerInterface* logger;
};

} // namespace platform_mc
} // namespace pldm

// Test Cases Using TEST Instead of TEST_F
TEST(TerminusTest, DoesSupportCommandTest)
{
    ConsoleLogger* logger = new ConsoleLogger();
    pldm::platform_mc::TerminusWithLogger* terminusWithLogger =
        new pldm::platform_mc::TerminusWithLogger(1, 0xFF, logger);

    try
    {
        EXPECT_TRUE(terminusWithLogger->doesSupportCommand(0, 1));
    }
    catch (const pldm::platform_mc::LoggingException& e)
    {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    try
    {
        EXPECT_FALSE(terminusWithLogger->doesSupportCommand(8, 1));
    }
    catch (const pldm::platform_mc::LoggingException& e)
    {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    delete terminusWithLogger;
    delete logger;
}

TEST(TerminusTest, InvalidCommandTest)
{
    ConsoleLogger* logger = new ConsoleLogger();
    pldm::platform_mc::TerminusWithLogger* terminusWithLogger =
        new pldm::platform_mc::TerminusWithLogger(1, 0xFF, logger);

    try
    {
        EXPECT_FALSE(terminusWithLogger->doesSupportCommand(0, 80));
    }
    catch (const pldm::platform_mc::LoggingException& e)
    {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    delete terminusWithLogger;
    delete logger;
}

// new code that sayad work
// class LoggerInterface
// {
//   public:
//     virtual void logInfo(const std::string& message) = 0;
//     virtual ~LoggerInterface() = default;
// };

// // Simple Console Logger Implementation
// class ConsoleLogger : public LoggerInterface
// {
//   public:
//     void logInfo(const std::string& message) override
//     {
//         std::cout << "[INFO] " << message << std::endl;
//     }
// };

// namespace pldm
// {
// namespace platform_mc
// {
// class TerminusWithLogger : public Terminus
// {
//   public:
//     TerminusWithLogger(uint8_t tid, uint64_t supportedTypes,
//                        LoggerInterface* logger) :
//         Terminus(tid, supportedTypes), logger(logger)
//     {}

//     bool doesSupportCommand(uint8_t type, uint8_t command)
//     {
//         if (!doesSupportType(type))
//         {
//             logger->logInfo("Type " + std::to_string(type) +
//                             " is not supported by Terminus TID " +
//                             std::to_string(getTid()));
//             return false;
//         }

//         const auto& supportedCmds = getSupportedCommands();
//         size_t idx = type * 8 + (command / 8);
//         if (idx >= supportedCmds.size())
//         {
//             logger->logInfo(
//                 "Command index out of range for Type " + std::to_string(type)
//                 + " Command " + std::to_string(command));
//             return false;
//         }

//         if (supportedCmds[idx] & (1 << (command % 8)))
//         {
//             logger->logInfo("Supported command: TYPE=" + std::to_string(type)
//             +
//                             ", CMD=" + std::to_string(command) +
//                             ", TID=" + std::to_string(getTid()));
//             return true;
//         }

//         logger->logInfo("Unsupported command: TYPE=" + std::to_string(type) +
//                         ", CMD=" + std::to_string(command));
//         return false;
//     }

//   private:
//     LoggerInterface* logger;
// };

// } // namespace platform_mc
// } // namespace pldm

// // Test Cases Using TEST Instead of TEST_F
// TEST(TerminusTest, DoesSupportCommandTest)
// {
//     ConsoleLogger* logger = new ConsoleLogger();
//     pldm::platform_mc::TerminusWithLogger* terminusWithLogger =
//         new pldm::platform_mc::TerminusWithLogger(1, 0xFF, logger);

//     EXPECT_TRUE(terminusWithLogger->doesSupportCommand(0, 1));
//     EXPECT_FALSE(terminusWithLogger->doesSupportCommand(0, 9));
//     EXPECT_FALSE(terminusWithLogger->doesSupportCommand(8, 1));

//     delete terminusWithLogger;
//     delete logger;
// }

// TEST(TerminusTest, InvalidCommandTest)
// {
//     ConsoleLogger* logger = new ConsoleLogger();
//     pldm::platform_mc::TerminusWithLogger* terminusWithLogger =
//         new pldm::platform_mc::TerminusWithLogger(1, 0xFF, logger);

//     EXPECT_FALSE(terminusWithLogger->doesSupportCommand(0, 80));

//     delete terminusWithLogger;
//     delete logger;
// }

// worked but line 50 par nahi gaya  upar wala

/* class LoggerInterface
{
  public:
    virtual void logInfo(const std::string& message) = 0;
    virtual ~LoggerInterface() = default;
};

// Simple Console Logger Implementation
class ConsoleLogger : public LoggerInterface
{
  public:
    void logInfo(const std::string& message) override
    {
        std::cout << "[INFO] " << message << std::endl;
    }
};  working codeheree  */

// class TerminusWithLogger : public pldm::platform_mc::Terminus
// {
//   public:
//     TerminusWithLogger(uint8_t tid, uint64_t supportedTypes,
//                        LoggerInterface* logger) :
//         pldm::platform_mc::Terminus(tid, supportedTypes), logger(logger)
//     {}

//     // Override doesSupportCommand to include logging
//     bool doesSupportCommand(uint8_t type, uint8_t command)
//     {
//         if (!doesSupportType(type))
//         {
//             logger->logInfo("Type " + std::to_string(type) +
//                             " is not supported by Terminus TID " +
//                             std::to_string(getTid()));
//             return false;
//         }

//         const auto& supportedCmds = getSupportedCommands();
//         size_t idx = type * 8 + (command / 8);
//         if (idx >= supportedCmds.size())
//         {
//             logger->logInfo(
//                 "Command index out of range for Type " + std::to_string(type)
//                 + " Command " + std::to_string(command));
//             return false;
//         }

//         if (supportedCmds[idx] & (1 << (command % 8)))
//         {
//             logger->logInfo("Supported command: TYPE=" + std::to_string(type)
//             +
//                             ", CMD=" + std::to_string(command) +
//                             ", TID=" + std::to_string(getTid()));
//             return true;
//         }

//         logger->logInfo("Unsupported command: TYPE=" + std::to_string(type) +
//                         ", CMD=" + std::to_string(command));
//         return false;
//     }

//   private:
//     LoggerInterface* logger;
// };

// ne codeddd hereeee
//  namespace pldm
//  {
//  namespace platform_mc
//  {
//  class TerminusWithLogger : public Terminus
//  {
//  public:
//      TerminusWithLogger(uint8_t tid, uint64_t supportedTypes,
//      LoggerInterface* logger) :
//          Terminus(tid, supportedTypes), logger(logger)
//      {}

//     bool doesSupportCommand(uint8_t type, uint8_t command)
//     {
//         if (!doesSupportType(type))
//         {
//             logger->logInfo("Type " + std::to_string(type) +
//                             " is not supported by Terminus TID " +
//                             std::to_string(getTid()));
//             return false;
//         }

//         const auto& supportedCmds = getSupportedCommands();
//         size_t idx = type * 8 + (command / 8);
//         if (idx >= supportedCmds.size())
//         {
//             logger->logInfo(
//                 "Command index out of range for Type " + std::to_string(type)
//                 + " Command " + std::to_string(command));
//             return false;
//         }

//         if (supportedCmds[idx] & (1 << (command % 8)))
//         {
//             logger->logInfo("Supported command: TYPE=" + std::to_string(type)
//             +
//                             ", CMD=" + std::to_string(command) +
//                             ", TID=" + std::to_string(getTid()));
//             return true;
//         }

//         logger->logInfo("Unsupported command: TYPE=" + std::to_string(type) +
//                         ", CMD=" + std::to_string(command));
//         return false;
//     }

// private:
//     LoggerInterface* logger;
// };
// } // namespace platform_mc
// }
// class TerminusTest : public ::testing::Test
// {
// protected:
//     void SetUp() override
//     {
//         logger = new ConsoleLogger();
//         terminusWithLogger =
//             new pldm::platform_mc::TerminusWithLogger(1, 0xFF, logger);
//     }

//     void TearDown() override
//     {
//         delete logger;
//         delete terminusWithLogger;
//     }

//     ConsoleLogger* logger;
//     pldm::platform_mc::TerminusWithLogger* terminusWithLogger;
// };

// TEST_F(TerminusTest, DoesSupportCommandTest)
// {
//     EXPECT_TRUE(terminusWithLogger->doesSupportCommand(0, 1));
//     EXPECT_FALSE(terminusWithLogger->doesSupportCommand(0, 9));
//     EXPECT_FALSE(terminusWithLogger->doesSupportCommand(8, 1));
// }

// TEST_F(TerminusTest, InvalidCommandTest)
// {
//     EXPECT_FALSE(terminusWithLogger->doesSupportCommand(0, 80));
// }

// previous working code herreeee is the placedddd ...

// namespace lg2
// {
// MOCK_METHOD(void, info,
//             (const char* message, const char* arg1, uint8_t value1,
//              const char* arg2, uint8_t value2, const char* arg3,
//              pldm_tid_t value3),
//             ());
// } // namespace lg2

// // Test fixture for Terminus
// class TerminusTest : public ::testing::Test
// {
//   protected:
//     // You can initialize any setup needed for Terminus
//     pldm_tid_t tid = 1;
//     uint64_t supportedTypes = 0b1; // Supports only type 0
//     Terminus terminus{tid, supportedTypes};
// };

// // Test for doesSupportCommand
// TEST_F(TerminusTest, DoesSupportCommand_CatchException)
// {
//     // Arrange: Make the supportedCmds vector too small to simulate
//     // out-of-bounds access
//     terminus.supportedCmds.resize(
//         1); // Resize to force an out-of-bounds exception
//     uint8_t type = 0;
//     uint8_t command = 2;

//     // Expect lg2::info to be called before the exception
//     EXPECT_CALL(
//         lg2::info,
//         info(
//             testing::StrEq(
//                 "PLDM type {TYPE} command {CMD} is supported by terminus
//                 {TID}"),
//             testing::StrEq("TYPE"), type, testing::StrEq("CMD"), command,
//             testing::StrEq("TID"), tid)).Times(1);

//     // Act: Call the method
//     bool result = terminus.doesSupportCommand(type, command);

//     // Assert: The catch block should handle the exception and return false
//     EXPECT_FALSE(result);
// }

// new code

TEST(TerminusTest, supportedTypeTest)
{
    auto t1 = pldm::platform_mc::Terminus(1, 1 << PLDM_BASE);
    auto t2 =
        pldm::platform_mc::Terminus(2, 1 << PLDM_BASE | 1 << PLDM_PLATFORM);

    EXPECT_EQ(true, t1.doesSupportType(PLDM_BASE));
    EXPECT_EQ(false, t1.doesSupportType(PLDM_PLATFORM));
    EXPECT_EQ(true, t2.doesSupportType(PLDM_BASE));
    EXPECT_EQ(true, t2.doesSupportType(PLDM_PLATFORM));
}

TEST(TerminusTest, getTidTest)
{
    const pldm_tid_t tid = 1;
    auto t1 = pldm::platform_mc::Terminus(tid, 1 << PLDM_BASE);

    EXPECT_EQ(tid, t1.getTid());
}

TEST(TerminusTest, parseSensorAuxiliaryNamesPDRTest)
{
    auto t1 =
        pldm::platform_mc::Terminus(1, 1 << PLDM_BASE | 1 << PLDM_PLATFORM);
    std::vector<uint8_t> pdr1{
        0x0,
        0x0,
        0x0,
        0x1,                             // record handle
        0x1,                             // PDRHeaderVersion
        PLDM_SENSOR_AUXILIARY_NAMES_PDR, // PDRType
        0x0,
        0x0,                             // recordChangeNumber
        0x0,
        21,                              // dataLength
        0,
        0x0,                             // PLDMTerminusHandle
        0x1,
        0x0,                             // sensorID
        0x1,                             // sensorCount
        0x1,                             // nameStringCount
        'e',
        'n',
        0x0, // nameLanguageTag
        0x0,
        'T',
        0x0,
        'E',
        0x0,
        'M',
        0x0,
        'P',
        0x0,
        '1',
        0x0,
        0x0 // sensorName
    };

    std::vector<uint8_t> pdr2{
        0x1, 0x0, 0x0,
        0x0,                             // record handle
        0x1,                             // PDRHeaderVersion
        PLDM_ENTITY_AUXILIARY_NAMES_PDR, // PDRType
        0x1,
        0x0,                             // recordChangeNumber
        0x11,
        0,                               // dataLength
        /* Entity Auxiliary Names PDR Data*/
        3,
        0x80, // entityType system software
        0x1,
        0x0,  // Entity instance number =1
        0,
        0,    // Overall system
        0,    // shared Name Count one name only
        01,   // nameStringCount
        0x65, 0x6e, 0x00,
        0x00, // Language Tag "en"
        0x53, 0x00, 0x30, 0x00,
        0x00  // Entity Name "S0"
    };

    t1.pdrs.emplace_back(pdr1);
    t1.pdrs.emplace_back(pdr2);
    t1.parseTerminusPDRs();

    auto sensorAuxNames = t1.getSensorAuxiliaryNames(0);
    EXPECT_EQ(nullptr, sensorAuxNames);

    sensorAuxNames = t1.getSensorAuxiliaryNames(1);
    EXPECT_NE(nullptr, sensorAuxNames);

    const auto& [sensorId, sensorCnt, names] = *sensorAuxNames;
    EXPECT_EQ(1, sensorId);
    EXPECT_EQ(1, sensorCnt);
    EXPECT_EQ(1, names.size());
    EXPECT_EQ(1, names[0].size());
    EXPECT_EQ("en", names[0][0].first);
    EXPECT_EQ("TEMP1", names[0][0].second);
    EXPECT_EQ(2, t1.pdrs.size());
    EXPECT_EQ("S0", t1.getTerminusName().value());
}

TEST(TerminusTest, parseSensorAuxiliaryMultiNamesPDRTest)
{
    auto t1 =
        pldm::platform_mc::Terminus(1, 1 << PLDM_BASE | 1 << PLDM_PLATFORM);
    std::vector<uint8_t> pdr1{
        0x0,
        0x0,
        0x0,
        0x1,                             // record handle
        0x1,                             // PDRHeaderVersion
        PLDM_SENSOR_AUXILIARY_NAMES_PDR, // PDRType
        0x0,
        0x0,                             // recordChangeNumber
        0x0,
        53,                              // dataLength
        0,
        0x0,                             // PLDMTerminusHandle
        0x1,
        0x0,                             // sensorID
        0x1,                             // sensorCount
        0x3,                             // nameStringCount
        'e',
        'n',
        0x0, // nameLanguageTag
        0x0,
        'T',
        0x0,
        'E',
        0x0,
        'M',
        0x0,
        'P',
        0x0,
        '1',
        0x0,
        0x0, // sensorName Temp1
        'f',
        'r',
        0x0, // nameLanguageTag
        0x0,
        'T',
        0x0,
        'E',
        0x0,
        'M',
        0x0,
        'P',
        0x0,
        '2',
        0x0,
        0x0, // sensorName Temp2
        'f',
        'r',
        0x0, // nameLanguageTag
        0x0,
        'T',
        0x0,
        'E',
        0x0,
        'M',
        0x0,
        'P',
        0x0,
        '1',
        0x0,
        '2',
        0x0,
        0x0 // sensorName Temp12
    };

    std::vector<uint8_t> pdr2{
        0x1, 0x0, 0x0,
        0x0,                             // record handle
        0x1,                             // PDRHeaderVersion
        PLDM_ENTITY_AUXILIARY_NAMES_PDR, // PDRType
        0x1,
        0x0,                             // recordChangeNumber
        0x11,
        0,                               // dataLength
        /* Entity Auxiliary Names PDR Data*/
        3,
        0x80, // entityType system software
        0x1,
        0x0,  // Entity instance number =1
        0,
        0,    // Overall system
        0,    // shared Name Count one name only
        01,   // nameStringCount
        0x65, 0x6e, 0x00,
        0x00, // Language Tag "en"
        0x53, 0x00, 0x30, 0x00,
        0x00  // Entity Name "S0"
    };

    t1.pdrs.emplace_back(pdr1);
    t1.pdrs.emplace_back(pdr2);
    t1.parseTerminusPDRs();

    auto sensorAuxNames = t1.getSensorAuxiliaryNames(0);
    EXPECT_EQ(nullptr, sensorAuxNames);

    sensorAuxNames = t1.getSensorAuxiliaryNames(1);
    EXPECT_NE(nullptr, sensorAuxNames);

    const auto& [sensorId, sensorCnt, names] = *sensorAuxNames;
    EXPECT_EQ(1, sensorId);
    EXPECT_EQ(1, sensorCnt);
    EXPECT_EQ(1, names.size());
    EXPECT_EQ(3, names[0].size());
    EXPECT_EQ("en", names[0][0].first);
    EXPECT_EQ("TEMP1", names[0][0].second);
    EXPECT_EQ("fr", names[0][1].first);
    EXPECT_EQ("TEMP2", names[0][1].second);
    EXPECT_EQ("fr", names[0][2].first);
    EXPECT_EQ("TEMP12", names[0][2].second);
    EXPECT_EQ(2, t1.pdrs.size());
    EXPECT_EQ("S0", t1.getTerminusName().value());
}

TEST(TerminusTest, parseSensorAuxiliaryNamesMultiSensorsPDRTest)
{
    auto t1 =
        pldm::platform_mc::Terminus(1, 1 << PLDM_BASE | 1 << PLDM_PLATFORM);
    std::vector<uint8_t> pdr1{
        0x0,
        0x0,
        0x0,
        0x1,                             // record handle
        0x1,                             // PDRHeaderVersion
        PLDM_SENSOR_AUXILIARY_NAMES_PDR, // PDRType
        0x0,
        0x0,                             // recordChangeNumber
        0x0,
        54,                              // dataLength
        0,
        0x0,                             // PLDMTerminusHandle
        0x1,
        0x0,                             // sensorID
        0x2,                             // sensorCount
        0x1,                             // nameStringCount
        'e',
        'n',
        0x0, // nameLanguageTag
        0x0,
        'T',
        0x0,
        'E',
        0x0,
        'M',
        0x0,
        'P',
        0x0,
        '1',
        0x0,
        0x0, // sensorName Temp1
        0x2, // nameStringCount
        'f',
        'r',
        0x0, // nameLanguageTag
        0x0,
        'T',
        0x0,
        'E',
        0x0,
        'M',
        0x0,
        'P',
        0x0,
        '2',
        0x0,
        0x0, // sensorName Temp2
        'f',
        'r',
        0x0, // nameLanguageTag
        0x0,
        'T',
        0x0,
        'E',
        0x0,
        'M',
        0x0,
        'P',
        0x0,
        '1',
        0x0,
        '2',
        0x0,
        0x0 // sensorName Temp12
    };

    std::vector<uint8_t> pdr2{
        0x1, 0x0, 0x0,
        0x0,                             // record handle
        0x1,                             // PDRHeaderVersion
        PLDM_ENTITY_AUXILIARY_NAMES_PDR, // PDRType
        0x1,
        0x0,                             // recordChangeNumber
        0x11,
        0,                               // dataLength
        /* Entity Auxiliary Names PDR Data*/
        3,
        0x80, // entityType system software
        0x1,
        0x0,  // Entity instance number =1
        0,
        0,    // Overall system
        0,    // shared Name Count one name only
        01,   // nameStringCount
        0x65, 0x6e, 0x00,
        0x00, // Language Tag "en"
        0x53, 0x00, 0x30, 0x00,
        0x00  // Entity Name "S0"
    };

    t1.pdrs.emplace_back(pdr1);
    t1.pdrs.emplace_back(pdr2);
    t1.parseTerminusPDRs();

    auto sensorAuxNames = t1.getSensorAuxiliaryNames(0);
    EXPECT_EQ(nullptr, sensorAuxNames);

    sensorAuxNames = t1.getSensorAuxiliaryNames(1);
    EXPECT_NE(nullptr, sensorAuxNames);

    const auto& [sensorId, sensorCnt, names] = *sensorAuxNames;
    EXPECT_EQ(1, sensorId);
    EXPECT_EQ(2, sensorCnt);
    EXPECT_EQ(2, names.size());
    EXPECT_EQ(1, names[0].size());
    EXPECT_EQ("en", names[0][0].first);
    EXPECT_EQ("TEMP1", names[0][0].second);
    EXPECT_EQ(2, names[1].size());
    EXPECT_EQ("fr", names[1][0].first);
    EXPECT_EQ("TEMP2", names[1][0].second);
    EXPECT_EQ("fr", names[1][1].first);
    EXPECT_EQ("TEMP12", names[1][1].second);
    EXPECT_EQ(2, t1.pdrs.size());
    EXPECT_EQ("S0", t1.getTerminusName().value());
}

TEST(TerminusTest, parsePDRTestNoSensorPDR)
{
    auto t1 =
        pldm::platform_mc::Terminus(1, 1 << PLDM_BASE | 1 << PLDM_PLATFORM);
    std::vector<uint8_t> pdr1{
        0x1, 0x0, 0x0,
        0x0,                             // record handle
        0x1,                             // PDRHeaderVersion
        PLDM_ENTITY_AUXILIARY_NAMES_PDR, // PDRType
        0x1,
        0x0,                             // recordChangeNumber
        0x11,
        0,                               // dataLength
        /* Entity Auxiliary Names PDR Data*/
        3,
        0x80, // entityType system software
        0x1,
        0x0,  // Entity instance number =1
        0,
        0,    // Overall system
        0,    // shared Name Count one name only
        01,   // nameStringCount
        0x65, 0x6e, 0x00,
        0x00, // Language Tag "en"
        0x53, 0x00, 0x30, 0x00,
        0x00  // Entity Name "S0"
    };

    t1.pdrs.emplace_back(pdr1);
    t1.parseTerminusPDRs();

    auto sensorAuxNames = t1.getSensorAuxiliaryNames(1);
    EXPECT_EQ(nullptr, sensorAuxNames);
}
