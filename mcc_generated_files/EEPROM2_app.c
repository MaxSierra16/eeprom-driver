/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/
#include <stdint.h>
#include "pin_manager.h"
#include "drivers/spi_master.h"
#include "EEPROM2_app.h"

#define EEPROM2_READ         0x03                // read data from memory
#define EEPROM2_WREN         0x06                // set the write enable latch
#define EEPROM2_WRITE        0x02                // write data to memory array
#define EEPROM2_RDSR         0x05                // read STATUS register
#define EEPROM2_ADDRBYTES    3


static void EEPROM2_AddressAssign(uint8_t *addressBuffer, uint32_t byteAddr);
static void EEPROM2_WriteEnable(void);
static void EEPROM2_CheckStatusRegister(void);

static uint8_t EEPROM2_ReadStatusRegister(void);


void EEPROM2_WriteByte (uint8_t byteData, uint32_t byteAddr)
{
    uint8_t addressBuffer[EEPROM2_ADDRBYTES];

    EEPROM2_AddressAssign(addressBuffer, byteAddr);
    
    EEPROM2_WriteEnable();

    EEPROM2_CheckStatusRegister();
    
    spiMaster[EEPROM2].spiOpen();
    LATGbits.LATG9 = 0; /* set EEPROM2_nCS output low */
    
    //Send Write Command
    spiMaster[EEPROM2].exchangeByte(EEPROM2_WRITE);
    //Send address byte/s
    spiMaster[EEPROM2].exchangeBlock(addressBuffer,EEPROM2_ADDRBYTES);
    //Send data byte
    spiMaster[EEPROM2].exchangeByte(byteData);
    
    LATGbits.LATG9 = 1; /* set EEPROM2_nCS output high */
    spiMaster[EEPROM2].spiClose();
    
}

uint8_t EEPROM2_ReadByte (uint32_t address)
{
    uint8_t readByte;
    uint8_t addressBuffer[EEPROM2_ADDRBYTES];
    
    //Wait for write cycle to complete
    EEPROM2_WritePoll();

    EEPROM2_AddressAssign(addressBuffer, address);
    
    spiMaster[EEPROM2].spiOpen();
    LATGbits.LATG9 = 0; /* set EEPROM2_nCS output low */
    
    //Send Read Command
    spiMaster[EEPROM2].exchangeByte(EEPROM2_READ);
    //Send address bytes
    spiMaster[EEPROM2].exchangeBlock(addressBuffer,EEPROM2_ADDRBYTES);
    //Send Dummy data to clock out data byte from slave
    readByte = spiMaster[EEPROM2].exchangeByte(0x00);
    
    LATGbits.LATG9 = 1; /* set EEPROM2_nCS output high */
    spiMaster[EEPROM2].spiClose();
    
    //return data byte read
    return(readByte);
}

void EEPROM2_WriteBlock(uint8_t *writeBuffer, uint8_t buflen, uint32_t startAddr)
{
    uint8_t addressBuffer[EEPROM2_ADDRBYTES];
    
    EEPROM2_AddressAssign(addressBuffer, startAddr);
    
    EEPROM2_WriteEnable();

    EEPROM2_CheckStatusRegister();
    
    spiMaster[EEPROM2].spiOpen();
    LATGbits.LATG9 = 0; /* set EEPROM2_nCS output low */
    
    //Send Write Command
    spiMaster[EEPROM2].exchangeByte(EEPROM2_WRITE);
    //Send address bytes
    spiMaster[EEPROM2].exchangeBlock(addressBuffer,EEPROM2_ADDRBYTES);
    //Send data bytes to be written
    spiMaster[EEPROM2].writeBlock(writeBuffer,buflen);
    
    //Toggle CS line to end operation
    LATGbits.LATG9 = 1; /* set EEPROM2_nCS output high */
    spiMaster[EEPROM2].spiClose();
       
}

void EEPROM2_ReadBlock(uint8_t *readBuffer, uint8_t buflen, uint32_t startAddr)
{
    uint8_t addressBuffer[EEPROM2_ADDRBYTES];
    
    //Wait for write cycle to complete
    EEPROM2_WritePoll();
    
    EEPROM2_AddressAssign(addressBuffer,startAddr);
    
    spiMaster[EEPROM2].spiOpen();
    LATGbits.LATG9 = 0; /* set EEPROM2_nCS output low */
    
    //Send Read Command
    spiMaster[EEPROM2].exchangeByte(EEPROM2_READ);
    //Send Address bytes
    spiMaster[EEPROM2].exchangeBlock(addressBuffer,EEPROM2_ADDRBYTES);
    //Send dummy/NULL data to clock out data bytes from slave
    spiMaster[EEPROM2].exchangeBlock(readBuffer,buflen);
    
    LATGbits.LATG9 = 1; /* set EEPROM2_nCS output high */
    spiMaster[EEPROM2].spiClose();
}

static void EEPROM2_AddressAssign(uint8_t *addressBuffer, uint32_t byteAddr)
{
    uint8_t i = 0;
    uint8_t j = EEPROM2_ADDRBYTES - 1;
    uint32_t address = byteAddr;
    
    while(address > 0)
    {
        addressBuffer[j-i] = address & 0xFF;
        i++;
        address >>= 8;
    }
}

static void EEPROM2_WriteEnable(void)
{    
    spiMaster[EEPROM2].spiOpen();
    LATGbits.LATG9 = 0; /* set EEPROM2_nCS output low */

    //Send Write Enable command
    spiMaster[EEPROM2].exchangeByte(EEPROM2_WREN);
    
    LATGbits.LATG9 = 1; /* set EEPROM2_nCS output high */
    spiMaster[EEPROM2].spiClose();
}

static void EEPROM2_CheckStatusRegister(void)
{
    uint8_t check;
    //Check if WEL bit is set
    while(check != 2)
    {
        check = EEPROM2_ReadStatusRegister();
    }
}

static uint8_t EEPROM2_ReadStatusRegister(void)
{
    uint8_t statusByte;
    
    spiMaster[EEPROM2].spiOpen();
    LATGbits.LATG9 = 0; /* set EEPROM2_nCS output low */
    
    //Send Read Status Register Operation
    spiMaster[EEPROM2].exchangeByte(EEPROM2_RDSR);
    //Send Dummy data to clock out data byte from slave
    statusByte = spiMaster[EEPROM2].exchangeByte(0x00);
    
    LATGbits.LATG9 = 1; /* set EEPROM2_nCS output high */
    spiMaster[EEPROM2].spiClose();
    
    //return data byte read
    return(statusByte);
}

uint8_t EEPROM2_WritePoll(void)
{
    uint8_t pollByte;
    
    //Read the Status Register
    pollByte = EEPROM2_ReadStatusRegister();
    
    //pollByte = SSP1BUF;
    //Check if WEL and WIP bits are still set
    while(pollByte == 3)
    {
       pollByte = EEPROM2_ReadStatusRegister();
    }
    
    //return 1 if WEL and WIP bits are cleared and the write cycle is finished
    return(1);
}
