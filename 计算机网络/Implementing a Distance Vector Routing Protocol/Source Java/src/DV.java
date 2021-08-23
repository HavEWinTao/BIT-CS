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
	//·��
	public static String init_Path_Neighbor;
	public static String init_Path_Sys;
	public static String log_Path;
	//��������
	public static String id;
	public static int udpPort;
	public static int Frequency;
	public static double Unreachable;
	public static int MaxValidTime;
	public static int NodeNumber;
	//�б�
	public static Map<String, Neighbor> list_neighbor;
	public static Map<String, Routing> list_routing;
	//�����׽���
	private static DatagramSocket socket = null;
	//��
	private static Lock lock = new ReentrantLock();

	public void start(String[] args) {
		// 0.��ز�����ʼ��
		id = args[0];
		udpPort = Integer.parseInt(args[1]);
		init_Path_Neighbor = args[2];
		init_Path_Sys = "system_configuration_file.txt";
		log_Path = id + ".log.txt";

		list_neighbor = new TreeMap<String, Neighbor>();
		list_routing = new TreeMap<String, Routing>();

		//����ļ�����
		try {
			PrintWriter out = new PrintWriter(log_Path);
			out.print("");
			out.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		// 1.��ȡ�ھӽڵ������ļ�
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

		// 2.��ȡϵͳ�����ļ�
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

		// 3.��ʼ�������·�ɱ�
		Routing r;
		for (Map.Entry<String, Neighbor> n : list_neighbor.entrySet()) {
			r = new Routing();
			r.destNode = n.getValue().id;
			r.distance = n.getValue().cost;
			r.neighbor = n.getValue().id;
			list_routing.put(r.destNode, r);
		}
		System.out.println("3.Init local Routing table, Finish.");

		// 4.��ʱ�����ڽڵ㷢�ͱ������������Ķ�ʱ��
		Timer timer = new Timer("timer");
		timer.schedule(new Sender(), 0, Frequency);

		// 5.���������ھӽڵ㷢����Ϣ���߳�
		Thread receive = new Thread(new Receive(), "receive");
		receive.start();

		// 6.�������ڽڵ�����߳�
		/** *
		 * ����û�а�python�汾�ĳ��򣬴�����������̣߳�����ÿ���̵߳ĳ�ʱ��
		 * ��ÿT����һ���Ƿ�ʱ������һ�����ݰ��� t+dtʱ�� ���Ȼ���
		 * ·����k���ߣ����� t+T ʱ�����ڵ���Ϊ·����k��δ��ʱ���ߡ���t+2Tʱ
		 * ��ˣ�ÿMaxValidTime/2�¼����һ���Ƿ�ʱ
		 */
		Timer killer = new Timer("killer");
		killer.schedule(new Killer(), MaxValidTime / 2, MaxValidTime / 2);

		// 7.���̣߳����������в���
		try {
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			String cmd;
			while (true) {
				cmd = br.readLine();
				if (cmd.equals("K")) {
					System.out.println("�ڵ���ֹ���У�");
					timer.cancel();
					socket.close();
					receive.interrupt();
					killer.cancel();
					break;
				} else if (cmd.equals("P")) {
					System.out.println("�ڵ���ͣ����");
					timer.cancel();
					socket.close();
					receive.interrupt();
					killer.cancel();
				} else if (cmd.equals("S")) {
					System.out.println("�ڵ�ָ����У�");
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
			//ȥ���ַ���ĩβ \0
			byte[] zero = new byte[1];
			zero[0] = 0;
			String szero = new String(zero);
			while (true) {
				try {
					//�߳���ֹ
					if (Thread.currentThread().isInterrupted()) {
						break;
					}
					//��һ���б�
					socket.receive(packet_id);
					neighbor_id = new String(data1).replace(szero, "");        //ȥ��ĩβ \0
					socket.receive(packet_list);
					ByteArrayInputStream inByte = new ByteArrayInputStream(data2);
					ObjectInputStream inObj = new ObjectInputStream(inByte);
					rout = (Map<String, Routing>) inObj.readObject();

					lock.lock();

					//��ʱ���
					list_neighbor.get(neighbor_id).stamp = System.currentTimeMillis();
					list_neighbor.get(neighbor_id).alive = true;
					//�ҵ����ھӵ�cost
					double neighbor_cost = list_neighbor.get(neighbor_id).cost;

					//���ݽ��ܵ���Ϣ���µ�ǰ�����Ϣ
					boolean change = false;
					for (Map.Entry<String, Routing> n : rout.entrySet()) {
						String dest = n.getKey();    //���Ŀ�ĵ�
						Routing line = list_routing.get(dest);    //��ǰ��㵽���Ŀ�ĵص���Ϣ
						//��ǰ�ڵ㵽Ŀ�ĵص� cost ���� ͨ������ڵ㵽Ŀ�ĵصľ���+��ʱ
						if (line != null) {
							if (line.distance > n.getValue().distance + neighbor_cost) {
								line.neighbor = neighbor_id;
								line.distance = n.getValue().distance + neighbor_cost;
								change = true;
							}
						}
					}

					lock.unlock();

					//��ӡ��־
					print_save(neighbor_id, rout);

					//·�ɱ��и��£����͸���������·����
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
				if (!n.getKey().equals(id)) {    //���ü����Լ�
					if (n.getValue().alive) {	//���ü����ѻ��Ľڵ�
						long cur = System.currentTimeMillis();
						if (n.getValue().stamp + MaxValidTime < cur) {    //��ʱ��
							n.getValue().alive = false;    //�趨���޳�ʱ�䣬����
						}
					}
				}
			}
			lock.unlock();
		}
	}

	//����·�ɱ�ĺ���
	private static void send_table() {
		try {
			//�������ڷ��͵�UDP�׽���
			DatagramSocket socket = new DatagramSocket();

			lock.lock();
			for (Map.Entry<String, Neighbor> n : list_neighbor.entrySet()) {
				if (!n.getKey().equals(id)) {    //���÷��͸��Լ�
					//��������ߵ� IP��port
					InetAddress address = InetAddress.getByName(n.getValue().address);
					int port = n.getValue().portNum;    //�����ߵĶ˿�
					//׼�����ݰ�
					byte[] data1 = id.getBytes();
					DatagramPacket packet_id = new DatagramPacket(data1, data1.length, address, port);
					ByteArrayOutputStream outByte = new ByteArrayOutputStream();
					ObjectOutputStream outObj = new ObjectOutputStream(outByte);
					outObj.writeObject(list_routing);
					byte[] data2 = outByte.toByteArray();
					DatagramPacket packet_list = new DatagramPacket(data2, data2.length, address, port);
					//�������ݰ�
					socket.send(packet_id);
					socket.send(packet_list);


				}
			}
			//�����־
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

	//��ӡ��־ �������ݰ� ��ǰ�ڵ��list
	private static void print_save() {
		//��������ļ���ʽ
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

		//���
		System.out.print(s);
		try {
			FileWriter out = new FileWriter(new File(log_Path), true);
			out.append(s);
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	//��ӡ��־ �������ݰ� ���յ���list
	private static void print_save(String received_id, Map<String, Routing> l) {
		//��������ļ���ʽ
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
		//���
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
		String id;        //�ھӽڵ�ID
		double cost;    //���ھӵĴ���

		int portNum;    //�˿ں�
		String address;	//IP��ַ
		long stamp;     //ʱ���
		boolean alive;	//�Ƿ�����
	}

	private static class Routing implements Serializable {
		String destNode;    //Ŀ��ڵ�id
		double distance;    //����
		String neighbor;    //�ھӽڵ�id
	}
}

