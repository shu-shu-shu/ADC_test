/*-------------------------*/
/* SSL OPEN SOURCE PROJECT */
/*----------------------------------------------------------------------*/
/* ABSTRACT   : iic communication                                       */
/* LAST UPDATE: 2016/06/22                                              */
/* CREATOR    : S.HOTTA                                                 */
/* NOTICE:                                                              */
/* ※MCP2515に特化したものである．                                      */
/* ※IIボーレートは97.7kHzに設定．                                      */
/* ※IICバスインタフェース仕様では通常モードが最大100kHZ                */
/* ※高速モードが最大400kHZ                                             */
/*----------------------------------------------------------------------*/


#include "iodefine.h"
#include "ssl_iic.h"

/************************************************/
/* dummy interrput function(ダミーリード用関数) */
/************************************************/
#pragma interrupt(dummy)
void dummy(void)
{
// Interrput error
}
/******************************************/
/* iic_init iic初期化の関数               */
/******************************************/

void iic_init(void)
{

    MST.CR1.BIT._IIC=0;		//モジュールストップを解除
	IIC.SCRX.BIT.IICE = 1; // IICの制御レジスタ，データレジスタのCPUアクセスを許可
	IIC.ICCR.BYTE = 0x89;	//2進数で1000100
		// ICE(7)=b'1 Enable I2C bus interface
		// IEIC(6)=b'0 Disables the interrupt
		// MST(5)=b'0 Slave mode
		// TRS(4)=b'0 Receive mode
		// ACKE(3)=b'1 Continuous data transfer is halted
		// BBSY(2)=b'0
		// IRIC(1)=b'0
		// SCP(0)=b'1 Start/stop condition issuance disabling
		// set I2C pin function
	//ここでSHをスレーブデバイスとして扱う場合には第一スレーブアドレスと第二スレーブアドレスの設定を行う．今回はマスターデバイスとしてしか使わないと想定しているため実装せず．
	PFC.PBCR1.WORD = 0x0c00; // SCL，SDAをIICポートとして使用
	PFC.PBCR2.WORD = 0x0000;
 	IIC.ICMR.BYTE = 0x38;	//転送レートを97.7kHzに設定,共通基板のSHのクロックに対して一番遅い値を使用．
		// MILS(7)=b'0 MSB first
		// WAIT(6)=b'0 A wait state is inserted between DATA and ACK
		// CKS2[2:0](5:3)=b'111 Transfer clock select
		// 156kH@(@P-fai40MHz,IICX=1)
		// 97.7kHz(@P-fai25MHz,IICX=1)これを設定
		// 39.1kH@(@P-fai10MHz,IICX=1)
	IIC.SCRX.BYTE = 0x39;	//IICレジスタへのCPUアクセス許可，転送ビットレート関連のビットを設定
		// IICX(5)=b'1 transfer-rate select,reference CKS bit
		// IICE(4)=b'1 Enables CPU access to the register
		// HNDS(3)=b'1 Set this bit to 1
		// STOPIM(0)=b'1 disables interrupt requests
		// Set device code,EEPROM address
	return ;

}




/*****************************************************************/
/* iic_write スレーブ側の送信データ設定		                     */			
/* argument1 ;t_data (unsigned char*)送信データを入れる配列	     */
/* argument2 ;t_data_num (unsigned char)送信データ個数 		     */
/* argument3 ;slave_address (unsigned char)スレーブアドレス      */
/* return ;1=OK/0=NG slave device NO_ACK(unsigned char)          */
/*****************************************************************/
unsigned char iic_write(unsigned char* t_data, int t_data_num, unsigned char slave_address)
{

	int i=0;
    while( IIC.ICCR.BIT.BBSY !=0 ); // SCL，SDAラインの状態判定
	IIC.ICCR.BYTE |= 0x30; // マスタ送信モードに設定(MST=1,TRS=1)

	IIC.ICCR.BYTE=((IIC.ICCR.BYTE & 0xfe) | 0x04); // 開始条件発行(BBSY=1,SCP=0)
 	while( IIC.ICCR.BIT.IRIC==0 ); // 開始条件生成待ち
	
	// Slave address+W Transmission
	IIC.EQU.ICE1.UN_ICDR = (unsigned char)(slave_address|IIC_DATA_W); // 送信データの設定（スレーブアドレスとW/R）
	IIC.ICCR.BIT.IRIC = 0; // IRICフラグをクリア，これは送信データの設定の直後に行わなければならない
	while( IIC.ICCR.BIT.IRIC==0 ); // 1バイト送信完了待ち
	
	if( IIC.ICSR.BIT.ACKB!=0 ){ // スレーブデバイスからのACKの有無チェック
		return (0); // ACKなしで戻り値0を返す
	}

	for (i=0; i < t_data_num; i++){	//コマンドの数だけ送信させる

		IIC.EQU.ICE1.UN_ICDR = (unsigned char)(t_data[i]); // data set
		IIC.ICCR.BIT.IRIC = 0; // clear IRIC
		while( IIC.ICCR.BIT.IRIC==0 ); // Wait 1byte transmitted
	
		if( IIC.ICSR.BIT.ACKB!=0 ){ // Test the acknowledge bit
			return (0); // no ACK
		}
	}


	return (1); // ACK OK
}


/*********************************************************************/
/* iic_read データ読み込みの関数                                     */	
/* argument1 ;r_data(unsigned char*)データを入れる配列	             */
/* argument2 ;r_data_num (unsigned char)データサイズ 		         */
/* argument3 ;slave_address (unsigned char)スレーブアドレス          */
/* return ;1=OK/0=NG slave device NO_ACK(unsigned char)				 */
/*********************************************************************/
unsigned char iic_read(unsigned char *r_data, unsigned char r_data_num, unsigned char slave_address)
{
	unsigned char ack; // ACKフラグ
	unsigned char count; // read data number
	unsigned char dummy; // dummy data

	IIC.ICCR.BIT.IRIC = 0; // IRICフラグをクリア
 	while(PB.DR.BIT.B2!=0); // SCL端子がLowレベルになるまで待機
	IIC.ICCR.BYTE |= 0x30; // マスター送信モードに設定(MST=1,TRS=1)
	IIC.ICCR.BYTE=((IIC.ICCR.BYTE & 0xfe)|0x04); // 開始条件発行(BBSY=1,SCP=0)
	while( IIC.ICCR.BIT.IRIC==0 ); // 開始条件生成待ち
	IIC.EQU.ICE1.UN_ICDR = (unsigned char)(slave_address|IIC_DATA_R); // スレーブアドレスのセット
	IIC.ICCR.BIT.IRIC = 0; // IRICフラグのクリア

	while( IIC.ICCR.BIT.IRIC==0 ); // 1バイト送信完了待ち
	
	if( IIC.ICSR.BIT.ACKB!=0 ){ // スレーブデバイスからのACKの有無チェック
		ack = 0; // ACKなし
	}
	else{
		ack = 1;
	}
	
	if(ack==1){
		// マスター受信モードに設定 (HNDS=1,WAIT=0)
		IIC.ICCR.BIT.TRS = 0; // 受信モード設定(TRS=0)
		IIC.ICMR.BIT.WAIT = 0; // ウェイトなし
		IIC.ICSR.BIT.ACKB = 0; // ACKデータをクリア
		IIC.SCRX.BIT.HNDS = 1; // 連続受信動作なし
		IIC.ICCR.BIT.IRIC = 0; // IRICフラグをクリア

		// データ受信開始
		if(r_data_num>1){ // nバイトのデータを読み込む場合 (n>1)
			dummy = IIC.EQU.ICE1.UN_ICDR; // ダミーリード
			while( IIC.ICCR.BIT.IRIC==0 ); // 1バイト受信待ち
			IIC.ICCR.BIT.IRIC = 0; // IRICフラグをクリア
			
			for( count=2; count<r_data_num; count++ ){ 
			//*r_data = IIC.ICDR.BYTE; // read receive data
			*r_data = IIC.EQU.ICE1.UN_ICDR; // 受信データの読み込み
			while( IIC.ICCR.BIT.IRIC==0 ); // 1バイト受信待ち
			IIC.ICCR.BIT.IRIC = 0; // IRICフラグのクリア
			r_data++;
			}
		}  
	
		IIC.ICSR.BIT.ACKB = 1; // 最後の受信用アクノレッジデータ設定
		
		if(r_data_num==1){ // 受信の1フレーム目が最終データの場合，受信開始のためのダミーリード
			dummy = IIC.EQU.ICE1.UN_ICDR; 
		}
		else{ // 後ろから2フレーム目のデータ読み込み (n>1)
			*r_data = IIC.EQU.ICE1.UN_ICDR; // 
			r_data++;
		}
		while( IIC.ICCR.BIT.IRIC==0 ); // 1バイト受信待ち
		IIC.ICCR.BIT.IRIC = 0; // IRICフラグをクリア
		IIC.ICCR.BIT.TRS = 1; // 最後の受信データの読み込み
		*r_data = IIC.EQU.ICE1.UN_ICDR; 
	}
	return(ack);
}


/******************************************/
/* iic_init iic 停止の関数                */
/******************************************/
void iic_stop(void){
		// 停止条件発行
	IIC.ICCR.BYTE = IIC.ICCR.BYTE & 0xfa; // 停止条件発行
}
	