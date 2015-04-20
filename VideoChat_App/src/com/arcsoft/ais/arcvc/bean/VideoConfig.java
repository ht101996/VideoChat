package com.arcsoft.ais.arcvc.bean;

public class VideoConfig {

	private int width;
	private int height;
	private byte[] sps;
	private byte[] pps;

	public byte[] getSps() {
		return sps;
	}

	public void setSps(byte[] sps) {
		this.sps = sps;
	}

	public byte[] getPps() {
		return pps;
	}

	public void setPps(byte[] pps) {
		this.pps = pps;
	}

	public int getWidth() {
		return width;
	}

	public void setWidth(int width) {
		this.width = width;
	}

	public int getHeight() {
		return height;
	}

	public void setHeight(int height) {
		this.height = height;
	}

}
