import GBN.Config;
import GBN.Receiver;
import GBN.Sender;

import java.io.IOException;

public class user1 {

	public static void main(String[]
									args) {
		String dest = "./user1/";
		String file1 = "./sendfile/1.xlsx";
		String file2 = "./sendfile/2.pdf";
		String file3 = "./sendfile/3.pptx";

		String filet = "./sendfile/test.pdf";

		Sender sender = new Sender();
		Receiver receiver = new Receiver(dest,Config.UdpPort);
		receiver.start();

		try {
			System.in.read();
		} catch (IOException e) {
			e.printStackTrace();
		}

//		sender.send(filet, "localhost", Config.UdpPort);
		sender.send(file1, "localhost", Config.UdpPort+1);
//		sender.send(file2, "localhost", Config.UdpPort+1);
//		sender.send(file3, "localhost", Config.UdpPort+1);
	}
}
