#include <stdio.h>
#include "iodefine.h"
#include "ssl_ad12.h"




//connfiguration register のD2=b'0,D1=b'0,D0=b'0に固定し，アラートは使用せず
unsigned char ad12_init(unsigned char select_ch, unsigned char adc_slave_address)	//初期化の関数
{
	unsigned char t_data[4];//送信コマンド，[0]アドレスレジスタでコンフィグレジスタを指す，[1],[2]コンフィグレジスタに書き込み（2バイト）
	unsigned char ack = 0;
	unsigned char i = 0;

	t_data[0]=CONFIGURATION_REGISTER;
	t_data[1]=(select_ch&0xFF00)>>8;
	t_data[2]=(FILR_BIT|select_ch)&0x00FF;



	//コンバートピンを下げる
	PFC.PDIORH.BIT.B25= 0x01;	//コンバートピンのPD25を入出力に設定
	PFC.PDCRH1.BIT.PD25MD= 0x00;		//PD25を出力に設定
	PD.DR.BIT.B25= 0x00;	//ピンをLoに
	
	iic_init();		//実際は衛星全体でiic_initを管理したほうがいい，最終的にはこの関数の中から外したい
	ack = iic_write(t_data, 3, adc_slave_address);
	if(ack==0){
		return 0;
	}
	for(i=0; i <=3; i++){	//コマンド用配列の初期化
		t_data[i]=0;
	}
	t_data[0]=SEQUENCE_READ;	//今度は1バイト書込しか行わない

	ack = iic_write(t_data, 1, adc_slave_address);	//t_dataの4配列目にSEQUENCE_READを入れて一度の書込にしても良いが手続きとしては二種類行っているため理解のため分離
	if(ack==0){
		return 0;
	}
	else{
		return 1;
	}
	
}

void ad12_set()	//どのCHを使用するかの設定
{
	//I2Cwrite
}



//ad_data[]の配列は[48]以上であること，ちなみに7以下はつかっていない，こことSHを合体させても良い
unsigned char ad12_conv(unsigned long int *ad_data, unsigned char ch_num, unsigned char adc_slave_address)		//変換開始，配列のサイズは3つでその内2つを使用
{
	unsigned char i;
	unsigned char r_data[3];	//iic_read用の読み込みデータ格納変数
	for( i=1; i <= ch_num; i++){		//設定したチャンネルの数だけサイクル変換での取得をする
		iic_read(r_data, 2, adc_slave_address);		//スレーブへの読み込み
		switch((r_data[0]>>4)&0x07){		//まず読み込んだデータからIDを取得それでどのCHのデータかを選別
		case 0x00:
			switch(adc_slave_address){		//次に5つあるADCからどのADCを使っているかでポートがわかるため，取得したデータをポート単位で格納
			case ADC_1:
				ad_data[8]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];		//MSBデータのADデータとは関係のない上位5ビットをマスクして8ビット左シフト＋LSB
				break;
			case ADC_2:
				ad_data[16]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[24]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[32]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[40]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x01:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[9]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[17]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[25]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[33]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[41]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x02:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[10]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[18]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[26]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[34]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[42]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];		
				break;
			}
			break;
		case 0x03:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[11]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[19]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[27]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[35]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[43]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x04:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[12]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[20]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[28]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[36]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[44]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x05:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[13]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[21]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[29]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[37]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[45]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x06:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[14]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[22]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[30]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[38]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[46]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x07:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[15]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[23]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[31]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[39]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[47]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		}						
	}
	return 1;
}

void ad12_stop()
{
	iic_stop();
}


