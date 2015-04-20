package com.arcsoft.ais.arcvc.utils;

/**
 * Common definition in ArcSoft VC
 */
public class CoreVCDef {
	/** added in 7.10; new in 7.19.4 default is to use CONNECT HTTP/1.1 */ 
	public static final int eCURLPROXY_HTTP = 0;
	/** added in 7.19.4; force to use CONNECT HTTP/1.0 */
	public static final int eCURLPROXY_HTTP_1_0 = 1;
	/** support added in 7.15.2; enum existed already in 7.10 */
	public static final int eCURLPROXY_SOCKS4 = 4;
	/** added in 7.10 */
	public static final int eCURLPROXY_SOCKS5 = 5;
	/** added in 7.18.0 */
	public static final int eCURLPROXY_SOCKS4A = 6;
	/** Use the SOCKS5 protocol but pass along the host name rather than the IP address. added  in 7.18.0 */
	public static final int eCURLPROXY_SOCKS5_HOSTNAME = 7;

	/**
	 * Description: UserRequestParam parameter, used when login to VC
	 */
	public static class PeerIdResponse{
		/** user id used when login to VC */
		public String szPeerId;
		public String szOnlineStatus;
	}

	/**
	 * Description: UserRequestParam parameter, used when login to VC
	 */
	public static class FriendResponse{
		/** user id used when login to VC */
		public long userId;
		public String szNickname;
		public String szOnlineStatus;
		public PeerIdResponse pPeerIdResponse[];
	}
	
}
