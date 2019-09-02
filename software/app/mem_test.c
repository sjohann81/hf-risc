#include <hf-risc.h>


int32_t memTestDataBus(volatile uint8_t *address){
	uint8_t pattern;

	for (pattern = 1; pattern != 0; pattern <<= 1){
		*address = pattern;
		if (*address != pattern){
			return (pattern);
		}
	}
	return (0);
}

uint8_t *memTestAddressBus(volatile uint8_t *baseAddress, uint32_t nBytes){
	uint32_t addressMask = (nBytes/sizeof(uint8_t) - 1);
	uint32_t offset;
	uint32_t testOffset;
	uint8_t pattern     = (uint8_t)0xAAAAAAAA;
	uint8_t antipattern = (uint8_t)0x55555555;

	nBytes--;
	for (offset = 1; (offset & addressMask) != 0; offset <<= 1){
		baseAddress[offset] = pattern;
	}
	testOffset = 0;
	baseAddress[testOffset] = antipattern;
	for (offset = 1; (offset & addressMask) != 0; offset <<= 1){
		if (baseAddress[offset] != pattern){
			return ((uint8_t *) &baseAddress[offset]);
		}
	}
	baseAddress[testOffset] = pattern;
	for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1){
		baseAddress[testOffset] = antipattern;
		if (baseAddress[0] != pattern){
			return ((uint8_t *) &baseAddress[testOffset]);
		}
		for (offset = 1; (offset & addressMask) != 0; offset <<= 1){
			if ((baseAddress[offset] != pattern) && (offset != testOffset)){
				return ((uint8_t *) &baseAddress[testOffset]);
			}
		}
		baseAddress[testOffset] = pattern;
	}
	return (0);
}

uint8_t *memTestDevice(volatile uint8_t *baseAddress, uint32_t nBytes){
	uint32_t offset;
	uint32_t nWords = nBytes / sizeof(uint8_t);
	uint8_t pattern;
	uint8_t antipattern;

	for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++){
		baseAddress[offset] = pattern;
	}

	for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++){
		if (baseAddress[offset] != pattern){
			return ((uint8_t *) &baseAddress[offset]);
		}
		antipattern = ~pattern;
		baseAddress[offset] = antipattern;
	}
	for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++){
		antipattern = ~pattern;
		if (baseAddress[offset] != antipattern){
			return ((uint8_t *) &baseAddress[offset]);
		}
	}
	return (0);
}


#define MARCHC_BIT_WIDTH            32      //  access data width
#define MARCHC_TEST_PASS    1
#define MARCHC_TEST_FAIL    0
#define MARCHC_TEST_FAIL_SIZE   -1
#define MARCHC_TEST_FAIL_STACK  -2

static int ReadZeroWriteOne( volatile int * ptr)
{
    int tempValue;
    int loopCounter;

    for (loopCounter=MARCHC_BIT_WIDTH-1; loopCounter>=0; loopCounter--)
    {

        tempValue =(((*ptr)>>loopCounter) & 1);  // read 0
        if (tempValue!= 0)
        {
            return MARCHC_TEST_FAIL;
        }

        *ptr=(*ptr | (1<<loopCounter));             // write 1
    }

    return MARCHC_TEST_PASS;
}

static int ReadOneWriteZero( volatile int * ptr )
{
    int tempValue;
    int loopCounter;


    for (loopCounter=0; loopCounter<MARCHC_BIT_WIDTH; loopCounter++)
    {
        tempValue =(((*ptr)>>loopCounter) & 1);     // read 1

        if (tempValue!= 1)
        {
            return MARCHC_TEST_FAIL;
        }

        tempValue =  *ptr  & ~(1<<loopCounter);       // write 0
        *ptr= tempValue;
    }

    return MARCHC_TEST_PASS;
}

static int ReadZero( volatile int * ptr )
{
    int tempValue;
    int loopCounter;

    for (loopCounter=0; loopCounter<MARCHC_BIT_WIDTH; loopCounter++)
    {
        tempValue =(((*ptr)>>loopCounter) & 1);    // read 0

        if (tempValue!= 0)
        {
            return MARCHC_TEST_FAIL;
        }
    }

    return MARCHC_TEST_PASS;
}

//static int RAMtestMarchC( int* ramStartAddress, int ramSize, int isFullC)
static int RAMtestMarchC( int* ramStartAddress, int ramSize)
{
    int testResult;
    int *ramEndAddress;
    int *ptr;

    ramEndAddress=ramStartAddress + (ramSize/4);    // ramSize should be in bytes

    // erase all sram
    for(ptr=ramStartAddress; ptr<ramEndAddress; ptr++)
    {
        *ptr=0;
    }

    testResult=MARCHC_TEST_PASS;


    // Test Bitwise if 0 and replace it with 1 starting from lower Addresses
    for(ptr=ramStartAddress; ptr<ramEndAddress && testResult==MARCHC_TEST_PASS; ptr++)
    {
        testResult =  ReadZeroWriteOne(ptr);
    }

    // Test Bitwise if 1 and replace it with 0 starting from lower Addresses
    for(ptr=ramStartAddress; ptr<ramEndAddress && testResult==MARCHC_TEST_PASS; ptr++)
    {
        testResult =  ReadOneWriteZero(ptr);
    }

//    if(isFullC)
//    {
        // Test if all bits are zeros starting from lower Addresses
        for(ptr=ramStartAddress; ptr<ramEndAddress && testResult==MARCHC_TEST_PASS; ptr++)
        {
            testResult =  ReadZero(ptr);
        }
//    }

    // Test Bitwise if 0 and replace it with 1 starting from higher Addresses
    for (ptr=ramEndAddress-1; ptr>=ramStartAddress && testResult==MARCHC_TEST_PASS; ptr--)
    {
        testResult =  ReadZeroWriteOne(ptr);
    }

    // Test Bitwise if 1 and replace it with 0 starting from higher Addresses
    for (ptr=ramEndAddress-1; ptr>=ramStartAddress && testResult==MARCHC_TEST_PASS; ptr--)
    {
        testResult =  ReadOneWriteZero(ptr);
    }

    // Test if all bits are zeros starting from higher Addresses
    for (ptr=ramEndAddress-1; ptr>=ramStartAddress && testResult==MARCHC_TEST_PASS; ptr--)
    {
        testResult =  ReadZero(ptr);
    }

    return testResult;
}

int main(void)
{
	unsigned int addr = 0x30000000, addr2;
	unsigned int len = 1024 * 128;

	printf("\nmem size: %d Kbytes", len >> 10);

	printf("\nmemory data bus test");
	addr2 = memTestDataBus((uint8_t *)addr);
	if (addr2 != 0)
		printf(" failed! pattern: %08x", addr2);
	else
		printf(" ok.");

	printf("\nmemory address bus test");
	addr2 = (uint32_t)memTestAddressBus((uint8_t *)addr, len);
	if (addr2 != 0)
		printf(" failed! address: %08x", addr2);
	else
		printf(" ok.");

	printf("\nmemory device is");
	addr2 = (uint32_t)memTestDevice((uint8_t *)addr, len);
	if (addr2 != 0)
		printf(" defective! address: %08x", addr2);
	else
		printf(" ok.");

	if (RAMtestMarchC((int *)addr, (int)len) == MARCHC_TEST_PASS){
		printf("\nMarch-C test passed!");
	}else{
		printf("\nMarch-C test failed!");
	}

	printf("\ntest completed.\n");

	return 0;
}
