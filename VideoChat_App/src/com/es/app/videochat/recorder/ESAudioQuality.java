
package com.es.app.videochat.recorder;

/**
 * A class that represents the quality of an audio stream. 
 */
public class ESAudioQuality {

	/** Default audio stream quality. */
	public final static ESAudioQuality DEFAULT_AUDIO_QUALITY = new ESAudioQuality(44100,128000);

	/**	Represents a quality for a video stream. */ 
	public ESAudioQuality() {}

	/**
	 * Represents a quality for an audio stream.
	 * @param samplingRate The sampling rate
	 * @param bitRate The bitrate in bit per seconds
	 */
	public ESAudioQuality(int samplingRate, int bitRate) {
		this.samplingRate = samplingRate;
		this.bitRate = bitRate;
	}	

	public int samplingRate = 0;
	public int bitRate = 0;

	public boolean equals(ESAudioQuality quality) {
		if (quality==null) return false;
		return (quality.samplingRate == this.samplingRate 				&
				quality.bitRate == this.bitRate);
	}

	public ESAudioQuality clone() {
		return new ESAudioQuality(samplingRate, bitRate);
	}

}
