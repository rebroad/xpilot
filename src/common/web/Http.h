/* $Id: Http.h,v 1.4 2004/01/24 00:43:04 dick Exp $
 *
 * Our interface to a web browser
 *
 *      Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: Http.h,v $
 * Revision 1.4  2004/01/24 00:43:04  dick
 * Note the IP address of the other end of our connection.
 *
 * Revision 1.3  2002/07/09 22:53:32  dick
 * Add showtime to xpprintf()
 *
 * Revision 1.2  2002/06/26 04:44:08  dick
 * use Network.h, not network.h
 *
 * Revision 1.1  2002/06/25 04:41:03  dick
 * An http server.
 *
 */


#ifndef	_HTTP_H_
#define	_HTTP_H_

#include "Network.h"

enum AUTH {
	authFail = FALSE,
	authReadOnly,
	authReadWrite,
	authAdmin,
	authOk = authAdmin,
	authPublic = authFail
};

#define	HTTP_OK				200
#define	HTTP_TEMPERROR		401
#define	HTTP_EXTENDEDERROR	402		// check the Error: header for info

#ifdef	_WINDOWS
#define	BINARYFILEFLAGS	(O_RDONLY|O_BINARY)
#else
#define	BINARYFILEFLAGS	(O_RDONLY)
#endif

class Http : public Network {
  public:
	Http();
	virtual	~Http();
	void	Print404(PCSTR file);
	void	Print404(PCSTR error, PCSTR longerror);
	void	Print404(PCSTR error, PCSTR longerror, ObjList& headers);
	void	PrintError(int e, PCSTR error, PCSTR longerror);
	void	PrintExtendedError(int e, int exerror, PCSTR error, PCSTR longerror);
	void	Print200html();
	void	Print200text();
	void	Print200file();
	void	PrintHead();
	void	PrintBodyChoo();
	void	PrintTail();
	void	PrintHiddenField(PCSTR key, PCSTR value);
	char*	ShowTime(time_t then);
	char*	ShowTimeRelative(time_t then);

	void	ProcessReceive();
	void	ParseCommand();
	void	ParseGetOpts(ObjList& args, PCSTR s);

	virtual void	ProcessGET();
	void	ProcessPOST();
	void	ProcessHEAD();
	void	ProcessGETgraphic(PCSTR file);
	void	ProcessGEThtml(PCSTR file);
	void	LogHeaders(int level);

	void	ProcessFile(PCSTR file);

	virtual void DoCommand() = 0;
	virtual void PrintServer() = 0;	// "Server: BSAC? (Version) ?"
	virtual	void TokenSubst(String s);


	AUTH	AuthenticateUser(AUTH level);
	AUTH	AuthenticateFail();
	bool	AuthOK(AUTH auth);

//	bsuser*	FindUser();
	ObjList	stringsin;				// http headers
	enum { GET, POST, HEAD } httpCommand;
	String	filename;
	String	command;
	ObjList	args;					// args after the command/filename like (blah)?arg=1&x=2
	String	connectHost;			// who's talkin to us
	IN_ADDR	connectAddr;			// ip address of who's talkin to us


//	bsuser* GetUser() { return u; };
//	void  SetUser(bsuser* us) { u = us; };

	time_t now;						// when this connection started

//	bsuser*	u;						// The user on this http
  private:
	bool		modify;				// are we modifiying a config?
    char		showtimebuf[80];
	bool		headersOuted;		// Did we output the headers after a GET?

};

class HttpListen : public Network
{
  public:
	HttpListen();
	~HttpListen();
	SOCKET	listensock;
	SOCKET	connectsock[SOMAXCONN];
	SOCKADDR_IN s_in;
	time_t	retryAt;				// retry delay
	Http*	myhttps[SOMAXCONN];
	enum nstate {
		init,
		ready,
		error
	};
	nstate	state;
	bool	InitListen();
	int		port;			// port to listen to
};


extern	void	HttpAccept(SOCKET s);
extern	void	HttpRead(SOCKET s);
extern	void	HttpClose(SOCKET s);

extern	Http*	FindHttpBySock(SOCKET s);
extern	Http*	FindHttp();
extern	HttpListen*	FindHttpListener();

extern	String	Encode64(const char* in);
extern	String	Decode64(const char* in);

#endif		/* _HTTP_H_ */

