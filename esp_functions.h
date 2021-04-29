
#ifndef ESP_FUNCTIONS_H
#define	ESP_FUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include<string.h>
#include<stdarg.h>

#define ESP8266_STATION 0x01
#define ESP8266_SOFTAP 0x02

#define ESP8266_TCP 1
#define ESP8266_UDP 0

#define ESP8266_OK 1
#define ESP8266_READY 2
#define ESP8266_FAIL 3
#define ESP8266_NOCHANGE 4
#define ESP8266_LINKED 5
#define ESP8266_UNLINK 6


///***______________________ESP module Function Declarations__________________**///
void _esp8266_putch(unsigned char);
unsigned char _esp8266_getch(void);
/** Function prototypes **/
bit esp8266_isStarted(void);        // Check if the module is started (AT)
bit esp8266_restart(void);          // Restart module (AT+RST)
void esp8266_echoCmds(bool);        // Enabled/disable command echoing (ATE)
// WIFI Mode (station/softAP/station+softAP) (AT+CWMODE)
void esp8266_mode(unsigned char);
// Connect to AP (AT+CWJAP)
unsigned char esp8266_connect(unsigned char*, unsigned char*);
// Disconnect from AP (AT+CWQAP)
void esp8266_disconnect(void);
// Local IP (AT+CIFSR)
void esp8266_ip(char*);
// Create connection (AT+CIPSTART)
bit esp8266_start(unsigned char protocol, char* ip, unsigned char port);
// Send data (AT+CIPSEND)
bit esp8266_send(unsigned char*);
// Receive data (+IPD)
void esp8266_receive(unsigned char*, uint16_t, bool);
/** Functions for internal use only **/
// Print a string to the output
void _esp8266_print(unsigned const char *);
// Wait for a certain string on the input
inline uint16_t _esp8266_waitFor(unsigned char *);
// Wait for any response on the input
inline unsigned char _esp8266_waitResponse(void);
void Lcd_Set_Cursor(char , char b);
void Lcd_Print_Char(char);
void Lcd_Print_String(char *);
//********__________________End of Function Declaration_________________********///



#ifdef	__cplusplus
}
#endif

#endif	/* ESP_FUNCTIONS_H */

