public class b {
	public static void main(String[] args) {
		DV dv = new DV();
		String[] s = new String[3];

		if(args.length<2){
			s[0] = "b";
			s[1] = "52002";
			s[2] = "b.txt";
			dv.start(s);
		}
		else
			dv.start(args);
	}
}
