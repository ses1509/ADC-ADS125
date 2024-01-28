#include<i2cInterface.h>

i2cInterfaceClass::i2cInterfaceClass(uint8_t  i2c_bus, uint8_t i2c_address)
{
	m_i2cAddress 		= i2c_address;
	m_i2cFd 		= -1;
	m_i2cBus 		= i2c_bus;
	m_conversionDelay 	= ADS1115_CONVERSIONDELAY;
	m_bitShift 		= 0;
	m_gain			= GAIN_TWOTHIRDS;		
}


void i2cInterfaceClass::setGainTwoThird()
{
	m_gain = GAIN_TWOTHIRDS;
}



int i2cInterfaceClass::setupI2CInterface()
{

	
	const char *device ;
	if(m_i2cBus==1){
		device = "/dev/i2c-0" ;
	}
	else{
		device = "/dev/i2c-1" ;
	}
	if ((m_i2cFd = open(device, O_RDWR)) < 0)
	{
		printf(" 1-error @ %s:%d \n", __FUNCTION__, __LINE__);
    		return FAILURE;
    		
  	}

	if (ioctl(m_i2cFd, I2C_SLAVE, m_i2cAddress) < 0) 
	{
		printf(" 2-error @ %s:%d \n", __FUNCTION__, __LINE__);
		return FAILURE;
	}
	
	
	uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Bits 0-1 	Disable the comparator (default val)
			  ADS1015_REG_CONFIG_CLAT_NONLAT  | // Bit 2 		Non-latching (default val)
			  ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Bit 3 		Alert/Rdy active low   (default val)
			  ADS1015_REG_CONFIG_CMODE_TRAD   | // Bit 4 		Traditional comparator (default val)
			  ADS1015_REG_CONFIG_DR_128SPS    | // Bit 5-7  	128 samples per second (default)
			  ADS1015_REG_CONFIG_MODE_SINGLE  | // Bit 8 		Single-shot mode (default)
			  m_gain			  | // Bits 9-11 	Gain
			  ADS1015_REG_CONFIG_MUX_DIFF_0_1 | // Bits 12-14 	Differential Channel 0_1
			  ADS1015_REG_CONFIG_OS_SINGLE ;    // Bit 15           

	uint8_t writeBuf[3];
	//uint8_t readBuf[2];
	
	writeBuf[0] = ADS1015_REG_POINTER_CONFIG;
	writeBuf[1] = config>>8;
	writeBuf[2] = config & 0x00ff;
	
	// begin conversion
	if (write(m_i2cFd, writeBuf, 3) != 3) {
		printf(" conversion -module -error @ %s:%d \n", __FUNCTION__, __LINE__);
		return FAILURE;
	}
	else{
		return m_i2cFd;
	}

}

float i2cInterfaceClass::readADC_Differential_0_1()
{
	//cout<<"m_gain =   "<<m_gain<<endl;
	int16_t val = -100;
	uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Bits 0-1 	Disable the comparator (default val)
			  ADS1015_REG_CONFIG_CLAT_NONLAT  | // Bit 2 		Non-latching (default val)
			  ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Bit 3 		Alert/Rdy active low   (default val)
			  ADS1015_REG_CONFIG_CMODE_TRAD   | // Bit 4 		Traditional comparator (default val)
			  ADS1015_REG_CONFIG_DR_920SPS    | // Bit 5-7  	128 samples per second (default)
			  ADS1015_REG_CONFIG_MODE_SINGLE  | // Bit 8 		Single-shot mode (default)
			  m_gain			  | // Bits 9-11 	Gain
			  ADS1015_REG_CONFIG_MUX_DIFF_0_1 | // Bits 12-14 	Differential Channel 0_1
			  ADS1015_REG_CONFIG_OS_SINGLE ;    // Bit 15           

	uint8_t writeBuf[3];
	uint8_t readBuf[2];
	
	//printf("config 0_1 = %04x \n", config);  //commented on nov-2022
	
	writeBuf[0] = ADS1015_REG_POINTER_CONFIG;
	writeBuf[1] = config>>8;
	writeBuf[2] = config & 0x00ff;
	
	// begin conversion
	if (write(m_i2cFd, writeBuf, 3) != 3) {
		perror("Write to register");
		printf(" %s:%d \n", __FUNCTION__, __LINE__);
	}
	
	#if 1
	// wait for conversion complete
	// checking bit 15
	do{
		if (read(m_i2cFd, writeBuf, 2) != 2) 
		{
			perror("Read conversion");
			printf(" %s:%d \n", __FUNCTION__, __LINE__);
			break;
		}
	}
	while ((writeBuf[0] & 0x80) == 0);
	#endif

	readBuf[0] = ADS1015_REG_POINTER_CONVERT;   // conversion register is 0
	if (write(m_i2cFd, readBuf, 1) != 1) {
		perror("Write register select");
		printf(" %s:%d \n", __FUNCTION__, __LINE__);
		return val;
	}
  
  	// read 2 bytes
	if (read(m_i2cFd, readBuf, 2) != 2) {
		perror("Read conversion");
		printf(" %s:%d \n", __FUNCTION__, __LINE__);
		return val;
	}

	// convert display results
	val = readBuf[0] << 8 | readBuf[1];

	return val * SCALE_FACTOR/1000;
}


float i2cInterfaceClass::readADC_Differential_2_3()
{
	int16_t val = -100;
	uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Bits 0-1 	Disable the comparator (default val)
			  ADS1015_REG_CONFIG_CLAT_NONLAT  | // Bit 2 		Non-latching (default val)
			  ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Bit 3 		Alert/Rdy active low   (default val)
			  ADS1015_REG_CONFIG_CMODE_TRAD   | // Bit 4 		Traditional comparator (default val)
			  ADS1015_REG_CONFIG_DR_920SPS    | // Bit 5-7  	128 samples per second (default)
			  ADS1015_REG_CONFIG_MODE_SINGLE  | // Bit 8 		Single-shot mode (default)
			  m_gain			  | // Bits 9-11 	Gain
			  ADS1015_REG_CONFIG_MUX_DIFF_2_3 | // Bits 12-14 	Differential Channel 0_1
			  ADS1015_REG_CONFIG_OS_SINGLE ;    // Bit 15           

	uint8_t writeBuf[3];
	uint8_t readBuf[2];
	
	writeBuf[0] = ADS1015_REG_POINTER_CONFIG;
	writeBuf[1] = config>>8;
	writeBuf[2] = config & 0x00ff;
	//printf("config 2_3 = %04x \n", config);  //commented on nov-2022
	// begin conversion
	if (write(m_i2cFd, writeBuf, 3) != 3) {
		perror("Write to register");
		printf(" %s:%d \n", __FUNCTION__, __LINE__);
	}
	

	// wait for conversion complete
	// checking bit 15
	#if 1
	do{
		if (read(m_i2cFd, writeBuf, 2) != 2) 
		{
			perror("Read conversion");
			printf(" %s:%d \n", __FUNCTION__, __LINE__);
			break;
		}
	}
	while ((writeBuf[0] & 0x80) == 0);
	#endif

	readBuf[0] = ADS1015_REG_POINTER_CONVERT;   // conversion register is 0
	if (write(m_i2cFd, readBuf, 1) != 1) {
		perror("Write register select");
		printf(" %s:%d \n", __FUNCTION__, __LINE__);
		return val;
	}
  
  	// read 2 bytes
	if (read(m_i2cFd, readBuf, 2) != 2) {
		perror("Read conversion ");
		printf(" %s:%d \n", __FUNCTION__, __LINE__);
		return val;
	}

	// convert display results
	val = readBuf[0] << 8 | readBuf[1];

	return val * SCALE_FACTOR/1000;
}
