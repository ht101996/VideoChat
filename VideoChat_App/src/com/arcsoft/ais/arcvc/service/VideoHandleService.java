//package com.arcsoft.ais.arcvc.service;
//
//import java.io.DataInputStream;
//import java.io.File;
//import java.io.IOException;
//import java.io.InputStream;
//import java.io.RandomAccessFile;
//
//public class VideoHandleService {
//
//	private void startVideoRecording() {
//		System.out.println("startVideoRecording");
//		(new Thread() {
//			public void run() {
//				int frame_size = 1024;
//				byte[] buffer = new byte[1024];
//				int num, number = 0;
//				InputStream fis = null;
//				try {
//					fis = receiver.getInputStream();
//				} catch (IOException e1) {
//					return;
//				}
//				try {
//					Thread.sleep(500);
//				} catch (InterruptedException e1) {
//					e1.printStackTrace();
//				}
//				number = 0;
//				// releaseMediaRecorder();
//				// 如果是H264或是MPEG_4_SP的就要在这里找到相应的设置参数的流
//				// avcC box H264的设置参数
//				// esds box MPEG_4_SP 的设置参数
//				// 其实 如果分辨率 等数值不变的话，这些参数是不会变化的，
//				// 那么我就只需要在第一次运行的时候确定就可以了
//				// while (true) {
//				// try {
//				// num = fis.read(buffer, number, frame_size);
//				// number += num;
//				// if (num < frame_size) {
//				// break;
//				// }
//				// } catch (IOException e) {
//				// break;
//				// }
//				// }
//
//				// initializeVideo(myCamera);
//				number = 0;
//				// 重新启动捕获，以获取视频流
//				DataInputStream dis = new DataInputStream(fis);
//
//				// 读取最前面的32个自己的空头
//				try {
//					dis.read(buffer, 0, 44);// LG G2要跳过40个字节
//				} catch (Exception e1) {
//					// TODO Auto-generated catch block
//					e1.printStackTrace();
//				}
//
//				try {
//					File file = new File("/sdcard/stream.h264");
//					if (file.exists())
//						file.delete();
//					raf = new RandomAccessFile(file, "rw");
//				} catch (Exception ex) {
//					ex.printStackTrace();
//				}
//
//				// 这些参数要对应我现在的视频设置，如果想变化的话需要去重新确定，
//				// 当然不知道是不是不同的机器是不是一样，我这里只有一个HTC G7做测试。
//				byte[] h264sps = { 0x67, 0x42, (byte) 0x80, 0x0B, (byte) 0xE4, (byte) 0x41, 0x62, (byte) 0x74, (byte) 0x03, (byte) 0x68, (byte) 0x50,
//						(byte) 0x9A, (byte) 0x80 };
//				byte[] h264pps = { 0x68, (byte) 0xCE, 0x38, (byte) 0x80 };
//				byte[] h264head = { 0, 0, 0, 1 };
//				try {
//					raf.write(h264head);
//					raf.write(h264sps);
//					raf.write(h264head);
//					raf.write(h264pps);
//				} catch (Exception e1) {
//					// TODO Auto-generated catch block
//					e1.printStackTrace();
//				}
//				while (true) {
//					try {
//						// 读取每场的长度
//						int h264length = dis.readInt();
//						System.out.println(h264length);
//						number = 0;
//						raf.write(h264head);
//						while (number < h264length) {
//							int lost = h264length - number;
//							num = fis.read(buffer, 0, frame_size < lost ? frame_size : lost);
//							number += num;
//							raf.write(buffer, 0, num);
//						}
//						// //////////////////////////////////////////////////////////
//						// int read = dis.read(buffer, 0, 1024);
//						// if (read == -1)
//						// break;
//						// raf.write(buffer);
//						// number += read;
//					} catch (IOException e) {
//						e.printStackTrace();
//						break;
//					}
//				}
//				try {
//					receiver.close();
//					sender.close();
//					lss.close();
//				} catch (IOException e) {
//					e.printStackTrace();
//				}
//			}
//		}).start();
//	}
//
//}
