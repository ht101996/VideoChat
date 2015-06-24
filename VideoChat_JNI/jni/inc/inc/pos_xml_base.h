/*----------------------------------------------------------------------------------------------
*
* This file is ArcSoft's property. It contains ArcSoft's trade secret, proprietary and         
* confidential information. 
* 
* The information and code contained in this file is only for authorized ArcSoft employees 
* to design, create, modify, or review.
* 
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER AUTHORIZATION.
* 
* If you are not an intended recipient of this file, you must not copy, distribute, modify, 
* or take any action in reliance on it. 
* 
* If you have received this file in error, please immediately notify ArcSoft and 
* permanently delete the original and any copy of any file and any printout thereof.
*
*-------------------------------------------------------------------------------------------------*/

/*
 * File Name:       pos_xml_base.h
 *
 * Reference:
 *
 * Author:          Liu Gen
 *
 * Description:
 *      a simple xml paser and serialize base class
 *
 * History:
 *      please record the history in the following format:
 *      verx.y.z    date        author      description or bug/cr number
 *      ----------------------------------------------------------------
 *      ver0.0.1    05/10/2010  Liu Gen     first draft
 *  
 *CodeReview Log:
 *      please record the code review log in the following format:
 *      verx.y.z    date        author      description
 *      ----------------------------------------------------------------
 * 
 */

#ifndef _POS_XMLBASE_H
#define _POS_XMLBASE_H

#include "pos_prolog.h"

#include "pos_stdio.h"
#include "pos_stdlib.h"
#include "pos_string.h"

#include "pos_type.h"
#include "pos_log.h"
#include "pos_file.h"
#include "pos_mm.h"
#include "pos_xml.h"
#include <pos_md5.h>

#if defined (__WIN32__)
#pragma warning (disable : 4996)
#endif

#define COMPARE_LAST_N_BYTES        2
#define COMPARE_FIRST_N_BYTES       (COMPARE_LAST_N_BYTES)
#define VRF_CODE_LENGTH             24

typedef enum xml_packet_flags
{
	XML_PACKET_FLAG_NONE = 0,
	XML_PACKET_FLAG_BASE64 = 0x000000001, /*ask for encoding to BASE64*/
	XML_PACKET_FLAG_ENTITY_CODE = 0x000000002, /*ask for encoding to BASE64*/
}xml_packet_flags;
CONST STATIC Sint8 BASE64_START_1ST_CHAR = 0x7F;
CONST STATIC Sint8 BASE64_START_2ND_CHAR = 0x7D;

typedef enum pjsua_server_type
{
	PJSUA_UCS,				// communicate with UCS
	PJSUA_UIS,				// communicate with UIS
	PJSUA_MP,				// communicate with MP
}pjsua_server_type;

#ifdef __cplusplus


#define XML_ENTITY_ENCODE_ENABLE
#define XML_BASE_ENTITY_BUFFER  1024
class XMLBase
{
public:
    XMLBase() {
        m_iEntityBufLength = XML_BASE_ENTITY_BUFFER;
        m_pEntityBuf = (Sint8*)pos_mm_malloc(XML_BASE_ENTITY_BUFFER);
        memset(m_pEntityBuf, 0, m_iEntityBufLength);
        m_iElementValueLength = 0;
        m_pElementValue = (Sint8*)pos_mm_malloc(XML_BASE_ENTITY_BUFFER);
        memset(m_pElementValue, 0, XML_BASE_ENTITY_BUFFER);
        }     
    virtual ~XMLBase() {
        pos_mm_free(m_pEntityBuf);
        m_pEntityBuf = 0;
        pos_mm_free(m_pElementValue);
        m_pElementValue = 0;
        }
    
/**************************************************************
* xml entity encode, the followings are entities
*    < => &lt;
*    > => &gt;
*    & => &amp;
*    ' => &apos;
*    " => &quot;
* return the byte length after encode
**************************************************************/
    Sint32 XMLEntityEncode(Sint8* pszIn, Sint32 iInLen, Sint8* pszOut, Sint32 *piOutLen)
    {
    	//variable define
    	Sint8* inPos=pszIn;
    	int outLength=0;

    	//check parameters
    	if((NULL==pszOut) || (NULL==inPos)){
    		return 0;
    	}

    	//while encode
    	while(inPos && iInLen>0){//while get every character
    		switch(*inPos){
    			case '<':
    				memcpy(pszOut+outLength,"&lt;",4);
    				outLength+=4;
    				break;
    			case '>':
    				memcpy(pszOut+outLength,"&gt;",4);
    				outLength+=4;
    				break;
    			case '&':
    				memcpy(pszOut+outLength,"&amp;",5);
    				outLength+=5;
    				break;
    			case '\'':
    				memcpy(pszOut+outLength,"&apos;",6);
    				outLength+=6;
    				break;
    			case '\"':
    				memcpy(pszOut+outLength,"&quot;",6);
    				outLength+=6;
    				break;
    			default:
    				pszOut[outLength]=*inPos;
    				outLength++;
    				break;
    		}
    		inPos++;
    		iInLen--;
    	}
        *piOutLen = outLength;
    	return outLength;
    }
/**************************************************************
* xml entity decode, the followings are entities
*    &lt; => <
*    &gt; => >
*    &amp; => &
*    &apos; => '
*    &quot; => "
* return the byte length after decode
**************************************************************/
    Sint32 XMLEntityDecode(Sint8* pszIn, Sint32 iInLen, Sint8* pszOut, Sint32 *piOutLen)
    {
    	//variable define
    	Sint8 *inPos=pszIn;
    	Sint32 outLength=0;

    	//check parameters
    	if((NULL==pszOut) || (NULL==inPos)){
    		return 0;
    	}

    	//while decode
    	while(inPos && iInLen>0){//while get every character
    		if(*inPos == '&'){
    			if(0==strncmp(inPos,"&lt;",4)){//if is <
    				inPos+=4;
    				pszOut[outLength]='<';
    			}else if(0==strncmp(inPos,"&gt;",4)){//if is >
    				inPos+=4;
    				pszOut[outLength]='>';
    			}else if(0==strncmp(inPos,"&amp;",5)){//if is &
    				inPos+=5;
    				pszOut[outLength]='&';
    			}else if(0==strncmp(inPos,"&apos;",6)){//if is '
    				inPos+=6;
    				pszOut[outLength]='\'';
    			}else if(0==strncmp(inPos,"&quot;",6)){//if is "
    				inPos+=6;
    				pszOut[outLength]='\"';
    			}else{
    				pszOut[outLength]=*inPos;
    				inPos++;
    			}			
    		}else{
    			pszOut[outLength]=*inPos;
    			inPos++;
                iInLen--;
    		}
    		outLength++;
    	}
        *piOutLen = outLength;
    	return outLength;
    }
    Sint32 PacketBegin(Sint8 *pBuf, Sint32 iLength)
    {
        m_pData = pBuf;
        m_iDataLength = iLength;
        return POS_OK;
    }

    Sint32 PacketXMLHeader(Sint8 *pXMLNameSpace, Sint8 *pXMLSchema, Sint8 *pSessionName)
    {

        if(m_pData == 0 || m_iDataLength < 256 || pXMLNameSpace == POS_NULL || pXMLSchema == POS_NULL)
            return POS_FAIL;

        
#ifdef __USING_XML_HEADER__         
        sprintf(m_pData, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<tns:%s xmlns:tns=\"http://www.example.org/%s\" \
xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
xsi:schemaLocation=\"http://www.example.org/%s %s \" \
xsi:type=\"tns:cmd_hd_st\">", pSessionName, pXMLNameSpace, pXMLNameSpace, pXMLSchema);
#else
sprintf(m_pData, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<tns:%s xmlns:tns=\"http://www.example.org/a\">", pSessionName);
#endif
        sprintf(m_SessionName, "%s", pSessionName);
        return POS_OK;
    }

    Sint32 PacketElementInt(Sint8 *pName, Sint32 iNameLen, Sint32 iValue)
    {
        char buf[256];
        iNameLen = iNameLen;
        sprintf(buf, "<%s>%d</%s>", pName, (int)iValue, pName);
        if((strlen(buf)+strlen(m_pData)) < (Uint32)m_iDataLength)
        {
            sprintf((m_pData+strlen(m_pData)), "%s", buf);
        }
        else
            return POS_FAIL;
        
        return POS_OK;
    }

    Sint32 PacketElementUnsignedInt(Sint8 *pName, Sint32 iNameLen, Uint32 uiValue)
    {
        iNameLen = iNameLen;
        char buf[256];
        sprintf(buf, "<%s>%u</%s>", pName, (unsigned int)uiValue, pName);
        if((strlen(buf)+strlen(m_pData)) < (Uint32)m_iDataLength)
        {
            sprintf(m_pData+strlen(m_pData), "%s", buf);
        }
        else
            return POS_FAIL;
        return POS_OK;
    }
    Sint32 PacketElementString(Sint8 *pName, Sint32 iNameLen, Sint8 *pValue, Sint32 iValueLen, Uint32 flags = XML_PACKET_FLAG_ENTITY_CODE)
    {
	    char *p=POS_NULL;
	    Sint32 len;

	    /*@2010.5.25 yjchen: I modified these codes. Only copy iNameLen of pName and iValeLen of pValue.
	    *  The orginal codes are:
	    * sprintf(buf, "<tns:%s>%s</tns:%s>", pName, pValue, pName);
	    * if((strlen(buf)+strlen(m_pData)) < m_iDataLength)
	    * {
	    * 	sprintf(m_pData+strlen(m_pData), "%s", buf);
	    * }
	    * else
	    * 	return -1;
	    */
	    pos_assert((POS_NULL != pName) && (POS_NULL!=pValue));

    /* Add begin by genliu @2010-12-01: add entity encode. */
 
#ifdef XML_ENTITY_ENCODE_ENABLE
    memset(m_pEntityBuf, 0, m_iEntityBufLength);
    if(XML_PACKET_FLAG_NONE != flags)
    {
        XMLEntityEncode(pValue, iValueLen, m_pEntityBuf, &iValueLen);
        pValue = m_pEntityBuf;
    }
#else
    /* Add end by genliu @2010-12-01 */

	/*@2010.8.9 yjchen : add for base64
	* Rule:
	* 1, If need to encode to base64, add BASE64_START_1ST_CHAR 
	*       and BASE64_START_2ND_CHAR at the beginning. 
	*     And then encode the string to base64, put them just following BASE64_START_2ND_CHAR.
	* 2, If no need to encode to base64 but the first char is BASE64_START_1ST_CHAR, add another
	*    BASE64_START_1ST_CHAR at the beginning and then put the string after it.
	* 3, If not 1 and 2, do nothing to string, just packet it.
	*/
	Sint8 ischanged = 0;
	char buf[512] = {0};
	len = iValueLen;
	if(XML_PACKET_FLAG_BASE64&flags)
	{
		len = iValueLen*3/2;/*not precise.....*/
	}
	else if(BASE64_START_1ST_CHAR == *pValue)
	{
		len++;
		ischanged = 1;
	}
	/****************************/
			
	    if((iNameLen*2 + len/*iValueLen*/ + 5 + 1 + 6 + 2 + strlen(m_pData)) > (Uint32)m_iDataLength)
	    {
			POS_LOG("ERROR! pos_xml_base::PacketElementString: Your buffer is too small\n");
		    return POS_FAIL;
	    }

	/*@2010.8.9 yjchen : add for base64. Please refer to Rule 1*/
	if(XML_PACKET_FLAG_BASE64&flags)
	{
		len = sizeof(buf) -2;
		/*add our cipher*/
		buf[0] = BASE64_START_1ST_CHAR;
		buf[1] = BASE64_START_2ND_CHAR;
		if(POS_OK == pos_base64_encode((Uint8 *)pValue,iValueLen,buf+2,&len))
		{
			pValue = buf;
			iValueLen = len+2;/*add first two cipher's characters*/
#ifdef TIXCAP_DEBUG
			buf[iValueLen] =0;
			POS_LOG("PacketElementString: base64_encode:%s\n",buf);
#endif
		}
		else
		{
			POS_LOG("ERROR! pos_xml_base::PacketElementString: pos_base64_encode fail\n");
			return POS_FAIL;
		}
	}
	/****************************/
#endif
	    p = m_pData+strlen(m_pData);
		
	    len = 1/*strlen("<")*/;
	    memcpy(p,"<",len);
	    p += len;
	    memcpy(p,pName,iNameLen);
	    p += iNameLen;
	    *p = '>';
	    p++;
#ifdef XML_ENTITY_ENCODE_ENABLE
#else
	if(1 == ischanged)
	{/*@2010.7.26 yjchen : add one BASE64_START_1ST_CHAR, please refer to Rule 2*/
		*p = BASE64_START_1ST_CHAR;
		p++;
	}
#endif  

	    memcpy(p,pValue,iValueLen);
	    p += iValueLen;
	    len = 2/*strlen("</")*/;
	    memcpy(p,"</",len);
	    p += len;
	    memcpy(p,pName,iNameLen);
	    p += iNameLen;
	    *p = '>';
	    p++;
	    *p = 0;
	    return POS_OK;
    }


    Sint32 PacketEnd()
    {
        sprintf(m_pData+strlen(m_pData), "</tns:%s>", m_SessionName);
        return POS_OK;
    }

    Sint32 ParseBegin(Sint8 *pData, Sint32 iLength)
    {
        m_pData = pData;
        m_iDataLength = iLength;
        m_Parser = pos_xml_parser_create(NULL);
        pos_xml_set_element_handler(m_Parser, ElementStart, ElementEnd);
        pos_xml_set_character_data_handler(m_Parser, ElementValue);
        pos_xml_set_user_data(m_Parser, this);
        pos_xml_parse(m_Parser, pData, iLength, 0);
        pos_xml_parser_close(m_Parser);
        m_Parser = 0;
        return POS_OK;
    }

    virtual Sint32 ParseXMLHeader(Sint8 *pXMLNameSpace, Sint8 *pXMLSchema, Sint8 *pSessionName)
        {
            pXMLNameSpace = pXMLNameSpace;
            pXMLSchema = pXMLSchema;
            pSessionName = pSessionName;
            return 0;
        }
    virtual Sint32 ParseElement(Sint8 *pName, Sint32 iNameLen, Sint8 *pValue, Sint32 iValueLen) = 0;
    Sint32 ParseEnd() {return 0;}

private:
    STATIC Void ElementStart(Void *user_data_ptr, CONST TXML_Char *ele_name_ptr,
                                               CONST TXML_Char **ele_atts_ptr)
    {
        ele_atts_ptr = ele_atts_ptr;
        XMLBase *pBase = (XMLBase *)user_data_ptr;
        int i = 0;
        int iNameStrLen = (int)strlen(ele_name_ptr);
        char *pString = 0;
        
        for(i=0; i<iNameStrLen; i++)
        {
            if(ele_name_ptr[i] == ':')
            {
                pString = (char *)&ele_name_ptr[i+1];
                break;
            }
        }

        if(pString == 0)
            pString = (char *)ele_name_ptr;
        pBase->m_ElementLength = (int)pos_min(sizeof(pBase->m_ElementName), strlen(pString));
        memset((void *)pBase->m_ElementName, 0, sizeof(pBase->m_ElementName));
        memcpy(pBase->m_ElementName, pString, pBase->m_ElementLength);
        memset(pBase->m_pElementValue, 0, XML_BASE_ENTITY_BUFFER);
        pBase->m_iElementValueLength = 0;
        return;
    }

    STATIC Void ElementEnd(Void *user_data_ptr, CONST TXML_Char *ele_name_ptr)
    {
        ele_name_ptr = ele_name_ptr;
        XMLBase *pBase = (XMLBase *)user_data_ptr;
        //#ifdef XML_ENTITY_ENCODE_ENABLE
        //Sint32 iLength = 0;
        //memset(pBase->m_pEntityBuf, 0, pBase->m_iEntityBufLength);
        //pBase->XMLEntityDecode((Sint8 *)pBase->m_pElementValue, pBase->m_iElementValueLength, 
        //            pBase->m_pEntityBuf, &iLength);
        //#endif
        if(pBase->m_ElementLength > 0) // ignore the whole xml's end tag callback.
            pBase->ParseElement((char *)pBase->m_ElementName, pBase->m_ElementLength,
                                (char *)pBase->m_pElementValue, pBase->m_iElementValueLength);
        pBase->m_ElementLength = 0;
        return;
    }

    STATIC Void ElementValue(Void *user_data_ptr, CONST TXML_Char *str, Sint32 len)
    {
        XMLBase *pBase = (XMLBase *)user_data_ptr;
        /* Add begin by genliu @2010-12-01: add xml entity decode */
#ifdef XML_ENTITY_ENCODE_ENABLE

#else
	/*@2010.8.9 yjchen : check if need decoding*/
	Uint8 buf[512];
	if(BASE64_START_1ST_CHAR==*str)
	{
		if(BASE64_START_2ND_CHAR==*(str+1))
		{/*Rule 1*/
			Sint32 olen = sizeof(buf);
			if(POS_OK == pos_base64_decode(str+2,len-2,buf,&olen))
			{
				str = (Sint8 *)buf;
				len = olen;
#ifdef TIXCAP_DEBUG
				buf[olen] = 0;
				POS_LOG("ElementValue: base64_decode:%s\n",buf);
#endif
			}
			else
			{
				POS_LOG("TiXCAPBase::ElementValue: pos_base64_decode ERROR\n");
				return;
			}
		}
		else if(BASE64_START_1ST_CHAR==*(str+1))
		{ /*Rule 2*/
		   /* remove one 0x7F, we have add one while packeting.
		   * please refer to PacketElementString
		   */
			str++;
			len--;
		}
	}

	/*****************************************/
#endif
        /* Add end by genliu @2010-12-01*/
        memcpy(pBase->m_pElementValue+pBase->m_iElementValueLength, 
                        str, len);
        pBase->m_iElementValueLength += len; 

        return;
    }

private:
    char *m_pElementValue;
    int m_iElementValueLength;    
    char *m_pEntityBuf;
    int m_iEntityBufLength;
    char *m_pData;
    int m_iDataLength;
    char m_SessionName[64];
    Tpos_xml_parser m_Parser;
    char m_ElementName[64];
    int m_ElementLength;

};
#endif

#endif // _POS_XMLBASE_H
