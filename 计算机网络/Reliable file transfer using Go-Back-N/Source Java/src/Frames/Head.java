package Frames;
import GBN.Config;

public class Head extends Frame {
	public int nbuffered;
	public int seq;
	public int crc;
	public String name;

	public Head(int nbuffered,String name){
		if(name.length()> Config.DataSize){
			try {
				throw new Exception("Data is too long. "+name.length()+"  "+ Config.DataSize);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		this.type = TYPE.HEAD;
		this.nbuffered = nbuffered;
		this.name = name;
		this.seq = 0;
		this.crc = CRC16.crc_16_CCITT_False(name.getBytes());
	}
}
