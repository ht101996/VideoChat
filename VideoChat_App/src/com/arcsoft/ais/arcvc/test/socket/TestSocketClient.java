package com.arcsoft.ais.arcvc.test.socket;

import java.io.File;

import android.util.Log;

import com.arcsoft.ais.arcvc.test.socket.udp.UdpClientSocket;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.MD5Util;
import com.arcsoft.ais.arcvc.utils.SdCardUtils;

public class TestSocketClient {
	static SocketClient client = null ;
	public static boolean isSending = true ;
	public static String ip ;
	public static String filePath ;
	
   public static void start(String filePathString,String ipString){
		Log.d(Global.TAG, "TestSocketClient --------------ip=="+ip);
		Log.d(Global.TAG, "TestSocketClient --------------filePath=="+filePath);
	   ip = ipString;
	   filePath = filePathString;
	// 开启一个子线程，进行网络操作，
	    new Thread(networkTask).start();  
	    new Thread(networkTask2).start();  
	    new Thread(md5Task).start();  
   }

   public static void stop(){
		Log.d(Global.TAG, "TestSocketClient --------------stop==");
	   client.closeSocket();
	   client = null ;
	   isSending = false ;
   }
   
   static Runnable networkTask = new Runnable() {  
	    @Override  
	    public void run() {  
			Log.d(Global.TAG, "TestSocketClient --------------ip=="+ip);
		   isSending = true ;
		   if (client == null) {
			   client = new SocketClient(ip,12345);
		   }
		   
		   while (isSending) {
			   Log.d(Global.TAG, "..sendStream.start");
			  // client.sendStream(filePath);
			   UdpClientSocket.start(filePath);
			   Log.d(Global.TAG, "..sendStream ..end");
		   }
	    }  
	};  

	   static Runnable networkTask2 = new Runnable() {  
		    @Override  
		    public void run() {  
				Log.d(Global.TAG, "TestSocketClient --------------ip=="+ip);
			   isSending = true ;
			   if (client == null) {
				   client = new SocketClient(ip,12345);
			   }
			   
			   while (isSending) {
				   Log.d(Global.TAG, "..sendStream.start");
//				   client.sendStream(filePath+".back2");
				   UdpClientSocket.start(filePath+".back2");
				   Log.d(Global.TAG, "..sendStream ..end");
			   }
		    }  
		};  
	   static Runnable md5Task = new Runnable() {  
		    @Override  
		    public void run() {  
			   while (isSending) {
				   Log.d(Global.TAG, "MD5Util getFileMD5String...");
				   MD5Util.getMD5String("aaaaaaaaaaaaaaaaaaaaa" +
				   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
				   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
				   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
				   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
				   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
				   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
				   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
				   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
				   Log.d(Global.TAG, "MD5Util getFileMD5String...end");
//			       try {
//					Thread.sleep(10);
//				} catch (InterruptedException e) {
//					e.printStackTrace();
//				}
			   }
		    }  
		};  

		   static Runnable md5Task2 = new Runnable() {  
			    @Override  
			    public void run() {  
				   while (isSending) {
					   Log.d(Global.TAG, "md5Task2...");
//					   MD5Util.getFileMD5String(new File(filePath+".back2"));
					   MD5Util.getMD5String("aaaaaaaaaaaaaaaaaaaaa" +
						   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
						   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
						   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
						   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
						   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
						   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
						   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" +
						   		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
					   Log.d(Global.TAG, "md5Task2...end");
//				       try {
//						Thread.sleep(10);
//					} catch (InterruptedException e) {
//						e.printStackTrace();
//					}
				   }
			    }  
			};  
			
}