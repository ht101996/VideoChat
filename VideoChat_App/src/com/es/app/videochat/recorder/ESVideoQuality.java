
package com.es.app.videochat.recorder;

/**
 * A class that represents the quality of a video stream. 
 * It contains the resolution, the framerate (in fps) and the bitrate (in bps) of the stream.
 */
public class ESVideoQuality {

	/** Default video stream quality. */
	public final static ESVideoQuality DEFAULT_VIDEO_QUALITY = new ESVideoQuality(320,240,25,125000);

	/**	Represents a quality for a video stream. */ 
	public ESVideoQuality() {}

	/**
	 * Represents a quality for a video stream.
	 * @param resX The horizontal resolution
	 * @param resY The vertical resolution
	 * @param framerate The framerate in frame per seconds
	 * @param bitrate The bitrate in bit per seconds
	 * @param orientation The orientation of the video in the SurfaceView  
	 */
	public ESVideoQuality(int resX, int resY, int framerate, int bitrate, int orientation) {
		this.framerate = framerate;
		this.bitrate = bitrate;
		this.resX = resX;
		this.resY = resY;
		this.orientation = orientation;
	}	

	/**
	 * Represents a quality for a video stream.
	 * @param resX The horizontal resolution
	 * @param resY The vertical resolution
	 * @param framerate The framerate in frame per seconds
	 * @param bitrate The bitrate in bit per seconds 
	 */
	public ESVideoQuality(int resX, int resY, int framerate, int bitrate) {
		this.framerate = framerate;
		this.bitrate = bitrate;
		this.resX = resX;
		this.resY = resY;
	}

	public int framerate = 0;
	public int bitrate = 0;
	public int resX = 0;
	public int resY = 0;
	public int orientation = 90;

	public boolean equals(ESVideoQuality quality) {
		if (quality==null) return false;
		return (quality.resX == this.resX 				&
				quality.resY == this.resY 				&
				quality.framerate == this.framerate	&
				quality.bitrate == this.bitrate 		&
				quality.orientation == this.orientation);
	}

	public ESVideoQuality clone() {
		return new ESVideoQuality(resX,resY,framerate,bitrate);
	}

}
