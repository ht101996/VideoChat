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
 * \file rtpnattvladdress.h
 */

#ifndef RTPNATTVLDDRESS_H

#define RTPNATTVLDDRESS_H

#include "rtpconfig.h"
#include "rtpaddress.h"
#include "rtptypes.h"
#include "pos_addr.h"
#include "pos_type.h"
//#include "ganc_pub.h"

class RTPMemoryManager;

/** Represents an NAT traversal address.
 *  This class is used by the UDP over NAT traversal transmission component.
 *  When an RTPNATTVLAddress is used in one of the multicast functions of the transmitter, the port 
 *  number is ignored. When an instance is used in one of the accept or ignore functions of the 
 *  transmitter, a zero port number represents all ports for the specified IP address.
 */
class RTPNATTVLAddress : public RTPAddress
{
public:
	/** Creates an instance with IP address \c ip and port number \c port (both are interpreted in host byte order). */
	RTPNATTVLAddress(Uint32 ip = 0, Uint16 port = 0) : RTPAddress(NATTVLAddress) {
		SetIP(ip);
		SetPort(port);
	}
	
	/** Creates an instance with IP address \c ip and port number \c port (\c port is interpreted in host byte order). */
	RTPNATTVLAddress(CONST Uint8 ip[4], Uint16 port ) : RTPAddress(NATTVLAddress) {
		SetIP(ip);
		SetPort(port);
	}

	RTPNATTVLAddress(Sint8 *GPID) : RTPAddress(NATTVLAddress) {
		m_Addr.type = POS_ADDR_UUID;
		memcpy(m_Addr.addr.uuid, GPID, POS_UUID_LEN);
	}

	RTPNATTVLAddress(Tpos_addr *addr) : RTPAddress(NATTVLAddress) {
		memcpy(&m_Addr, addr, sizeof(Tpos_addr));
	}

	~RTPNATTVLAddress()	{}

	/** Sets the IP address for this instance to \c ip which is assumed to be in host byte order. */
	Void SetIP(Uint32 ip)	{
		m_Addr.type = POS_ADDR_SOCK;
		m_Addr.addr.sockaddr.sin_addr.s_addr = ip;
	}

	/** Sets the IP address of this instance to \c ip. */
	Void SetIP(CONST Uint8 ip[4]) {
		m_Addr.type = POS_ADDR_SOCK;
		m_Addr.addr.sockaddr.sin_addr.s_addr = (Uint32) ip[3];
		m_Addr.addr.sockaddr.sin_addr.s_addr |= (((Uint32) ip[2]) << 8);
		m_Addr.addr.sockaddr.sin_addr.s_addr |= (((Uint32) ip[1]) << 16);
		m_Addr.addr.sockaddr.sin_addr.s_addr |= (((Uint32) ip[0]) << 24);
	}

	/** Sets the port number for this instance to \c port which is interpreted in host byte order. */
	Void SetPort(Uint16 port) {
		m_Addr.type = POS_ADDR_SOCK;
		m_Addr.addr.sockaddr.sin_port = port;
	}

	/** Returns the IP address contained in this instance in host byte order. */
	Uint32 GetIP() const {
		if (m_Addr.type == POS_ADDR_SOCK) {
			return ntohl(m_Addr.addr.sockaddr.sin_addr.s_addr);
		}
		return 0;
	}

	/** Returns the port number of this instance in host byte order. */
	Uint16 GetPort() CONST {
		if (m_Addr.type == POS_ADDR_SOCK) {
			return ntohs(m_Addr.addr.sockaddr.sin_port);
		}
		return 0;
	}

	RTPAddress *CreateCopy(RTPMemoryManager *mgr) CONST;
	bool IsSameAddress(const RTPAddress *addr) CONST;
	bool IsFromSameHost(const RTPAddress *addr) CONST;

	Sint32 getType() CONST {
		return m_Addr.type;
	}

	Sint8* getGPID() CONST {
		if (m_Addr.type == POS_ADDR_UUID) {
			return (Sint8 *) m_Addr.addr.uuid;
		}
		return POS_NULL;
	}

	Tpos_addr& getAddr() {
		return m_Addr;
	}

#ifdef RTPDEBUG
	std::string GetAddressString() CONST;
#endif // RTPDEBUG
private:
	Tpos_addr m_Addr;
};

#endif // RTPNATTVLDDRESS_H

