/* $Id: XMLArchive.h,v 1.5 2004/02/02 18:47:39 dick Exp $
 *
 *  Extend Archive to do XML writing.
 *  (XML reading is done by the expat library)
 *
 *      Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * See the file COPYRIGHT.TXT for current copyright information.
 *
 */
/*
 * $Log: XMLArchive.h,v $
 * Revision 1.5  2004/02/02 18:47:39  dick
 * XMLArchive.h
 *
 * Revision 1.4  2004/01/06 03:32:37  dick
 * xarParseBool was incorrect
 *
 * Revision 1.3  2004/01/05 03:30:11  dick
 * gcc 3 does not like token pasting into a function name.  So unwind one level
 * of the xml macros, so we say ParseInt(attr) instead of ParseAttr(attr, Int).
 *
 * Revision 1.2  2002/07/14 19:00:01  dick
 * xarParseAttrType() lets us coerce the return type to something else.
 *
 * Revision 1.1  2002/06/25 04:35:49  dick
 * Wrap Archive with XML handling.
 *
 */


#ifndef	_XMLARCHIVE_H_
#define	_XMLARCHIVE_H_

#include "expat.h"
#include "Archive.h"
#include "Obj.h"

#if	0 && defined(_DEBUG)
#define	TRACEXML(__s)		__s
#else
#define	TRACEXML(__s)
#endif


bool	ParseBool(PCSTR s);
int		ParseInt(PCSTR s);
String	ParseString(PCSTR s);
Rect	ParseRect(PCSTR s);
double	ParseFloat(PCSTR s);

#define	XMLArchiveSUPERCLASS	Archive
class XMLArchive : public XMLArchiveSUPERCLASS
{
  public:
	XMLArchive() { level = 0; }

	// Archive*	car;

	void	WriteHead();
	void	WriteIndent();
	void	WriteString(PCSTR s, ...);
	void	_WriteElementStart(PCSTR s);
	void	_WriteElementEnd(PCSTR s);
	void	_WriteBoolAttr(PCSTR s, int b);
	void	_WriteIntAttr(PCSTR s, int b);
	void	_WriteFloatAttr(PCSTR s, double b);
	void	_WriteStringAttr(PCSTR s, PCSTR b);
	void	_WriteRectAttr(PCSTR s, const Rect& r);

//  private:
	int		level;

};

#define	WriteElementStart(__s)	_WriteElementStart(xmltoken_##__s)
#define	WriteElementEnd(__s)	_WriteElementEnd(xmltoken_##__s)
#define	WriteBoolAttr(__s)		_WriteBoolAttr(xmltoken_##__s, __s)
#define	WriteIntAttr(__s)		_WriteIntAttr(xmltoken_##__s, __s)
#define	WriteFloatAttr(__s)		_WriteFloatAttr(xmltoken_##__s, __s)
#define	WriteStringAttr(__s)	_WriteStringAttr(xmltoken_##__s, __s)
#define	WriteRectAttr(__s)		_WriteRectAttr(xmltoken_##__s, __s)


#define xarDeclareToken(__s)	const char* xmltoken_##__s=#__s
#define	xarDefineToken(__s)		extern const char* xmltoken_##__s
#define	xarMatchToken(__s)		(!strcmp(el, xmltoken_##__s))

#define	xarParseAttr(__s, __t)	if (xarMatchToken(__s)) \
									xarObjPREFIX.__s = Parse##__t##(attr[1])

#define	xarParseBool(__s)		if (xarMatchToken(__s)) \
									xarObjPREFIX.__s = ParseBool(attr[1])

#define	xarParseFloat(__s)		if (xarMatchToken(__s)) \
									xarObjPREFIX.__s = ParseFloat(attr[1])

#define	xarParseInt(__s)		if (xarMatchToken(__s)) \
									xarObjPREFIX.__s = ParseInt(attr[1])

#define	xarParseRect(__s)		if (xarMatchToken(__s)) \
									xarObjPREFIX.__s = ParseRect(attr[1])

#define	xarParseString(__s)		if (xarMatchToken(__s)) \
									xarObjPREFIX.__s = ParseString(attr[1])

/////
#define	xarParseAttrType(__s, __t, __r)	\
								if (xarMatchToken(__s)) \
									xarObjPREFIX.__s = (__r)Parse##__t##(attr[1])

#define	xarParseIntType(__s, __r)	\
								if (xarMatchToken(__s)) \
									xarObjPREFIX.__s = (__r)ParseInt(attr[1])
////
#define	xarParseObject(__s)		if (xarMatchToken(__s)) \
									__s::SetXMLParseHandlers(d->xmlp, \
											xarOBJECT::XMLStartHandler, \
											xarOBJECT::XMLEndHandler)

#define	xarParseObjectEnd(__s)	if (xarMatchToken(__s))					\
								xarObjPREFIX.PopXMLParseHandlers(d->xmlp)

#define	xarOPERATORS	static void	SetXMLParseHandlers(XML_Parser xmlp,	\
										XML_StartElementHandler seh,		\
										XML_EndElementHandler eeh);			\
						static void	PopXMLParseHandlers(XML_Parser xmlp);	\
						void	XMLOut(XMLArchive& xar);

#define	xarHANDLERS		static void		XMLStartHandler(void *data, const char *el, const char **attr); \
						static void		XMLEndHandler(void *data, const char *el);	\
						static XML_StartElementHandler		xmlParentSeh;			\
						static XML_EndElementHandler		xmlParentEeh;			\


#define	xarDECLARES		XML_StartElementHandler xarOBJECT::xmlParentSeh;	\
						XML_EndElementHandler	xarOBJECT::xmlParentEeh;	\
						void xarOBJECT::SetXMLParseHandlers(XML_Parser xmlp, \
								  XML_StartElementHandler seh, XML_EndElementHandler eeh)	\
							{ xarOBJECT::xmlParentSeh = seh; xarOBJECT::xmlParentEeh = eeh; \
							  XML_SetElementHandler(xmlp, XMLStartHandler, XMLEndHandler); } \
						void xarOBJECT::PopXMLParseHandlers(XML_Parser xmlp) \
							{ XML_SetElementHandler(xmlp, xarOBJECT::xmlParentSeh, xarOBJECT::xmlParentEeh); } \


#define	TRACEXMLSTART(__s)	TRACEXML(Trace(#__s ": start: %s \\t=%d", el, level);	\
							for (int i=0; attr[i] != NULL; i++)		\
								Trace("attr[%d]=%s ", i, attr[i]);	\
							Trace("\n"); )							\

#define	TRACEXMLEND(__s)	TRACEXML(Trace(#__s ": end: %s \\t=%d\n", level);)

#endif		// _XMLARCHIVE_H_
