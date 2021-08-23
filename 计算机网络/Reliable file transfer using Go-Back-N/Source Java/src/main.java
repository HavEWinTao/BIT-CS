import java.util.LinkedList;
import java.util.Queue;
import java.util.Scanner;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class main {
	public static void main(String[] args){
		Event event = new Event();
		while(true){
			//String str = event.get_event();
		}
	}

	static class Event{
		private final Lock lock = new ReentrantLock();
		private final Condition mutex = lock.newCondition();
		final int[] que = {0};

		public Event(){
			readint.start();
			time.start();
		}
		Thread readint= new Thread(){
			Scanner scan = new Scanner(System.in);
			@Override
			public void run() {
				while(true) {
					try {
						this.sleep(50);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					lock.lock();
					que[0]+=1;
					System.out.println("S1 add    size:" + que[0]);

					lock.unlock();
				}
			}
		};

		Thread time= new Thread(){
			@Override
			public void run(){
				while(true) {
					try {
						this.sleep(50);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					lock.lock();
					que[0]+=1;
					System.out.println("S2 add    size:" + que[0]);
					lock.unlock();

				}

			}
		};

//		public String get_event() {
//			String res;
//			lock.lock();
//			while (que.isEmpty()) {
////				try {
////					//System.out.println("Que is empty");
////					mutex.await();
////				} catch (InterruptedException e) {
////					e.printStackTrace();
////				}
//			}
//			System.out.println("Pop    size:" + que.size());
//			res = que.poll();
//			lock.unlock();
//
//			return res;
//		}
	}
}
