public class main {
	public static void main(String[] args) {
		DV dv = new DV();
		String[] s = new String[3];

		if(args.length<2){
			System.out.println("������ ·�������� �˿� �����ļ�·��");
		}
		else
			dv.start(args);
	}
}
