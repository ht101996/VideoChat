package com.arcsoft.ais.arcvc.utils;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.Properties;
import java.util.UUID;


import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

public class NetworkUtils {

    private static Properties properties = new Properties();

	static {
	}

	//根据IP获取本地Mac
    public static String getLocalMacAddressFromIp(Context context) {
        String mac_s= "";
       try {
            byte[] mac;
            NetworkInterface ne=NetworkInterface.getByInetAddress(InetAddress.getByName(getLocalIpAddress()));
            mac = ne.getHardwareAddress();
            mac_s = byte2hex(mac);
       } catch (Exception e) {
           e.printStackTrace();
       }

		Log.i(Global.TAG, "getLocalMacAddressFromIp===mac_s=" + mac_s);
       if (mac_s.equals("525400123456")) {//
    	   mac_s = UUID.randomUUID().toString().replace("-", "").substring(0, 12);
       }
		Log.i(Global.TAG, "getLocalMacAddressFromIp===mac_s=" + mac_s);
        return mac_s;
    }
    
    public static String getLocalIpAddress2(Context context) {       
    	 //获取wifi服务  
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);  
        //判断wifi是否开启  
        if (!wifiManager.isWifiEnabled()) {  
        wifiManager.setWifiEnabled(true);    
        }  
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();       
        int ipAddress = wifiInfo.getIpAddress();   
        
        return (ipAddress & 0xFF ) + "." +       
      ((ipAddress >> 8 ) & 0xFF) + "." +       
      ((ipAddress >> 16 ) & 0xFF) + "." +       
      ( ipAddress >> 24 & 0xFF) ;  
   }   
    
  //获取本地IP
    public static String getLocalIpAddress() {  
           try {  
               for (Enumeration<NetworkInterface> en = NetworkInterface  
                               .getNetworkInterfaces(); en.hasMoreElements();) {  
                           NetworkInterface intf = en.nextElement();  
                          for (Enumeration<InetAddress> enumIpAddr = intf  
                                   .getInetAddresses(); enumIpAddr.hasMoreElements();) {  
                               InetAddress inetAddress = enumIpAddr.nextElement();  
                               if (!inetAddress.isLoopbackAddress()) {  
                               return inetAddress.getHostAddress().toString();  
                               }  
                          }  
                       }  
                   } catch (SocketException ex) {  
                       Log.e("WifiPreference IpAddress", ex.toString());  
                   }  
           
                return null;  
   }  

    public static  String byte2hex(byte[] b) {
         StringBuffer hs = new StringBuffer(b.length);
         String stmp = "";
         int len = b.length;
         for (int n = 0; n < len; n++) {
          stmp = Integer.toHexString(b[n] & 0xFF);
          if (stmp.length() == 1)
           hs = hs.append("0").append(stmp);
          else {
           hs = hs.append(stmp);
          }
         }
         return String.valueOf(hs);
    }
}
