#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<pic18f4520.h>
#include<xc.h>
#include"esp.h"
#include"lcd.h"
#include"eeprom.h"
#include"adc.h"

#define _XTAL_FREQ 8000000
#define Baud_rate 9600
#define BOTH_STATION_AND_ACCESPOINT		3

#define DOMAIN				"api.thingspeak.com"
#define PORT				"80"
#define API_WRITE_KEY		"3E8O0Q0AY86HZKW5" //esp project_thingspeak
#define CHANNEL_ID			"527679"

#define SSID				"PRADIV SOLUTIONS"
#define PASSWORD			"pradiv777"


void ADC_Init()
{    
    TRISA = 0xFF;	/* Set as input port */
    ADCON1 = 0x0E;	/* Ref vtg is VDD and Configure pin as analog pin */
    ADCON2 = 0x92;	/* Right Justified, 4Tad and Fosc/32. */
    ADRESH=0;		/* Flush ADC output Register */
    ADRESL=0;   
}   

int ADC_Read(int channel)
{
    int digital;
    //channel&=0x3F;            // truncate channel to 6 bits 
    /* Channel 0 is selected i.e.(CHS3CHS2CHS1CHS0=0000) & ADC is disabled */
    ADCON0 =(ADCON0 & 0b11000011)|((channel<<2) & 0b00111100);  //195&60 in decimal
    
    ADCON0 |= ((1<<ADON)|(1<<GO));	/*Enable ADC and start conversion*/

    /* Wait for End of conversion i.e. Go/done'=0 conversion completed */
    while(ADCON0bits.GO_nDONE==1);

    digital = (ADRESH*256) | (ADRESL);	/*Combine 8-bit LSB and 2-bit MSB*/
    return(digital);
}

unsigned char response;

       unsigned char response;
       float energy ,voltage ,current ,v,c;  // float 4 bytes of data
       unsigned int Digital_vaue=0;  // ADC converted digital values
       unsigned int p_Digital_vaue=0; //past digital value
       unsigned int ADCResult= 0;  //intializing ADC result 
       unsigned int i;                  //i for address
       char voltageTxt;
       char currentTxt;
       char energytxt;
       char buffer[3];
       char data;

void main()
{
    unsigned char address=0x00;  //EEPROM address
   //buffer = (char*)malloc(sizeof(char)*10);
    int null=0;
    ADCON1 = 0x7A; // Configure all ports with analog function as digital except RA2
    OSCCON=0x72;		/* Use internal Osc. with 8 Mhz Frequency */
  //CMCON |= 7; // Disable comparators
    ADCON0 = 0X01;
    
  unsigned int a;
  TRISD = 0x00;
  Lcd_Init();
 ADC_Init();
 
 ///////////////////  WIFI MODULE   //////////////////
    Initialize_ESP8266();
    TRISD = 0x00; //set as out put ports
     Lcd_Start();//----Initialize LCd
   // Initialize_ESP8266() ; //initialise esp8266
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("Welcome");
     Lcd_Set_Cursor(2,1);
    Lcd_Print_String("pradiv solution");
    __delay_ms(1500);
    Lcd_Clear();
    
    do
    {
     //_esp8266_waitResponse();
        Lcd_Set_Cursor(1,1);
    Lcd_Print_String(response);
    _esp8266_print("AT+CWJAP");//AT Command for connect to AP(access point)
     Lcd_Print_String("ESP not found");
    }while (!esp8266_isStarted()); //wait till the ESP send back "OK"
    Lcd_Set_Cursor(1,1);
    __delay_ms(1500);  
    Lcd_Clear();
     // ESP communication successful
    
    /*Put the module in Soft AP  mode*/
    esp8266_mode(3);
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("ESP set as AP");
    __delay_ms(1500);
    Lcd_Clear();
    /*Module set as AP */
    
   //esp8266_start(TCP,);
    
    /*Configure the AP name and Password*/
    esp8266_config_softAP("PRADIV SOLUTIONS","pradiv777");
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("AP configured");
    __delay_ms(1500);
    /*AP configured*/
      //////////////   END WIFI MODULE   ///////////////////
 
  while(1)
  {
       for(i=0;i>=Digital_vaue;i++){
                ADCResult = ADC_Read(0b00000101); //Read result Channel AN0 selected, RA0 connected to cal LED 
             // For Vref+ = Vdd, Vref- = Vss and 10 bit resolution 5V is refrence voltage
                Digital_vaue = (ADCResult/ 5*1024);//For a 10 bit resolution ADC,possible to divide up to 1024 (2^10) voltages. 
       
                voltage=Digital_vaue*4800;//voltage value will be in volts
                voltageTxt=(char)voltage;
         
              //current = (ADCResult*0.00197);
                current= voltageTxt*3.5*(10^(-6));  //3.5uA=1.225V
                currentTxt=(char) current;
                v=(float)voltageTxt;
                c=(float)currentTxt;
            
                energy=(v*c*0.02);//Here we can not multiply array elements directly, so we are using C and V
                energytxt=(char)energy;
         
              //for(i=ADCResult;i>p_ADCResult;i++)
         
                if(Digital_vaue != p_Digital_vaue){
                buffer[0]=v;
                buffer[1]=c;
                buffer[2]=energytxt;
                p_Digital_vaue=Digital_vaue;
                
                //internal eeprom
                //writeEEPROM(address, buffer);      //writing data to first memory location and then goes upto last location
                //address++;
                //__delay_ms(10000);
                
                ////external eeprom
                I2CInit();
	            I2CStart();
             	//I2CSend(0xA0);
                
	            I2CSend(address);
	            I2CSend(buffer[0]);
                address++;
                I2CSend(buffer[1]);
                address++;
                I2CSend(buffer[2]);
                address++;
	            __delay_ms(10000); 
                }
                  else{
                    //external eeprom
                      I2CSend(null);
                      address++;
                      //internal eeprom
                      // writeEEPROM(address, null);
                       //address++;
                      }
                 Lcd_Set_Cursor(1,1);
                 Lcd_Print_String("Voltage:");
                 Lcd_Print_String(buffer[0]+"V" );
                 Lcd_Set_Cursor(1,1);
                 Lcd_Print_String("Current:");
                  Lcd_Print_String(buffer[1]+"A" );
                 Lcd_Print_String("Energy:");
                  Lcd_Print_String(buffer[2]+"J" );
                  
                if(address!=0X7D00){
                    for(i=0;i<3;i++){
            I2CRead(data);//Reads 1 byte from I2C and writes it to the array
           //esp8266_start(ESP8266_TCP, 216.3.128.12 , 80);
             esp8266_send(data);
             address++;
                    }
                }
                else{
                eErase();
                }
                
      
            }
      }
 }
