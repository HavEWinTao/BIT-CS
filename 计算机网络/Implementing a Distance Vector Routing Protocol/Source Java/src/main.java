public class main {
	public static void main(String[] args) {
		DV dv = new DV();
		String[] s = new String[3];

		if(args.length<2){
			System.out.println("请输入 路由器名称 端口 配置文件路径");
		}
		else
			dv.start(args);
	}
}
