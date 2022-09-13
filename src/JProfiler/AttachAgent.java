import java.io.File;
import com.sun.tools.attach.VirtualMachine;
import java.lang.management.ManagementFactory;
import java.util.Locale;

import java.util.Scanner;  //用于测试

public class AttachAgent{
    long tid = Thread.currentThread().getId();
    
    public static void main(String[] args) throws Exception{
        String pid = args[0];
        //用于测试
        // Scanner input = new Scanner(System.in);
        // String val = null;    
        // do{
        //     System.out.print("请输入：");
        //     val = input.next();       // 等待输入值
        //     System.out.println("您输入的是："+val);
        // }while(!val.equals("#"));   // 如果输入的值不版是#就继续输入
        // System.out.println("你输入了\"#\"，程序已经退出！");
        // input.close(); // 关闭资源
        //测试结束
        loadAgentPath(pid);    
    }

    static void loadAgentPath(String pid)throws Exception{
        VirtualMachine vm = VirtualMachine.attach(pid);
        try{
            final File lib;
            if (System.getProperty("os.name", "").toLowerCase(Locale.US).contains("os x")) {
                lib = new File("lib/libjpa.dylib");
            } else {
                lib = new File("/home/yuzhaoxi/Profiler/build/out/lib/libjpa.so");
            }
            String fullpath = lib.getAbsolutePath();
            if(!lib.exists()){
                System.out.printf("Expected %s at '%s' but it didn't exist.\n", lib.getName(), fullpath);
                System.exit(1);
            }
            else{ 
                vm.loadAgentPath(fullpath, ""); 
            }
        }catch(com.sun.tools.attach.AgentInitializationException e){
            if (!e.getMessage().equals("Agent_OnAttach failed")) throw e;
        }finally{
            vm.detach();
        }
    }
}