#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2chandler.h"

using namespace i2chandler;

namespace
{
    constexpr uint8_t I2CSENS_ADDR = 0x36;
    // Register definitions
    enum class I2CSensRegs : uint8_t
    {
        ID,
        CTRL,
        TEMPERATURE
    };
    // Enable value
    constexpr uint8_t ENABLE = 0x01;
};  // namespace anonymous

//-----------------------------------------------------------------------------

I2CHandler::I2CHandler(std::string path) : m_path{path}, m_running{false}
{
}

//-----------------------------------------------------------------------------

I2CHandler::~I2CHandler()
{
}

//-----------------------------------------------------------------------------

int8_t I2CHandler::init(void)
{
    // Open FD
    m_fd = open(m_path.c_str(), O_RDWR);
    if (m_fd == -1)
    {
        return m_fd;
    }

    // Set I2C slave address
    if (ioctl(m_fd, I2C_SLAVE, I2CSENS_ADDR) < 0)
    {
        return -2;
    }

    // Enable device
    if (!write_reg(static_cast<uint8_t>(I2CSensRegs::CTRL), ENABLE))
    {
        return -3;
    }

    return 0;
}

//-----------------------------------------------------------------------------

void I2CHandler::deinit(void)
{
    if (!write_reg(static_cast<uint8_t>(I2CSensRegs::CTRL), 0))
    {
        std::cerr << "Problem disabling device";
    }
}

//-----------------------------------------------------------------------------

void I2CHandler::runThread(void)
{
    if (!m_thread.joinable())
    {
        m_thread = std::thread(&I2CHandler::do_read, this);
    }
}

//-----------------------------------------------------------------------------

void I2CHandler::stopThread(void)
{
    m_running = false;

    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

//-----------------------------------------------------------------------------

void I2CHandler::do_read(void)
{
    std::chrono::system_clock::time_point current_time;
    uint8_t value;

    m_running = true;

    while (m_running)
    {
        // Read current time so we know when to timeout
        current_time = std::chrono::system_clock::now();

        // read I2C device and print
        if (!read_reg(static_cast<uint8_t>(I2CSensRegs::TEMPERATURE), value))
        {
            m_running = false;
            break;
        }

        std::cout << "Measured " << std::setprecision(3) << (value / 2.) << std::endl;

        // sleep_until
        std::this_thread::sleep_until(current_time + std::chrono::seconds(1));
    }
}

//-----------------------------------------------------------------------------

bool I2CHandler::write_reg(uint8_t reg_nr, uint8_t val)
{
    uint8_t buffer[2] = { reg_nr, val };

    return (write(m_fd, buffer, 2) == 2);
}

//-----------------------------------------------------------------------------

bool I2CHandler::read_reg(uint8_t reg_nr, uint8_t& val)
{
    uint8_t buffer[1] = { reg_nr };
    bool retval = (write(m_fd, buffer, 1) == 1);

    if (retval)
    {
        retval = (read(m_fd, buffer, 1) == 1);
        if (retval)
        {
            val = buffer[0];
        }
    }

    return retval;
}
