/*
 * main.c
 *
 * Created: 1/15/2022 3:15:35 PM
 *  Author: advai
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void USART_Init(unsigned int ubrr)
{
	UBRR0H = (unsigned int)(ubrr>>8);
	UBRR0L = (unsigned int)ubrr;
	
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	UCSR0C = (0<<USBS) | (3<<UCSZ0);
}
void transmit(unsigned char data)
{
	while( !(UCSR0A & (1<<UDRE0))){
		;
	}
	UDR0 = data;
}

unsigned char receive()
{
	while( !(UCSR0A & (1<<RXC))){
		;
	}
	return UDR0;
}

void I2C_Init(int addr){
	TWAR=addr<<1;
	TWCR=1<<TWEN;
	TWSR=0x00;
	TWBR=0x0C;
}

void I2C_SlaveWrite(unsigned int slavAdr){
	TWCR=1<<TWEN|1<<TWINT;
	TWDR=slavAdr<<1|0<<0;
	while(!(TWCR & (1<<TWINT)));
}

void I2C_SendMessage(unsigned char data){
	TWDR=data;
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
}

void I2C_Receive(){
	TWCR=1<<TWEN|1<<TWEA|1<<TWINT;
	while(!(TWCR & (1<<TWINT)));
}

void I2C_Stop(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while(!(TWCR & (1<<TWINT)));
}



void SlaveReceiver(){
	//sla +w has been received, ack sent from slave
	if((TWSR & 0xF8)==0x60){
		TWCR=1<<TWEN|1<<TWEA|1<<TWINT;
		while(!(TWSR & (1<<TWINT)));
		transmit(TWDR);
	}
	if(((TWSR & 0xF8)==0x80)){
		TWDR=0;
	}
}
unsigned int counter=0;
char msg[]="Message sent to master";
void SlaveTransmitter(){
	//when slave gets its SLA+R and it sends out an ack
	if((TWSR & 0xF8)==0xA8){
		if(counter<sizeof(msg)){
			I2C_SendMessage(msg[counter]);
			counter++;
		}
	}
	//when last character is sent out
	if((TWSR & 0xF8)==0xB8){
		TWCR=1<<TWEN|1<<TWEA;
	}
	
}

int main(void)
{
	I2C_Init(2<<0);
	USART_Init(MYUBRR);
	TWCR=1<<TWEN|1<<TWEA;
	while(1){
	SlaveReceiver();}
	SlaveTransmitter();
}
