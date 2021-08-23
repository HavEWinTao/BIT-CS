package GBN;

import Frames.*;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Receiver {
	private SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss:SSS");//格式化时间
	private String dest;
	private int port;
	private ExecutorService tpool;
	private Thread listener;
	public Receiver(String dest,int port){
		this.dest = dest;
		this.port = port;
		this.tpool = Executors.newFixedThreadPool(Config.nThreads);
	}

	public void start(){
		this.listener = new Thread(new Listener());
		this.listener.start();
		System.out.println(formatter.format(new Date()) + " Receiver start ! ! !");
	}

	public void stop(){
		try{
			this.listener.interrupt();
		}catch(Exception e){
			e.printStackTrace();
		}
	}

	private class Listener implements Runnable {
		@Override
		public void run() {
			try {
				ServerSocket server = new ServerSocket(port);
				while(true){
					Thread new_receiver = new Thread(new _Receiver(server.accept()));
					tpool.execute(new_receiver);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		private class _Receiver implements Runnable {
			Socket socket;
			public _Receiver(Socket socket) {
				this.socket = socket;
			}
			@Override
			public void run() {
				try {
					ObjectOutputStream outStream = new ObjectOutputStream(socket.getOutputStream());
					ObjectInputStream inStream = new ObjectInputStream(socket.getInputStream());
					System.out.println(formatter.format(new Date()) + " Receiver: Get a connect. Ip:" + socket.getInetAddress() + " port:" + socket.getPort());
					/**
					 * 变量声明
					 */
					String filename;                //文件名
					FileOutputStream fstream = null;//文件流
					Head h;
					Data d;
					Ack ack;
					Frame f;
					/**
					 * 开始数据帧传输
					 */
					String LogName = "./log/receive/"+"receivefrom"+"_"+socket.getPort()+"_"+new Date().getTime()/1000+".csv";;
					int nbuffered = 1;        //数据帧数
					int frame_expcted = 0;    //期望的数据包
					while (nbuffered > 0) {
						f = (Frame) inStream.readObject();
						switch (f.type) {
							case DATA:
								d = (Data) f;
								System.out.println(formatter.format(new Date()) + " Receiver:   Get Data. get:" + d.seq + "  expect:" + frame_expcted + " left:" + nbuffered);
								if (d.seq == frame_expcted) {    //期望帧
									if (d.crc16_Check()) {    //CRC16校验通过
										TEST.CSV.writeCsvLine(LogName,new Date().getTime()+","+frame_expcted+","+d.seq+","+"OK");
										ack = new Ack(frame_expcted);
										outStream.writeObject(ack);
										nbuffered -= 1;
										frame_expcted += 1;
										System.out.println(formatter.format(new Date()) + " Receiver:   Send ACK. ack:" + ack.ack);
										//写文件流
										fstream.write(d.data);
									} else {    //CRC16校验失败
										TEST.CSV.writeCsvLine(LogName,new Date().getTime()+","+d.seq+","+frame_expcted+","+"DataErr");
										System.out.println("            CRC16 check wrong");
										//丢弃 坏帧
									}
								} else {    //错误帧
									TEST.CSV.writeCsvLine(LogName,new Date().getTime()+","+d.seq+","+frame_expcted+","+"NoErr");
									//丢弃 坏帧
								}
								break;
							case HEAD:
								h = (Head) f;
								LogName = "./log/receive/"+"receivefrom"+"_"+socket.getPort()+"_"+h.name+"_"+new Date().getTime()/1000+".csv";
								TEST.CSV.writeCsvLine(LogName,"time_stamp,pdu_exp,pdu_recv,status");
								System.out.println(formatter.format(new Date()) + " Receiver:   Get Head. Fname:" + h.name + "  nbuffered:" + h.nbuffered);
								if (h.seq == frame_expcted) {    //期望第1帧
									TEST.CSV.writeCsvLine(LogName,new Date().getTime()+","+frame_expcted+","+0+","+"OK");
									ack = new Ack(frame_expcted);
									outStream.writeObject(ack);
									nbuffered = h.nbuffered - 1;
									frame_expcted += 1;
									System.out.println(formatter.format(new Date()) + " Receiver:   Send ACK. ack:" + ack.ack);
									//文件流初始化
									filename = h.name;
									fstream = new FileOutputStream(dest + filename);
								} else {    //错误帧
									TEST.CSV.writeCsvLine(LogName,new Date().getTime()+","+h.seq+","+frame_expcted+","+"NoErr");
									//丢弃 错误帧
								}
								break;

							default:
								fstream.close();
						}

					}
					System.out.println(formatter.format(new Date()) + " Receiver:   Finish ! ! !");
					fstream.close();
					socket.close();
				} catch (Exception e) {
					e.printStackTrace();

				}
			}
		}
	}
}
