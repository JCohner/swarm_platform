# File structure

## Main
### main.c
* turns on relevant PRCM Domains (Serial | Periph | RF)
* Initializes gpio settings (defined in gpio.c)
* Intializes i2c and with it accelerometer, gyro, and magnotometer (defined in imu.c, i2c_master.c, i2c_accel.c, i2c_gyro.c)
* Enables PWM (defined in pwm.c)
* Setups up RF (defined in zumo_rf.c)
* Sets motors to zero (defined in zumo.c)
* Infinite Loop:
	* Increments a counter, posts to UART0
	* Reads IMU and Magnetometer
	* Conducts step of RF biosynchronicity algorithm

## Sensor Interface
### imu.c
* `start()` intializes UART, i2c, accelerometer, and gyro
* `read_imu()` and `calibrate()` call i2c functions and data manipulation to easily interface with physical peripherals
* effectively higher level middleware for dealing with i2c and uart

### i2c_master.c ; i2c_accel.c, i2c_gyro.c, uart.c
* interfaces with Zumo's on board accelerometer, gyro, and magnotometer
* master:
	* defines i2c peripheral intiializer: `InitI2C0()`
	* defines i2c transmitter: 	`I2CSend()`
	* defines i2c receiver: `I2CReceive()`
* gyro and accel
	* defines WhoAmI functions: `GYRO_WhoAmI()` and `ACC_WhoAmI()`
	* defines setup functions: `acc_setup()` and `gyro_setup()`
	* defines register reading functions: `accel_ReadReg()` and `gyro_ReadReg()`
	* defines macros to interfaces with register reading functions
	* .h files contain pertinent register addresses
* UART
	* implements peripheral intiliazer `InitUART0()`
	* implements read and write functionality; heavily inspired by PIC32 implementations

## Motor Control
### zumo.c ; pwm.c ; gpio.c
* for motor setting functionality
* pwm.c is middleware to enable and write analog values from CC1310
* zumo.c uses both to control motors of zumo.c

## Radio
### zumo_rf.c
* `rf_setup()` configures RF parameters, queue, populates command structures with application specific information, opens handle to radio driver, sets RF frequency, performs init "chirp" 
* `rf_main()`called once by main loop to handle all RF related events
* significant work went into making it asynchronous
* implements biosynchronicity algorithm
	* anonymous units find time delta between received "chirps" (rf RXs)
	* unit "chirps" at half the time delta between heard chirps
	* if no other chirp heard for some time, chirps into the void

### RFQueue.c ; aplication_settings.c; smartrf_settings
* adapted files from TI example code of radio driver
* queue is useful tool for getting data from RX buffer of the cortex controlling RF
* smart rf settings sets basic settings for the proprietary RF commands used 
	* RadioDivSetup - setsup RF Driver
	* cmdFs - sets frequency
	* Tx & Rx - basic properties, application props configured in zumo_rf.c

* application_settings implements more advanced radio commands 
	* branch counter
	* carrier sense operations
	* flush queue op
	* trigger command (for alternatively killing queued ops)