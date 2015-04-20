/*

  This file is a part of JRTPLIB
  Copyright (c) 1999-2007 Jori Liesenborgs

  Contact: jori.liesenborgs@gmail.com

  This library was developed at the "Expertisecentrum Digitale Media"
  (http://www.edm.uhasselt.be), a research center of the Hasselt University
  (http://www.uhasselt.be). The library is based upon work done for 
  my thesis at the School for Knowledge Technology (Belgium/The Netherlands).

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

*/

/**
 * \file rtpnattvldestination.h
 */

#ifndef RTPNATTVLDESTINATION_H

#define RTPNATTVLDESTINATION_H

#include "rtpconfig.h"
#include "pos_addr.h"
#include "pos_type.h"
//#include "ganc_pub.h"
#include "rtpnattvladdress.h"

#if ! (defined(WIN32) || defined(_WIN32_WCE))
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/socket.h>
#endif // WIN32
#ifdef RTPDEBUG
	#include "rtpdefines.h"
	#include <stdio.h>
	#include <string>
#endif // RTPDEBUG
#include <string.h>

class RTPNATTVLDestination
{
public:
	RTPNATTVLDestination(uint32_t ip, uint16_t rtpportbase)	{
		m_RTPAddr.type	= POS_ADDR_SOCK;
		m_RTPAddr.addr.sockaddr.sin_family = AF_INET;
		m_RTPAddr.addr.sockaddr.sin_addr.s_addr = htonl(ip);
		m_RTPAddr.addr.sockaddr.sin_port = htons(rtpportbase);

		m_RTCPAddr.type	= POS_ADDR_SOCK;
		m_RTCPAddr.addr.sockaddr.sin_family = AF_INET;
		m_RTCPAddr.addr.sockaddr.sin_addr.s_addr = htonl(ip);
		m_RTCPAddr.addr.sockaddr.sin_port = htons(rtpportbase + 1);
	}

	RTPNATTVLDestination(Sint8 *GPID)	{
		m_RTPAddr.type	= POS_ADDR_UUID;
		memcpy(m_RTPAddr.addr.uuid, GPID, POS_UUID_LEN);

		m_RTCPAddr.type	= POS_ADDR_UUID;
		memcpy(m_RTCPAddr.addr.uuid, GPID, POS_UUID_LEN);
	}

	RTPNATTVLDestination(RTPNATTVLAddress& addr) {
		memcpy(&m_RTPAddr, &addr.getAddr(), sizeof(m_RTPAddr));
		memcpy(&m_RTCPAddr, &addr.getAddr(), sizeof(m_RTCPAddr));
		if (m_RTCPAddr.type == POS_ADDR_SOCK) {
			m_RTCPAddr.addr.sockaddr.sin_port += 1;
		}
	}

	Sint32 getType() const {
		return m_RTPAddr.type;
	}

	bool operator==(const RTPNATTVLDestination &src) const {
		return POS_ADDR_EQUAL((Tpos_addr *) &m_RTPAddr, (Tpos_addr *) &(src.m_RTPAddr));
		//if (src.getType() == getType()) {
		//	if (src.getType() == POS_ADDR_SOCK) {
		//		if (m_RTPAddr.addr.sockaddr.sin_addr.s_addr == src.m_RTPAddr.addr.sockaddr.sin_addr.s_addr &&
		//			m_RTPAddr.addr.sockaddr.sin_port == src.m_RTPAddr.addr.sockaddr.sin_port) {
		//				return true;
		//		}
		//	}
		//	else {
		//		if (memcmp(m_RTPAddr.addr.uuid, src.m_RTPAddr.addr.uuid, GANC_GPID_LENGTH) == 0) {
		//			return true;
		//		}
		//	}
		//}
		//return false;
	}

	uint32_t GetIP() const {
		return ntohl(m_RTPAddr.addr.sockaddr.sin_addr.s_addr);
	}

	// nbo = network byte order
	uint32_t GetIP_NBO() const {
		return m_RTPAddr.addr.sockaddr.sin_addr.s_addr;
	}

	uint16_t GetRTPPort_NBO() const	{
		return m_RTPAddr.addr.sockaddr.sin_port;
	}

	uint16_t GetRTCPPort_NBO() const {
		return m_RTCPAddr.addr.sockaddr.sin_port;
	}

	const Tpos_addr *GetRTPAddr() const	{
		return &m_RTPAddr;
	}

	const Tpos_addr *GetRTCPAddr() const {
		return &m_RTCPAddr;
	}

	Sint8 *getGPID() {
		return m_RTPAddr.addr.uuid;
	}

	Void setGPID(Sint8 *GPID) {
		memcpy(m_RTPAddr.addr.uuid, GPID, POS_UUID_LEN);
		memcpy(m_RTCPAddr.addr.uuid, GPID, POS_UUID_LEN);
	}

	Void setIP(Uint32 IP) {
		m_RTPAddr.addr.sockaddr.sin_addr.s_addr = IP;
		m_RTCPAddr.addr.sockaddr.sin_addr.s_addr = IP;
	}

	Void setPort(Uint16 port) {
		m_RTPAddr.addr.sockaddr.sin_port = port;
		m_RTCPAddr.addr.sockaddr.sin_port = port + 1;
	}

	Uint32 getHashIndex() const {
		Uint32 hashIndex = 0;
		if (m_RTPAddr.type == POS_ADDR_SOCK) {
			hashIndex = GetIP();
		}
		if (m_RTPAddr.type == POS_ADDR_UUID) {
			hashIndex	= m_RTPAddr.addr.uuid[0] << 3;
			hashIndex	+= m_RTPAddr.addr.uuid[1] << 2;
			hashIndex	+= m_RTPAddr.addr.uuid[2] << 1;
			hashIndex	+= m_RTPAddr.addr.uuid[3];
		}
		return hashIndex;
	}

#ifdef RTPDEBUG
	std::string GetDestinationString() const;
#endif // RTPDEBUG
private:
	Tpos_addr m_RTPAddr;
	Tpos_addr m_RTCPAddr;
};

#ifdef RTPDEBUG
inline std::string RTPIPv4Destination::GetDestinationString() const
{
	char str[24];
	uint32_t ip = ipaddr_hbo;
	uint16_t portbase = ntohs(rtpport_nbo);
	
	RTP_SNPRINTF(str,24,"%d.%d.%d.%d:%d",(int)((ip>>24)&0xFF),(int)((ip>>16)&0xFF),(int)((ip>>8)&0xFF),(int)(ip&0xFF),(int)(portbase));
	return std::string(str);
}
#endif // RTPDEBUG

#endif // RTPIPV4DESTINATION_H

