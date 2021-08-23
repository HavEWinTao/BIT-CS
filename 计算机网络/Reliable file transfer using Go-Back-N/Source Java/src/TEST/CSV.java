package TEST;

import java.io.*;
import java.util.ArrayList;
import java.util.List;


public class CSV {
    /**追加写入单行
     *
     * @param filename
     * @param str
     */
    public static void writeCsvLine(String filename, String str) {
        File writeFile = new File(filename);
        try {
            BufferedWriter writeText = new BufferedWriter(new FileWriter(writeFile,true));
            writeText.append(str).append("\r");
            writeText.flush();
            writeText.close();
        } catch (FileNotFoundException e) {
            System.out.println("没有找到指定文件");
        } catch (IOException e) {
            System.out.println("文件读写出错");
        }
    }

    /**非追加 写入整个datalist
     *
     * @param filename
     * @param dataList
     */
    public static void writeCsv(String filename, List<String> dataList){
        File writeFile = new File(filename);
        try {
            BufferedWriter writeText = new BufferedWriter(new FileWriter(writeFile));
            if (dataList != null && !dataList.isEmpty()) {
                for (String data : dataList) {
                    writeText.append(data).append("\r");
                }
            }
            writeText.flush();
            writeText.close();
        } catch (FileNotFoundException e) {
            System.out.println("没有找到指定文件");
        } catch (IOException e) {
            System.out.println("文件读写出错");
        }
    }

    /**将csv读入stringlist
     *
      * @param filename
     * @return
     */
    public static List<String> readCsv(String filename) {
        File file = new File(filename);
        List<String> dataList = new ArrayList<String>();
        BufferedReader br = null;
        try {
            br = new BufferedReader(new FileReader(file));
            String line = "";
            while ((line = br.readLine()) != null) {
                dataList.add(line);
            }
        } catch (FileNotFoundException e) {
            System.out.println("没有找到指定文件");
        } catch (IOException e) {
            System.out.println("文件读写出错");
        }
        return dataList;
    }

    /**测试函数
     *
     */
    public static void test(){
        List<String> dataList=new ArrayList<String>();
        dataList.add("number,name,sex");
        dataList.add("1,q,1");
        dataList.add("2,1,2");
        dataList.add("3,3,4");
        CSV.writeCsv("./user1/log3.csv", dataList);
        CSV.writeCsvLine("./log/log3.csv", "1,2,3");
        List<String> data = CSV.readCsv("./log/log3.csv");
        if(data!=null && !data.isEmpty()){
            for(int i=0; i<data.size();i++ ){
                String s=data.get(i);
                System.out.println(s);
                String[] as = s.split(",");
            }
        }
    }

    public static void main(String[] args) throws Exception {
        CSV.test();
    }
}
