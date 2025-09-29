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
#include "delay.h"
#include "EEPROM2_app.h"

uint8_t		writeBuffer[] = {0x1A, 0x2A, 0x4A, 0x8A};
uint8_t     readBuffer[10];
uint8_t     addressBuffer[] = {0xAB,0x00,0x10}; // Store the address you want to access here
uint8_t     readByte;
 
void EEPROM2_example(void)
{
    //Writes one byte to the address specified
    EEPROM2_WriteByte(0xA5,0x10AB01);

    //Wait for write cycle to complete
    EEPROM2_WritePoll();

    //Reads one byte of data from the address specified
    readByte = EEPROM2_ReadByte(0x10AB01);

    //Intermission
    DELAY_milliseconds(10);

    //Writes the data in writeBuffer beginning from the address specified
    EEPROM2_WriteBlock(writeBuffer,4,0x10AB02);

    //Wait for write cycle to complete
    EEPROM2_WritePoll();

    //Reads specified number of data bytes into the readBuffer array beginning from the address indicated
    EEPROM2_ReadBlock(readBuffer,4,0x10AB02);
}
