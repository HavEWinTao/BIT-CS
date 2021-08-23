package Frames;

import GBN.Config;

public class Data extends Frame{
	public int seq;        //包编号
	public byte[] data;    //数据
	public int crc16;//CRC校验码

	public Data(int seq, byte[] data) {
		if(data.length> Config.DataSize){
			try {
				throw new Exception("Data is too long. "+data.length+"  "+ Config.DataSize);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		this.type = TYPE.DATA;
		this.seq = seq;
		this.data = data;
		this.crc16 = CRC16.crc_16_CCITT_False(data);
	}
	public boolean crc16_Check(){
		return CRC16.crc_16_CCITT_Check(crc16,data);
	}
}
