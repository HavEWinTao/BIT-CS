import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.lang.reflect.Array;
import java.net.*;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class DV {
	//路径
	public static String init_Path_Neighbor;
	public static String init_Path_Sys;
	public static String log_Path;
	//本机参数
	public static String id;
	public static int udpPort;
	public static int Frequency;
	public static double Unreachable;
	public static int MaxValidTime;
	public static int NodeNumber;
	//列表
	public static Map<String, Neighbor> list_neighbor;
	public static Map<String, Routing> list_routing;
	//接收套接字
	private static DatagramSocket socket = null;
	//锁
	private static Lock lock = new ReentrantLock();

	public void start(String[] args) {
		// 0.相关参量初始化
		id = args[0];
		udpPort = Integer.parseInt(args[1]);
		init_Path_Neighbor = args[2];
		init_Path_Sys = "system_configuration_file.txt";
		log_Path = id + ".log.txt";

		list_neighbor = new TreeMap<String, Neighbor>();
		list_routing = new TreeMap<String, Routing>();

		//清空文件内容
		try {
			PrintWriter out = new PrintWriter(log_Path);
			out.print("");
			out.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		// 1.读取邻居节点配置文件
		try {
			File file = new File(init_Path_Neighbor);
			InputStreamReader instream = new InputStreamReader(new FileInputStream(file));
			BufferedReader in = new BufferedReader(instream);
			String str;
			String[] a;
			Neighbor item;
			while ((str = in.readLine()) != null) {
				a = str.split(" ");

				item = new Neighbor();
				item.id = a[0];
				item.cost = Double.parseDouble(a[1]);
				item.portNum = Integer.parseInt(a[2]);
				item.alive = false;
				if (a.length == 4) {
					item.address = a[3];
				} else {
					item.address = "localhost";
				}

				list_neighbor.put(a[0], item);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("1.Read neighbor file, Finish.");

		// 2.读取系统配置文件
		try {
			File file = new File(init_Path_Sys);
			InputStreamReader instream = new InputStreamReader(new FileInputStream(file));
			BufferedReader in = new BufferedReader(instream);
			String str;
			String[] a;
			while ((str = in.readLine()) != null) {
				a = str.split(":");
				if (a[0].equals("Frequency")) {
					Frequency = Integer.parseInt(a[1]);
				} else if (a[0].equals("Unreachable")) {
					Unreachable = Double.parseDouble(a[1]);
				} else if (a[0].equals("MaxValidTime")) {
					MaxValidTime = Integer.parseInt(a[1]);
				} else if (a[0].equals("Node_number")) {
					NodeNumber = Integer.parseInt(a[1]);
				} else {
					System.out.println("[ERROR]: \"" + a[0] + "\" is can not be decode.");
					throw new Exception();
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("2.Read sys Config file, Finish.");

		// 3.初始化本结点路由表
		Routing r;
		for (Map.Entry<String, Neighbor> n : list_neighbor.entrySet()) {
			r = new Routing();
			r.destNode = n.getValue().id;
			r.distance = n.getValue().cost;
			r.neighbor = n.getValue().id;
			list_routing.put(r.destNode, r);
		}
		System.out.println("3.Init local Routing table, Finish.");

		// 4.定时向相邻节点发送本结点距离向量的定时器
		Timer timer = new Timer("timer");
		timer.schedule(new Sender(), 0, Frequency);

		// 5.创建接收邻居节点发送信息的线程
		Thread receive = new Thread(new Receive(), "receive");
		receive.start();

		// 6.创建相邻节点监听线程
		/** *
		 * 这里没有按python版本的程序，创建多个监听线程，处理每个线程的超时。
		 * 若每T秒检测一次是否超时，则若一个数据包在 t+dt时刻 到达，然后该
		 * 路由器k掉线，则在 t+T 时，本节点认为路由器k还未超时掉线。在t+2T时
		 * 因此，每MaxValidTime/2事件检测一次是否超时
		 */
		Timer killer = new Timer("killer");
		killer.schedule(new Killer(), MaxValidTime / 2, MaxValidTime / 2);

		// 7.主线程，处理命令行参数
		try {
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			String cmd;
			while (true) {
				cmd = br.readLine();
				if (cmd.equals("K")) {
					System.out.println("节点终止运行！");
					timer.cancel();
					socket.close();
					receive.interrupt();
					killer.cancel();
					break;
				} else if (cmd.equals("P")) {
					System.out.println("节点暂停运行");
					timer.cancel();
					socket.close();
					receive.interrupt();
					killer.cancel();
				} else if (cmd.equals("S")) {
					System.out.println("节点恢复运行！");
					timer = new Timer("timer");
					timer.schedule(new Sender(), 0, Frequency);
					receive = new Thread(new Receive(), "receive");
					killer = new Timer("killer");
					killer.schedule(new Killer(), MaxValidTime / 2, MaxValidTime / 2);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	class Receive implements Runnable {
		@Override
		public void run() {
			String neighbor_id;
			Map<String, Routing> rout;
			byte[] data1 = new byte[1024];
			byte[] data2 = new byte[1024];

			try {
				socket = new DatagramSocket(udpPort);
			} catch (SocketException e) {
				e.printStackTrace();
			}
			DatagramPacket packet_id = new DatagramPacket(data1, data1.length);
			DatagramPacket packet_list = new DatagramPacket(data2, data2.length);
			//去除字符串末尾 \0
			byte[] zero = new byte[1];
			zero[0] = 0;
			String szero = new String(zero);
			while (true) {
				try {
					//线程终止
					if (Thread.currentThread().isInterrupted()) {
						break;
					}
					//收一个列表
					socket.receive(packet_id);
					neighbor_id = new String(data1).replace(szero, "");        //去除末尾 \0
					socket.receive(packet_list);
					ByteArrayInputStream inByte = new ByteArrayInputStream(data2);
					ObjectInputStream inObj = new ObjectInputStream(inByte);
					rout = (Map<String, Routing>) inObj.readObject();

					lock.lock();

					//打时间戳
					list_neighbor.get(neighbor_id).stamp = System.currentTimeMillis();
					list_neighbor.get(neighbor_id).alive = true;
					//找到到邻居的cost
					double neighbor_cost = list_neighbor.get(neighbor_id).cost;

					//根据接受的信息更新当前结点信息
					boolean change = false;
					for (Map.Entry<String, Routing> n : rout.entrySet()) {
						String dest = n.getKey();    //这个目的地
						Routing line = list_routing.get(dest);    //当前结点到这个目的地的信息
						//当前节点到目的地的 cost 大于 通过这个节点到目的地的距离+延时
						if (line != null) {
							if (line.distance > n.getValue().distance + neighbor_cost) {
								line.neighbor = neighbor_id;
								line.distance = n.getValue().distance + neighbor_cost;
								change = true;
							}
						}
					}

					lock.unlock();

					//打印日志
					print_save(neighbor_id, rout);

					//路由表有更新，发送给所有相邻路由器
					if (change) {
						System.out.println("Routing table change!!!!");
						send_table();
					}
				} catch (InterruptedIOException e) {
					System.out.println("[Thread]:Receive is stopped.");
				} catch (IOException e) {
					e.printStackTrace();
				} catch (ClassNotFoundException e) {
					e.printStackTrace();
				}


			}
		}
	}

	class Sender extends TimerTask {
		@Override
		public void run() {
			send_table();
		}
	}

	class Killer extends TimerTask {
		@Override
		public void run() {
			lock.lock();
			for (Map.Entry<String, Neighbor> n : list_neighbor.entrySet()) {
				if (!n.getKey().equals(id)) {    //不用监听自己
					if (n.getValue().alive) {	//不用监听脱机的节点
						long cur = System.currentTimeMillis();
						if (n.getValue().stamp + MaxValidTime < cur) {    //超时了
							n.getValue().alive = false;    //设定无限长时间，掉线
						}
					}
				}
			}
			lock.unlock();
		}
	}

	//发送路由表的函数
	private static void send_table() {
		try {
			//创建用于发送的UDP套接字
			DatagramSocket socket = new DatagramSocket();

			lock.lock();
			for (Map.Entry<String, Neighbor> n : list_neighbor.entrySet()) {
				if (!n.getKey().equals(id)) {    //不用发送给自己
					//定义接收者的 IP、port
					InetAddress address = InetAddress.getByName(n.getValue().address);
					int port = n.getValue().portNum;    //接收者的端口
					//准备数据包
					byte[] data1 = id.getBytes();
					DatagramPacket packet_id = new DatagramPacket(data1, data1.length, address, port);
					ByteArrayOutputStream outByte = new ByteArrayOutputStream();
					ObjectOutputStream outObj = new ObjectOutputStream(outByte);
					outObj.writeObject(list_routing);
					byte[] data2 = outByte.toByteArray();
					DatagramPacket packet_list = new DatagramPacket(data2, data2.length, address, port);
					//发送数据包
					socket.send(packet_id);
					socket.send(packet_list);


				}
			}
			//输出日志
			print_save();
			lock.unlock();

		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private static int counter = 0;
	private static SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss:SSS");

	//打印日志 发送数据包 当前节点的list
	private static void print_save() {
		//构造输出文件格式
		String s = new String();
		s += formatter.format(new Date());
		s += "  ## Sent.Source Node= " + id + "; Sequence Number = " + counter + "\n";
		counter+=1;
		lock.lock();
		for (Map.Entry<String, Routing> n : list_routing.entrySet()) {
			s += "    destNode = " + n.getValue().destNode;
			s += "; distance = " + n.getValue().distance;
			s += "; neighbor = " + n.getValue().neighbor;
			s += "\n";
		}
		lock.unlock();

		s+="\n";

		//输出
		System.out.print(s);
		try {
			FileWriter out = new FileWriter(new File(log_Path), true);
			out.append(s);
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	//打印日志 接收数据包 接收到的list
	private static void print_save(String received_id, Map<String, Routing> l) {
		//构造输出文件格式
		String s = new String();
		s += formatter.format(new Date());
		s += "  ## Received.Source Node= " + received_id + "; Sequence Number = " + counter + "\n";
		counter+=1;
		for (Map.Entry<String, Routing> n : l.entrySet()) {
			s += "    destNode = " + n.getValue().destNode;
			s += "; distance = " + n.getValue().distance;
			s += "; neighbor = " + n.getValue().neighbor;
			s += "\n";
		}
		s += "\n";
		//输出
		System.out.print(s);
		try {
			FileWriter out = new FileWriter(new File(log_Path), true);
			out.append(s);
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}


	private static class Neighbor implements Serializable {
		String id;        //邻居节点ID
		double cost;    //到邻居的代价

		int portNum;    //端口号
		String address;	//IP地址
		long stamp;     //时间戳
		boolean alive;	//是否在线
	}

	private static class Routing implements Serializable {
		String destNode;    //目标节点id
		double distance;    //距离
		String neighbor;    //邻居节点id
	}
}

