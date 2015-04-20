package com.arcsoft.ais.arcvc.test;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import android.util.Log;

import com.arcsoft.ais.arcvc.jni.H264Nal;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.SocketUtils;

public class VideoUtil {
	
	private static boolean usingCam = false;
	
	private static Map<String, byte[]> getSPSAndPPS(String fileName) throws IOException {
		Map<String, byte[]> map = new HashMap<String, byte[]>();
		File file = new File(fileName);
		FileInputStream fis = new FileInputStream(file);

		int fileLength = (int) file.length();
		byte[] fileData = new byte[fileLength];
		fis.read(fileData);
		
		fis.close();

		// 'a'=0x61, 'v'=0x76, 'c'=0x63, 'C'=0x43
		byte[] avcC = new byte[] { 0x61, 0x76, 0x63, 0x43 };

		// avcC的起始位置
		int avcRecord = 0;
		for (int ix = 0; ix < fileLength; ++ix) {
			if (fileData[ix] == avcC[0] && fileData[ix + 1] == avcC[1] && fileData[ix + 2] == avcC[2] && fileData[ix + 3] == avcC[3]) {
				// 找到avcC，则记录avcRecord起始位置，然后退出循环。
				avcRecord = ix + 4;
				break;
			}
		}
		if (0 == avcRecord) {
			System.out.println("没有找到avcC，请检查文件格式是否正确");
			return null;
		}

		// 加7的目的是为了跳过
		// (1)8字节的 configurationVersion
		// (2)8字节的 AVCProfileIndication
		// (3)8字节的 profile_compatibility
		// (4)8 字节的 AVCLevelIndication
		// (5)6 bit 的 reserved
		// (6)2 bit 的 lengthSizeMinusOne
		// (7)3 bit 的 reserved
		// (8)5 bit 的numOfSequenceParameterSets
		// 共6个字节，然后到达sequenceParameterSetLength的位置
		int spsStartPos = avcRecord + 6;
		byte[] spsbt = new byte[] { fileData[spsStartPos], fileData[spsStartPos + 1] };
		int spsLength = bytes2Int(spsbt);
		byte[] SPS = new byte[spsLength];
		// 跳过2个字节的 sequenceParameterSetLength
		spsStartPos += 2;
		System.arraycopy(fileData, spsStartPos, SPS, 0, spsLength);
		map.put("sps", SPS);
		// 底下部分为获取PPS
		// spsStartPos + spsLength 可以跳到pps位置
		// 再加1的目的是跳过1字节的 numOfPictureParameterSets
		int ppsStartPos = spsStartPos + spsLength + 1;
		byte[] ppsbt = new byte[] { fileData[ppsStartPos], fileData[ppsStartPos + 1] };
		int ppsLength = bytes2Int(ppsbt);
		byte[] PPS = new byte[ppsLength];
		ppsStartPos += 2;
		System.arraycopy(fileData, ppsStartPos, PPS, 0, ppsLength);
		map.put("pps", PPS);
		return map;
	}
	
	private static int bytes2Int(byte[] bt) {
		int ret = bt[0];
		ret <<= 8;
		ret |= bt[1];
		return ret;
	}
	
	public static void main(String args[]) throws Exception{
		startVideoRecording("F:/out.h264");
	}
	
	public static void startVideoRecording(String fileName) throws Exception{
		Map<String, byte[]> map = getSPSAndPPS(fileName);
		byte[] h264pps = map.get("pps");
		byte[] h264sps = map.get("sps");
		H264Nal nalu = new H264Nal();
		
		InputStream fis = null;
		byte[] buffer = new byte[65535];
		try {
			fis = new FileInputStream(fileName);
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		DataInputStream dis = new DataInputStream(fis);
		
		// 要跳过前面的字节，到mdat为止
		byte[] mdat = new byte[] { 0x6d, 0x64, 0x61, 0x74 };
		byte[] buff = new byte[4];
		while (true) {
			buff[0] = buff[1];
			buff[1] = buff[2];
			buff[2] = buff[3];
			buff[3] = dis.readByte();
			if (buff[0] == mdat[0] && buff[1] == mdat[1] && buff[2] == mdat[2] && buff[3] == mdat[3])
				break;
		}
		
		nalu.setType(h264sps[0] & 0x1f);
		nalu.setRefIdc((h264sps[0] & 0x60) >>> 5);
		nalu.setPayload(h264sps);
		nalu.setPayloadLength(h264sps.length);
		P2PClientManager.getP2PClientInstance().send264Packet("", nalu);
		nalu.setType(h264pps[0] & 0x1f);
		nalu.setRefIdc((h264pps[0] & 0x60) >>> 5);
		nalu.setPayload(h264pps);
		nalu.setPayloadLength(h264pps.length);
		P2PClientManager.getP2PClientInstance().send264Packet("", nalu);
		
		while (true) {
			Log.i(Global.TAG, "recording====" + "loop starting....");
			Log.i(Global.TAG, "recording====" + "loop usingCam...." + usingCam);
			try {
				Log.i(Global.TAG, "recording====" + "loop dis.available()...." + dis.available());
				Log.i(Global.TAG, "recording====" + "loop buffer.length...." + buffer.length);
				int h264length = dis.readInt();
				Log.i(Global.TAG, "recording====" + "loop h264length==" + h264length);
				if (h264length > buffer.length) {
					Log.e(Global.TAG, "recording====h264length: " + h264length + ", loop h264length > buffer.length");
					while (dis.available() > 0) {
						if (dis.available() > buffer.length) {
							dis.read(buffer, 0, buffer.length);
						} else {
							dis.read(buffer, 0, dis.available());
						}
					}
				}
				dis.read(buffer, 0, h264length);
				byte[] naluData = Arrays.copyOfRange(buffer, 0, h264length);
				if (naluData.length <= 0) {
					Log.i(Global.TAG, "recording====" + "loop naluData.length==" + naluData.length);
					continue ;
				}
				nalu.setType(naluData[0] & 0x1f);
				nalu.setRefIdc((naluData[0] & 0x60) >>> 5);
				nalu.setPayload(naluData);
				nalu.setPayloadLength(naluData.length);
				
				Thread.sleep(15);
				
				P2PClientManager.getP2PClientInstance().send264Packet("", nalu);
			} catch (Exception e) {
				e.printStackTrace();
				break;
			}

			Log.i(Global.TAG, "recording====" + "loop ended....");
		}
	}

}
