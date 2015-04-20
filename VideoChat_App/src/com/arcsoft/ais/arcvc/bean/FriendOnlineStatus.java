package com.arcsoft.ais.arcvc.bean;

public class FriendOnlineStatus {
	public long userId;
	public String nickname;
	public String onlineStatus;
	private String receivedMsgFlag;
	public PeerIdOnlineStatus peerIdOnlineStatus[];
	
	public long getUserId() {
		return userId;
	}
	public void setUserId(long userId) {
		this.userId = userId;
	}
	public String getNickname() {
		return nickname;
	}
	public void setNickname(String nickname) {
		this.nickname = nickname;
	}
	public String getOnlineStatus() {
		return onlineStatus;
	}
	public void setOnlineStatus(String onlineStatus) {
		this.onlineStatus = onlineStatus;
	}
	public PeerIdOnlineStatus[] getPeerIdOnlineStatus() {
		return peerIdOnlineStatus;
	}
	public void setPeerIdOnlineStatus(PeerIdOnlineStatus[] peerIdOnlineStatus) {
		this.peerIdOnlineStatus = peerIdOnlineStatus;
	}
	public String getReceivedMsgFlag() {
		return receivedMsgFlag;
	}
	public void setReceivedMsgFlag(String receivedMsgFlag) {
		this.receivedMsgFlag = receivedMsgFlag;
	}
	
}
