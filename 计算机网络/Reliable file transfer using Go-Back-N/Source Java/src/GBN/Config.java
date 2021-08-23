package GBN;

public class Config {
	//使用的端口
	public static int UdpPort = 4299; //port

	//协议参数
	public static int nThreads = 10;	//最大线程
	public static int DataSize = 2048;	//数据帧大小
	public static int SwSize = 7;		//窗口大小
	public static long TimeOut = 7000;	//超时 ms


	//丢包、坏包的参数
	public static int LostRate = 50;		//每11个包丢1个
	public static int ErrorRate =50 ;	//每9个包错1个
}
