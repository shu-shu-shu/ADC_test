#ifndef SSL_IIC_H
#define SSL_IIC_H


/*----*/
/*’è‹`*/
/*----*/
#define IIC_DATA_W 0x00		//WRITE DATA: b'0
#define IIC_DATA_R 0x01		//READ DATA : b'1


/*--------------------*/
/*Prototype Declataion*/
/*--------------------*/

void dummy(void);
void iic_init(void);
unsigned char iic_read(unsigned char* r_data,unsigned char r_data_num,unsigned char slave_address);
unsigned char iic_write(unsigned char *t_data, int t_data_num, unsigned char slave__address);
void iic_stop(void);
#endif