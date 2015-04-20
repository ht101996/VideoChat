package com.arcsoft.ais.arcvc.jni;

public class H264Nal {

	private int refIdc;
	private int type;
	private int payloadLength;
	private byte[] payload;

	public int getRefIdc() {
		return refIdc;
	}

	public void setRefIdc(int refIdc) {
		this.refIdc = refIdc;
	}

	public int getType() {
		return type;
	}

	public void setType(int type) {
		this.type = type;
	}

	public int getPayloadLength() {
		return payloadLength;
	}

	public void setPayloadLength(int payloadLength) {
		this.payloadLength = payloadLength;
	}

	public byte[] getPayload() {
		return payload;
	}

	public void setPayload(byte[] payload) {
		this.payload = payload;
	}

}
