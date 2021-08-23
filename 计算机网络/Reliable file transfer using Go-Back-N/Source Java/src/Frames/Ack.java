package Frames;

public class Ack extends Frame {
	public int ack;

	public Ack(int ack){
		this.type = TYPE.ACK;
		this.ack = ack;
	}
}
