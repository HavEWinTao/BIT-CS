public class a {
	public static void main(String[] args) {
		DV dv = new DV();
		String[] s = new String[3];

		if(args.length<2){
			s[0] = "x";
			s[1] = "52004";
			s[2] = "x.txt";
			dv.start(s);
		}
		else
			dv.start(args);
	}
}
