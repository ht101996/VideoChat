package com.arcsoft.ais.arcvc.utils.audio.sender;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import com.arcsoft.ais.arcvc.jni.AACNal;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.P2PClientManager;
import com.arcsoft.ais.arcvc.utils.SocketUtils;
import com.arcsoft.ais.arcvc.utils.audio.NetConfig;
import com.arcsoft.ais.arcvc.utils.audio.data.AudioData;

import android.util.Log;

public class AudioSender implements Runnable {
	String LOG = "AudioSender ";

	private boolean isSendering = false;
	private List<AudioData> dataList;

	DatagramSocket socket;
	DatagramPacket dataPacket;
	private InetAddress ip;
	private int port;
	AACNal nalu = new AACNal();

	public AudioSender() {
		dataList = Collections.synchronizedList(new LinkedList<AudioData>());
		try {
			try {
				ip = InetAddress.getByName(NetConfig.SERVER_HOST);
				Log.e(LOG, "服务端地址是 " + ip.toString());
				port = NetConfig.SERVER_PORT;
				socket = new DatagramSocket();
			} catch (UnknownHostException e) {
				e.printStackTrace();
			}
		} catch (SocketException e) {
			e.printStackTrace();
		}
	}

	public void addData(byte[] data, int size) {
		AudioData encodedData = new AudioData();
		encodedData.setSize(size);
		byte[] tempData = new byte[size];
		System.arraycopy(data, 0, tempData, 0, size);
		encodedData.setRealData(tempData);
		dataList.add(encodedData);
	}

	/*
	 * send data to server
	 */
	private void sendData0(byte[] data, int size) {
		try {
			dataPacket = new DatagramPacket(data, size, ip, port);
			dataPacket.setData(data);
			Log.e(LOG, "发送一段数据 " + data.length);
			socket.send(dataPacket);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/*
	 * send data to server
	 */
	private void sendData(byte[] data, int size) {
		Log.i(LOG , "sendData ....");
		try {

			String dataString = "" ;
			for (byte i : data) {
				dataString += " "+i;
			}
			Log.i(Global.TAG, "onAudioPacketReceive valueString=" + dataString);

			AACNal nalu = new AACNal();
			nalu.setPayload(data);
			nalu.setPayloadLength(size);
			P2PClientManager.getP2PClientInstance().sendAACPacket("", nalu);
			Log.d(Global.TAG, "sendData, naluData.length==" + nalu.getPayloadLength());
			Log.e(Global.TAG, "sendData,data.length==" + data.length);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	/*
	 * start sending data
	 */
	public void startSending() {
		Log.i(LOG , "startSending ....");
		new Thread(this).start();
	}

	/*
	 * stop sending data
	 */
	public void stopSending() {
		this.isSendering = false;
	}

	// run
	public void run() {
		this.isSendering = true;
		Log.i(LOG , "start....");
		while (isSendering) {
			if (dataList.size() > 0) {
				AudioData encodedData = dataList.remove(0);
				//sendData(encodedData.getRealData(), encodedData.getSize());
				sendData(encodedData.getRealData(), encodedData.getSize());
			}
		}
		Log.i(LOG , "stop!!!!");
	}
}