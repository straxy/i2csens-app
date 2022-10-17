/**
 * @file I2CHandler.h
 * @author Strahinja Jankovic (strahinja.p.jankovic[at]gmail.com)
 * @details Implementation of I2C userspace handling
 */

#ifndef _I2C_HANDLER_H_
#define _I2C_HANDLER_H_

#include <string>

namespace i2chandler
{

class I2CHandler
{
public:
    /**
     * Constructor
     * 
     * @param path - path to I2C device
     */
    explicit I2CHandler(std::string path);

    /* Disable copy constructor and assignment. */
    I2CHandler(const I2CHandler&)              = delete;
    I2CHandler & operator=(const I2CHandler&)  = delete;

    /** Destructor. */
    virtual ~I2CHandler();

    /**
     * init
     * 
     * Initialize I2C device.
     */
    int8_t init(void);

    /**
     * deinit
     * 
     * Disable I2C device.
     */
    void deinit(void);

    /**
     * runThread
     * 
     * Start executing thread.
     */
    void runThread(void);

    /**
     * stopThread
     * 
     * Stop executing thread.
     */
    void stopThread(void);

private:
    /**
     * do_read
     * 
     * Thread that periodically reads I2C temperature data.
     */
    void do_read(void);

    /**
     * write_reg
     * 
     * Helper function to write register value.
     */
    bool write_reg(uint8_t reg_nr, uint8_t val);

    /**
     * read_reg
     * 
     * Helper function to read register value
     */
    bool read_reg(uint8_t reg_nr, uint8_t& val);

    /** Path to I2C device. */
    std::string m_path;

    /** Thread handle. */
    std::thread m_thread;

    /** Flag used to control thread execution. */
    bool m_running;

    /** I2C dev file descriptor. */
    int m_fd;
};

}   // namespace i2chandler

#endif  // _I2C_HANDLER_H_
