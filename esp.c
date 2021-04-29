#include"lcd.h"
#include"esp_functions.h"
#include"adc.h"


#define Baud_rate 9600

#define BOTH_STATION_AND_ACCESPOINT		3

#define DOMAIN				"api.thingspeak.com"
#define PORT				"80"
#define API_WRITE_KEY		"3E8O0Q0AY86HZKW5" //esp project_thingspeak
#define CHANNEL_ID			"527679"

#define SSID				"PRADIV SOLUTIONS"
#define PASSWORD			"pradiv777"


unsigned char response;
//***Modified Codes**//
void Initialize_ESP8266(void)
{
    //****Setting I/O pins for UART****//
    TRISC6 = 0; // TX Pin set as output
    TRISC7 = 1; // RX Pin set as input
    //________I/O pins set __________//
    
    /**Initialize SPBRG register for required 
    baud rate and set BRGH for fast baud_rate**/
        SPBRG = ((_XTAL_FREQ/16)/Baud_rate) - 1;

    BRGH  = 1;  // for high baud_rate
    //_________End of baud_rate setting_________//
    
    //****Enable Asynchronous serial port*******//
    SYNC  = 0;    // Asynchronous
    SPEN  = 1;    // Enable serial port pins
    //_____Asynchronous serial port enabled_______//
    //**Lets prepare for transmission & reception**//
    TXEN  = 1;    // enable transmission
    CREN  = 1;    // enable reception
    //__UART module up and ready for transmission and reception__//
    
    //**Select 8-bit mode**//  
    TX9   = 0;    // 8-bit reception selected
    RX9   = 0;    // 8-bit reception mode selected
    //__8-bit mode selected__//     
}
//________UART module Initialized__________//
 


//**Function to send one byte of date to UART**//
void UART_send_char(char bt)  
{
    while(!TXIF);  // hold the program till TX buffer is free
    TXREG = bt; //Load the transmitter buffer with the received value
}
//_____________End of function________________//

//**Function to send one byte of date to UART**//
void _esp8266_putch(char bt)  
{
    while(!TXIF);  // hold the program till TX buffer is free
    TXREG = bt; //Load the transmitter buffer with the received value
}
//_____________End of function________________//
 
//Function to get one byte of date from UART
char _esp8266_getch()   
{
    if(OERR) // check for Error 
    {
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }
    
    while(!RCIF);  // hold the program till RX buffer is free
   
    return (RCREG); //receive the value and send it to main function
}
//_____________End of function________________//
 
//**Function to configure soft_AP**//
unsigned char esp8266_config_softAP(unsigned char* softssid, unsigned char* softpass) {
    _esp8266_print("AT+CWS  AP=\"");//set a parameters of AP
    _esp8266_print(softssid);
    _esp8266_print("\",\"");
    _esp8266_print(softpass);
    _esp8266_print("\",5,3\r\n");
    return _esp8266_waitResponse();
}
//___________End of function______________//

bit esp8266_isStarted(void) {
    _esp8266_print("AT\r\n");
    return (_esp8266_waitResponse() == ESP8266_OK);
}

/*bit esp8266_restart(void) {
    _esp8266_print("AT+RST\r\n");
    if (_esp8266_waitResponse() != ESP8266_OK) {
        return false;
    }
    return (_esp8266_waitResponse() == ESP8266_READY);
}
*/
/*void esp8266_echoCmds(bool echo) {
    _esp8266_print("ATE");
    if (echo) {
        _esp8266_putch('1');
    } else {
        _esp8266_putch('0');
    }
    _esp8266_print("\r\n");
    _esp8266_waitFor("OK");
}
*/
void esp8266_mode(unsigned char mode) {
    _esp8266_print("AT+CWMODE=3");//wifi mode
    _esp8266_putch(mode + '3');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
}

/*unsigned char esp8266_connect(unsigned char* ssid, unsigned char* pass) {
    _esp8266_print("AT+CWJAP=\"");
    _esp8266_print(ssid);
    _esp8266_print("\",\"");
    _esp8266_print(pass);
    _esp8266_print("\"\r\n");
    return _esp8266_waitResponse();
}*/

/*void esp8266_disconnect(void) {
    _esp8266_print("AT+CWQAP\r\n");
    _esp8266_waitFor("OK");
}
*/
/*void esp8266_ip(unsigned char* store_in) {
    _esp8266_print("AT+CIFSR\r\n");
    unsigned char received;
    do {
        received = _esp8266_getch();
    } while (received < '0' || received > '9');
    for (unsigned char i = 0; i < 4; i++) {
        store_in[i] = 0;
        do {
            store_in[i] = 10 * store_in[i] + received - '0';
            received = _esp8266_getch();
        } while (received >= '0' && received <= '9');
        received = _esp8266_getch();
    }
    _esp8266_waitFor("OK");
}*/

bit esp8266_start(unsigned char protocol, char* ip, unsigned char port) {
    _esp8266_print("AT+CIPSTART=\"");
    if (protocol == ESP8266_TCP) {
        _esp8266_print("TCP");
    } else {
        _esp8266_print("UDP");
    }
    _esp8266_print("\",\"");
    _esp8266_print(ip);
    _esp8266_print("\",");
    unsigned char port_str[5] = "\0\0\0\0";
    sprintf(port_str, "%u", port);
    _esp8266_print(port_str);
    _esp8266_print("\r\n");
    if (_esp8266_waitResponse() != ESP8266_OK) {
        return 0;
    }
    if (_esp8266_waitResponse() != ESP8266_LINKED) {
        return 0;
    }
    return 1;
}

/*bit esp8266_send(unsigned char* data) {
    unsigned char length_str[6] = "\0\0\0\0\0";
    sprintf(length_str, "%u", strlen(data));
    _esp8266_print("AT+CIPSEND=");
    _esp8266_print(length_str);
    _esp8266_print("\r\n");
    while (_esp8266_getch() != '>');
    _esp8266_print(data);
    if (_esp8266_waitResponse() == ESP8266_OK) {
        return 1;
    }
    return 0;
}
*/
/*void esp8266_receive(unsigned char* store_in, uint16_t max_length, bool discard_headers) {
    _esp8266_waitFor("+IPD,");
    uint16_t length = 0;
    unsigned char received = _esp8266_getch();
    do {
        length = length * 10 + received - '0';
        received = _esp8266_getch();
    } while (received >= '0' && received <= '9');

    if (discard_headers) {
        length -= _esp8266_waitFor("\r\n\r\n");
    }

    if (length < max_length) {
        max_length = length;
    }

    uint16_t i;
    for (i = 0; i < max_length; i++) {
        store_in[i] = _esp8266_getch();
    }
    store_in[i] = 0;
    for (; i < length; i++) {
        _esp8266_getch();
    }
    _esp8266_waitFor("OK");
}*/

void _esp8266_print(unsigned const char *ptr) {
    while (*ptr != 0) {
        _esp8266_putch(*ptr++);
    }
}

/*inline uint16_t _esp8266_waitFor(unsigned char *string) {
    unsigned char so_far = 0;
    unsigned char received;
    uint16_t counter = 0;
    do {
        received = _esp8266_getch();
        counter++;
        if (received == string[so_far]) {
            so_far++;
        } else {
            so_far = 0;
        }
    } while (string[so_far] != 0);
    return counter;
}
*/

//Function to convert string to byte
void UART_send_string(char* st_pt)
{
    while(*st_pt) //if there is a char
        UART_send_char(*st_pt++); //process it as a byte data
}
//___________End of function______________//

    

inline unsigned char _esp8266_waitResponse(void) {
    unsigned char so_far[6] = {0,0,0,0,0,0};
    unsigned const char lengths[6] = {2,5,4,9,6,6};
    unsigned const char* strings[6] = {"OK", "ready", "FAIL", "no change", "Linked", "Unlink"};
    unsigned const char responses[6] = {ESP8266_OK, ESP8266_READY, ESP8266_FAIL, ESP8266_NOCHANGE, ESP8266_LINKED, ESP8266_UNLINK};
    unsigned char received;
    unsigned char response;
    bool continue_loop = true;
    while (continue_loop) {
        received = _esp8266_getch();
        for (unsigned char i = 0; i < 6; i++) {
            if (strings[i][so_far[i]] == received) {
                so_far[i]++;
                if (so_far[i] == lengths[i]) {
                    response = responses[i];
                    continue_loop = false;
                }
            } else {
                so_far[i] = 0;
            }
        }
    }
    return response;
}
/*
char message[] = "x=   W";
char *x = "00";
void Display_x(unsigned int num) {
 
 x[5] = (num/100000)%10 + 48;    //  lakh digit
 x[4] = (num/10000)%10 + 48;    //  ten thousand digit
 x[3] = (num/1000)%10 + 48;    // thousand  digit
 x[2] = (num/100)%10 + 48;    //  hundred digit   
 x[1] = (num/10)%10 + 48;    //  tens digit
 x[0] =  num%10     + 48;    //  unit digit
 // Display Frequency on LCD
 Lcd_Print_String(1, 11, x);
 
}
*/

 char ReadEEPROM_DATA;
       unsigned char eepromData;
           unsigned char eepromAddress;
            int num = 0;          
            unsigned char eepromAddress;

            //int ADCResult=0;
            float energy ,voltage ,current ,v,c;
            unsigned int ADCResult= 0;
      
            char voltageTxt[15];
       char currentTxt[15];
       char energytxt[15];

void main()
{
    ADCON1 = 0x7A; // Configure all ports with analog function as digital except RA2
 OSCCON=0x72;		/* Use internal Osc. with 8 Mhz Frequency */
//CMCON |= 7; // Disable comparators
ADCON0 = 0X01;
    
    
    Initialize_ESP8266();
    TRISD = 0x00;
    Lcd_Start();//----Initialize LCd
    Initialize_ESP8266() ; 
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("Welcome");
    Lcd_Set_Cursor(2,1);
    Lcd_Print_String("pradiv solution");
    __delay_ms(1500);
    Lcd_Clear();
    
    /*Check if the ESP_PIC communication is successful*/
    do
    {
    //_esp8266_waitResponse();
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String(response);
    _esp8266_print("AT+CWJAP");
    Lcd_Print_String("ESP not found");
    }while (!esp8266_isStarted()); //wait till the ESP send back "OK"
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("ESP is connected");
    __delay_ms(1500);   
    Lcd_Clear();
    /*Yes ESP communication successful*/
    
    /*Put the module in Soft AP  mode*/
    esp8266_mode(3);
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("ESP set as AP");
    __delay_ms(1500);
    Lcd_Clear();
    /*Module set as AP */
    
   // esp8266_start(TCP,);
    
    /*Configure the AP name and Password*/
    esp8266_config_softAP("PRADIV SOLUTIONS","pradiv777");
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("AP configured");
    __delay_ms(1500);
    /*AP configured*/
       
    
    ADC_Init();
    do{
    adc();
    }while(ADC_Read()!=0);
    
}
void adc(){
       ADCResult = ADC_Read(); //Read result
 voltage=(ADCResult*5000.0)/1024.0;
       voltage=voltage/1000;
         voltageTxt=(char)voltage;
       
          current = (ADCResult*0.00197);
          currentTxt=(char) current;
                  v=(float)voltageTxt;
         c=(float)currentTxt;
         energy=(v*c*0.02);
         energytxt=(char)energy;

         Lcd_Set_Cursor(1,1);
       Lcd_Print_String("Energy :" energytxt "J");
        Lcd_Set_Cursor(2,1);
       Lcd_Print_String( "voltage :"voltageTxt "V");
        Lcd_Set_Cursor(2,10);
       Lcd_Print_String("current :"currentTxt "A");
       while(1){
           int x;
           x=voltageTxt;
           num=x;
          if( (num%10)!=voltageTxt);
          voltageTxt++;
       
       }
       
}

    
    
    
    /* frequency count using counter
    CMCON0 = 0x07;    // Disable Comparators
 ANSEL = 0x00;     // Disable analog channels
 TRISC = 0x00;     // PORTC O/P
 TRISA = 0b00001100; // RA2/T0CKI input, RA3 is I/P only
 OPTION_REG = 0b00101000; // Prescaler (1:1), TOCS =1 for counter mode
 //Lcd_Init();                 // Initialize LCD
 Lcd_Cmd(Lcd_Clear);        // CLEAR display
 Lcd_Cmd(Lcd_Set_Cursor);   // Cursor off
 Lcd_Print_String(1,1,message);      // Write message1 in 1st row
 do {
 
  TMR0=0;
  __delay_ms(1000);  // Delay 1 Sec
  Display_x(TMR0);
 } while(1);  // Infinite loop
*/
