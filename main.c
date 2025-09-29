/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.5
        Device            :  PIC24EP512GU810
    The generated drivers are tested against the following:
        Compiler          :  XC16 v2.10
        MPLAB 	          :  MPLAB X v6.05
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
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

/**
  Section: Included Files
*/
#include <stdio.h>
#include <string.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/EEPROM2_app.h"

#define LINE_BUF_SIZE 32

static inline void uart_write(uint8_t b) {
    while (!UART1_IsTxReady()) { }
    UART1_Write(b);
}

/*  Main application */
int main(void)
{
    SYSTEM_Initialize();

    printf("\r\nEEPROM Bucket Tester. \r\n");
    printf("Type then hit 'enter' to save to external EEPROM memory. \r\n\n");

    uint8_t line[LINE_BUF_SIZE];
    uint8_t readBuffer[LINE_BUF_SIZE];
    uint32_t addr = 0x10AB01;
    int len = 0;
    int last_was_cr = 0;
    
    // Reads and display last written word.
    EEPROM2_ReadBlock(readBuffer, LINE_BUF_SIZE, addr);
    printf("Reading last written bytes from mem block (0 - %d).\r\n", LINE_BUF_SIZE);
    printf("Data --> ");
    for (int i = 0; i < LINE_BUF_SIZE; i++) printf("%c", readBuffer[i]);
    printf("\r\n\n");

    while (1) {
        if (UART1_IsRxReady()) {
            uint8_t ch = UART1_Read();

            if (ch == '\r') {
                // Treat CR as end-of-line (common on Windows/terminals sending CRLF)
                if (len == 0) continue;
                
                uart_write('\r');
                uart_write('\n');

                line[len] = '\0';
                
                // Writes the data in writeBuffer beginning from the address specified
                uint8_t writeBuffer[len];
                memcpy(writeBuffer, &line[0], len);
                EEPROM2_WriteBlock(writeBuffer, len, addr);
                // Wait for write cycle to complete
                EEPROM2_WritePoll();
                // Reads written data from EEPROM mem.
                uint8_t readBuffer[len];
                EEPROM2_ReadBlock(readBuffer, len, addr);
                // Writes data to terminal.
                printf("Reading back: ");
                for (int i = 0; i < len; i++) printf("%c", readBuffer[i]);
                printf("\r\n");

                len = 0;
                last_was_cr = 1;
            } else if (ch == '\n') {
                // If LF follows CR, ignore (CRLF); otherwise treat LF as end-of-line
                if (!last_was_cr) {
                    if (len == 0) continue;
                    // Echo LF as line ending for Unix-style terminals
                    uart_write('\n');

                    line[len] = '\0';
                     
                    // Writes the data in writeBuffer beginning from the address specified
                    uint8_t writeBuffer[len];
                    memcpy(writeBuffer, &line[0], len);
                    EEPROM2_WriteBlock(writeBuffer, len, addr);
                    // Wait for write cycle to complete
                    EEPROM2_WritePoll();
                    // Reads written data.
                    uint8_t readBuffer[len];
                    EEPROM2_ReadBlock(readBuffer, len, addr);
                    // write data read from EEPROM Mem.
                    printf("Reading back: ");
                    for (int i = 0; i < len; i++) printf("%c", readBuffer[i]);
                    printf("\r\n");

                    len = 0;
                }
                last_was_cr = 0;
            } else if (ch == '\b' || ch == 0x7F) {
                // Backspace/Delete
                if (len > 0) {
                    len--;
                    // Visual backspace: move left, erase, move left
                    uart_write('\b');
                    uart_write(' ');
                    uart_write('\b');
                }
                last_was_cr = 0;
            } else {
                last_was_cr = 0;
                if (len < (LINE_BUF_SIZE - 1)) {
                    line[len++] = ch;
                    // Echo typed character
                    uart_write(ch);
                } else {
                    // Buffer full: beep and ignore chars until newline
                    uart_write('\a'); // BEL
                }
            }
        }
    }

    return 1;
}
/**
 End of File
*/

