package GBN;


import Frames.*;
import TEST.*;
import java.io.*;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Sender {
	//配置参数
	int DataSize;    //数据帧大小
	int SwSize;       //窗口大小
	long TimeOut;    //超时
	SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss:SSS");

	public Sender() {
		this.DataSize = Config.DataSize;
		this.SwSize = Config.SwSize;
		this.TimeOut = Config.TimeOut;
	}

	public void send(String src, String ip, int port) {
		Thread sender =new Thread(new SendThread(src,ip,port));
		sender.start();
	}

	private class SendThread implements Runnable {
		private Lock lock = new ReentrantLock();
		int nbuffered;        //总数据帧数
		int sum;				//
		int expect;            //期待的帧
		int next;            //待发送的帧
		int next_to_prepare;
		Frame[] frame = null;    //窗口
		long[] time = null;        //访问时间
		private String src;
		private String ip;
		private int port;
		String status = "New"; //当前帧状态
		int statusc = 0;
		FileInputStream fstream;
		ObjectInputStream inStream;
		ObjectOutputStream outStream;
		int ackNo = 0;



		public SendThread(String src, String ip, int port){
			this.src = src;
			this.ip = ip;
			this.port = port;
		}

		@Override
		public void run() {

			try {
				/**
				 * 加载文件流
				 */
				File file = new File(src);
				fstream =  new FileInputStream(src);
				/**
				 * 套接字链接
				 * 网络输入、输出流建立
				 */
				Socket socket = new Socket(ip, port);
				inStream = new ObjectInputStream(socket.getInputStream());
				outStream = new ObjectOutputStream(socket.getOutputStream());
				System.out.println(formatter.format(new Date()) + " Sender: Connect success. Ip:" + socket.getInetAddress() + " port:" + socket.getPort());
				/**
				 * 协议参数初始化
				 */
				nbuffered = (int) Math.ceil((float) file.length() / DataSize) + 1;
				sum = nbuffered;
				next = 0;                //下一个要发送的帧
				next_to_prepare = 0;	//下一个准备的帧
				expect = 0;            //期待的ack序号
				frame = new Frame[SwSize];    //窗口
				time = new long[SwSize];        //时间戳列表
				/**
				 * 准备第一个窗口
				 */
				frame[0] = new Head(nbuffered, file.getName());    //头帧
				for (int i = 1; i < SwSize; i++) {
					frame[i] = new Data(i, fstream.readNBytes(DataSize));
				}
				next_to_prepare = SwSize;
				/**
				 * ACK监听线程
				 */
				Thread acklistener = new Thread(new AckListener());
				acklistener.start();
				/**
				 *	开始数据帧传输
				 */
				TEST.CSV.writeCsvLine("./log/config/config.csv",new Date().getTime()/1000+","+Config.nThreads+","+Config.DataSize
						+","+Config.SwSize+","+Config.TimeOut+","+Config.LostRate+","+Config.ErrorRate);
				String LogName = "./log/send/"+"sendto"+"_"+port+"_"+file.getName()+"_"+new Date().getTime()/1000+".csv";
				TEST.CSV.writeCsvLine(LogName,"time_stamp,pdu_to_send,status,ackedNo");
				while (true) {
					lock.lock();
					if(nbuffered <= 0){
						lock.unlock();
						break;
					};
					//窗口没发满
					if (next < next_to_prepare) {
						if(((int)(Math.random()*Config.LostRate))==0){	//丢包
							System.out.println(formatter.format(new Date()) + " Frame lost.");
							;
						}
						else{
							//错误帧
							if (frame[next%SwSize].type!=TYPE.HEAD && ((int)(Math.random()*Config.ErrorRate))==0) {
								Data d = ((Data)frame[next%SwSize]);
								Data wrong = new Data(d.seq,d.data);
								wrong.crc16 = -1;
								TEST.CSV.writeCsvLine(LogName,new Date().getTime()+","+d.seq+","+status+","+ackNo);
								outStream.writeObject(wrong);
								System.out.println(formatter.format(new Date()) + " Send a wrong Frame.");
							}
							else{	//正常发送包
								outStream.writeObject(frame[next % SwSize]);
								time[next % SwSize] = System.currentTimeMillis();
								statusc -= 1;
								if(statusc > 0){
									status = "TO";
								}else{
									status = "New";
								}
								if (frame[next % SwSize].type == TYPE.DATA) {
									Data d = (Data) frame[next % SwSize];
									System.out.println(formatter.format(new Date()) + " Sender:   send Data:" + d.seq + "  expect:" + expect + "  next:" + next+"  nbuffered:"+nbuffered);
									TEST.CSV.writeCsvLine(LogName,new Date().getTime()+","+d.seq+","+status+","+ackNo);
								} else {
									System.out.println(formatter.format(new Date()) + " Sender:   send Data:0  expect:" + expect + "  next:" + next+"  nbuffered:"+nbuffered);
									TEST.CSV.writeCsvLine(LogName,new Date().getTime()+","+0+","+status+","+ackNo);
								}
							}
						}
						next += 1;    //发了一个，该发下一个
					}
					//检测最早发送的窗口是否超时
					long cur = System.currentTimeMillis();
					if (cur - time[expect % SwSize] > TimeOut) {
						statusc = next - expect;
						next = expect;
						System.out.println("time is out!");
					}
					lock.unlock();
				}
				System.out.println(formatter.format(new Date()) + " Sender:    Finish ! ! !");
				fstream.close();
				socket.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		private class AckListener implements Runnable {
			public static Ack ack;

			@Override
			public void run() {
				try {
					while (true) {
						lock.lock();
						if(nbuffered <= 0){
							lock.unlock();
							break;
						}
						lock.unlock();
						Frame f = (Frame) inStream.readObject();
						lock.lock();
						if (f.type == TYPE.ACK) {
							ack = (Ack) f;
							ackNo = ack.ack;
							System.out.println(formatter.format(new Date()) + " Sender:   get ACK:" + ack.ack + "  expect:" + expect + " next:" + next+"  nbuffered:"+nbuffered);
							//操作
							if (ack.ack == expect) {
								nbuffered -= 1;    //收到一个正确的ack帧，减1
								int t;
								if(	(frame[next%SwSize]).type==TYPE.HEAD ){
									t = 0;
								}
								else{
									t = ((Data)frame[next%SwSize]).seq;
								}
								if(next_to_prepare<sum) {
									frame[next_to_prepare % SwSize] = new Data(next_to_prepare, fstream.readNBytes(DataSize));
									System.out.println("        Update Frame:" + next + " to :" + t + "  p:" + next % SwSize);
									next_to_prepare += 1;
									time[expect % SwSize] = 0;    //清除旧发送时间
								}
								else{
									System.out.println("        Non need to update Frame");
								}
								expect += 1;            //期望的ack
							}
						} else {
							throw new Exception("        Expect: ACK. But get: " + f.type);
						}
						lock.unlock();
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		};
	}
}
