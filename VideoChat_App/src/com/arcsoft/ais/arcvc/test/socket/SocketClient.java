package com.arcsoft.ais.arcvc.test.socket;

import java.io.*;
import java.net.*;

import com.arcsoft.ais.arcvc.utils.SdCardUtils;

 public class SocketClient {
    static Socket clientSocket;
	private InputStream inputStream;
    
    public SocketClient(String site, int port){
        try{
            clientSocket = new Socket(site,port);
            System.out.println("Client is created! site:"+site+" port:"+port);
        }catch (UnknownHostException e){
            e.printStackTrace();
        }catch (IOException e){
            e.printStackTrace();
        }
    }
    
    public String sendMsg(String msg){
        try{
        	InputStream is = new FileInputStream(SdCardUtils.getSdCardPath()+"/video/out.h264");
//            BufferedReader in = new BufferedReader(new InputStreamReader(is));
            BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            PrintWriter out = new PrintWriter(clientSocket.getOutputStream());
//            out.println(msg);
            out.flush();
            return in.readLine();
        }catch(IOException e){
            e.printStackTrace();
        }
        return "";
    }

    public void sendStream(String file){
        try{
        	inputStream = new FileInputStream(file);  
            // 获取Socket的OutputStream对象用于发送数据。  
            OutputStream outputStream = clientSocket.getOutputStream();  
            // 创建一个byte类型的buffer字节数组，用于存放读取的本地文件  
            byte buffer[] = new byte[10 * 1024];  
            int temp = 0;  
            // 循环读取文件  
            while ((temp = inputStream.read(buffer)) != -1) {  
                // 把数据写入到OuputStream对象中  
                outputStream.write(buffer, 0, temp);  
            }  
            // 发送读取的数据到服务端  
            outputStream.flush();  
        }catch(IOException e){
            e.printStackTrace();
        }
    }
    
    public void closeSocket(){
        try{
            clientSocket.close();
            inputStream.close();
        }catch(IOException e){
            e.printStackTrace();
        }
    }
    public static void main(String[] args) throws Exception{
        
    }

}