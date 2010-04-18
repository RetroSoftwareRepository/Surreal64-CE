#include "Network.h"
//Basic Network / HTTP(1.1) support added by freakdave
/*
What's left:

- Add support for binary files (PRIO: high)
- Check why XNetDnsLookup is not working (PRIO: high)

- Implement a simple webserver (PRIO: low)
	-> Edit settings and launch roms with browser from pc
	-> Users will be able to access their *sites*(IP/DNS), browse their roms directory
	   and download/exchange their roms

How this code is meant to be working:
- Add Update option in the menu
- On entering the update option the code should
- Call UpdateIni(Version)
- Surreal will then try to download and replace the requested file
- and clean up afterwards...

*/

/* Server */
#define DEBUG

#ifndef DEBUG
#define Server "65.98.40.42"
#define DNS "freakdave.xbox-scene.com"
#else
#define Server "192.168.0.26"
#define DNS "localhost"
#endif



/* Variables */
FILE *fp;
INT err; //Error
SOCKET s_socket; // The main socket
INT done = 1; // Conditional variable
char buffer[8192]; // buffer
char real_host[256]; // host, e.g. www.google.com
char request[4096]; // request
char headerSend[2048]; // request header

int size = 1;
char line[2048];
char templine[2048];
char result[1024];
char *mem;
char cblock[1];

MemBuffer headersBuffer;

//Tells us whether or not an Ethernet cable is connected to the XBOX 
BOOL IsXboxConnected()
{
	DWORD dwStatus = XNetGetEthernetLinkStatus();
	if(dwStatus != 0){
	   return TRUE;
   }
   else
   {
	   return FALSE;
   }
}

void MemBufferCreate(MemBuffer *b)
{
    b->size = MEM_BUFFER_SIZE;
    b->buffer =(unsigned	char *) malloc( b->size );
    b->position = b->buffer;
}

void MemBufferGrow(MemBuffer *b)
{
    size_t sz;
    sz = b->position - b->buffer;
    b->size = b->size *2;
    b->buffer =(unsigned	char *) realloc(b->buffer,b->size);
    b->position = b->buffer + sz; // readjust current position
}

void MemBufferAddByte(MemBuffer *b,unsigned char byt)
{
    if( (size_t)(b->position-b->buffer) >= b->size )
        MemBufferGrow(b);

    *(b->position++) = byt;
}

//Function that receives a http header returned from the server
void ReceiveHeader(void){
MemBufferCreate(&headersBuffer );
    int chars = 0;
    done = FALSE;

    while(!done)
    {
        err = recv(s_socket,buffer,1,0);
        if(err<0)
            done=TRUE;

        switch(*buffer)
        {
            case '\r':
                break;
            case '\n':
				if(chars==0){done = TRUE;}
                chars=0;
                break;
            default:
                chars++;
                break;
        }

        MemBufferAddByte(&headersBuffer,*buffer);
    }

	//Write out the header into a file
	fp=fopen("T:\\header.txt", "w+");
	fprintf(fp, (char*) headersBuffer.buffer);
	fclose(fp);

	done=FALSE;
	//Open the header again and retrieve the file size of the
	//requested file..
	fp=fopen("T:\\header.txt", "r");
	while(!done){
	fgets(line,1024,fp);
	strncpy(templine,line,16);
		if(strncmp(templine,"Content-Length: ",16) == 0){
		OutputDebugString(line);
		OutputDebugString(templine);
		sprintf(result,line + 16);
		OutputDebugString(result);
		done = TRUE;
		}
	}
	fclose(fp);
	DeleteFile("T:\\header.txt");

	//Store the file size in an INT
	size = atoi(result);
}

//Wrapper for gethostbyname() -> not implemented in Winsock for XDK
struct hostent* _cdecl gethostbyname(const char* _name)
{
	HostEnt *server = (HostEnt *)malloc( sizeof( HostEnt ) );
	struct hostent *host = (struct hostent *)malloc( sizeof( struct hostent ) );
	unsigned long addr = inet_addr( _name );

	WSAEVENT hEvent = WSACreateEvent();
	XNDNS* pDns = NULL;
	if( addr != INADDR_NONE )
	{
		host->h_addr_list[0] = (char *)malloc( 4 );
		memcpy( host->h_addr_list[0], &addr, 4 );
	}
	else
	{
		INT err = XNetDnsLookup(_name, hEvent, &pDns);
		WaitForSingleObject(hEvent, INFINITE);
		if( !pDns || pDns->iStatus )
		{
			if( pDns )
				XNetDnsRelease(pDns);
			free( host );
			return NULL;
		}

		host->h_addr_list[0] = (char *)malloc( 4 );
		memcpy( host->h_addr_list[0], &(pDns->aina[0].s_addr), 4 );

		XNetDnsRelease(pDns);
	}

	host->h_name = (char *)malloc( strlen( _name ) );
	strcpy( host->h_name, _name );
	host->h_aliases = 0;
	host->h_addrtype = AF_INET;
	host->h_length = 4;
	host->h_addr_list[1] = NULL;

	return host;
}


// Returns host adress (borrowed from LinksBoks)
DWORD GetHostAddress(const char *host)
{
    struct hostent *phe;
    char *p;

    phe = gethostbyname( host );
            
    if(phe==NULL)
        return 0;
    
    p = *phe->h_addr_list;
    return *((DWORD*)p);
}

// Sends a string through a socket to the connected server
BOOL SendString(SOCKET sock,LPCSTR str)
{
    err = send(sock,str,strlen(str),0);
	if(err == SOCKET_ERROR)
	{
		WSACleanup();//Terminate Winsock
		XNetCleanup();//Terminate Network
		OutputDebugString("Server did not respond\n");
		return FALSE;
	}
return TRUE;
}

//Initialize our Network
//TODO: Mess with Buffer sizes
BOOL InitNetwork(void){
	OutputDebugString("Initializing Network\n");
	XNetStartupParams xnsp;
	memset(&xnsp, 0, sizeof(xnsp));
	xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
	xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	xnsp.cfgSockDefaultRecvBufsizeInK = 32;//Change Receive buffer to 32K
	err = XNetStartup(&xnsp);

	if(err != 0){
		XNetCleanup();//Terminate Network
		OutputDebugString("Could not initialize Network\n");
		return FALSE;
	}
return TRUE;
}

//Init Winsock
BOOL InitWinsock(void){
OutputDebugString("Initializing Winsock\n");
WSADATA wsaData;
err = WSAStartup( MAKEWORD(2,2), &wsaData );

if(err != 0){
	WSACleanup();//Terminate Winsock
	XNetCleanup();//Terminate Network
	OutputDebugString("Could not initialize Winsock\n");
	return FALSE;
}
	return TRUE;
}

//Create a socket that is bound to a specific service provider
BOOL CreateSocket(void){
OutputDebugString("Creating Socket\n");
s_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

if (s_socket == INVALID_SOCKET) {
	WSACleanup();//Terminate Winsock
	XNetCleanup();//Terminate Network
	OutputDebugString("Could not create Socket\n");
	return FALSE;
}
return TRUE;
}


//Clean up Network
BOOL TerminateNetwork(void){
OutputDebugString("Cleaning up Network\n");
XNetCleanup();
return TRUE;
}

//Clean up Winsock
BOOL TerminateWinsock(void){
OutputDebugString("Cleaning up Winsock\n");
WSACleanup();
return TRUE;
}

//Downloads a file. Usage: DownloadFile("www.myhost.com|123.345.567.789","/mydir/getme.html","D:\\saveme.html");
BOOL DownloadFile(const char *host, const char *path_and_filename, const char *save_as){
//Use a SOCKADDR_IN struct to fill in address information
sockaddr_in server_info;
server_info.sin_family = AF_INET;
server_info.sin_port = htons(80);
server_info.sin_addr.s_addr = GetHostAddress(host);

//Connect to the server
OutputDebugString("Connecting to server\n");
err = connect(s_socket,(sockaddr*) &server_info,sizeof(struct sockaddr));

if(err == SOCKET_ERROR)
{
	WSACleanup();//Terminate Winsock
	XNetCleanup();//Terminate Network
	OutputDebugString("Could not connect to server\n");
	return FALSE;
}

//Send the full request to the server
OutputDebugString("Requesting file from server\n");

sprintf(request,path_and_filename);
sprintf(real_host,host);

//Checks if request is a valid request
	if( !*request ){
       lstrcpyn(request,"/",sizeof(request));
	}

//Construct Request
	//SendString(s_socket,"GET ");
		strcpy(headerSend, "GET ");

	//SendString(s_socket,request);
		strcat(headerSend,request);

	//SendString(s_socket," HTTP/1.1\r\n");
		strcat(headerSend, " HTTP/1.1\r\n");
//Request constructed

//Construct Hostname
	//SendString(s_socket,"Host: ");
		strcat(headerSend, "Host: ");

	//SendString(s_socket,real_host);
		//strcat(headerSend,real_host); //Not working ->XNetDnsLookup
		strcat(headerSend,DNS);//<-65.98.40.42
		

	//SendString(s_socket,"\r\n");
		strcat(headerSend,"\r\n");
//Hostname constructed


	//SendString(s_socket,"From: Surreal64XXX@xbox-scene.com\r\n");
		strcat(headerSend, "From: Surreal64XXX@xbox-scene.com\r\n");

    //SendString(s_socket,"Accept: text/html, text/plain, image/gif, image/x-xbitmap,"
       // " image/jpeg, image/pjpeg, application/vnd.ms-excel,"
       // " application/msword, application/vnd.ms-powerpoint, application/x-rar-compressed,"
       // " */*\r\n");
		strcat(headerSend, "Accept: text/html, text/plain, image/gif, image/x-xbitmap,"
        " image/jpeg, image/pjpeg, application/vnd.ms-excel,"
        " application/msword, application/vnd.ms-powerpoint, application/x-rar-compressed,"
        " */*\r\n");

    //SendString(s_socket,"Accept-Language: en-us\r\n");
		strcat(headerSend, "Accept-Language: en-us\r\n");

    //SendString(s_socket,"Accept-Encoding: gzip, deflate\r\n");
		strcat(headerSend, "Accept-Encoding: gzip, deflate\r\n");

    //SendString(s_socket,"User-Agent: Mozilla/4.0\r\n");
		strcat(headerSend, "User-Agent: Mozilla/4.0\r\n");

	//send blank line and tell the server that we're done
    //SendString(s_socket,"\r\n");								
		strcat(headerSend, "\r\n");

	//Send our full request packet to the server
	SendString(s_socket, headerSend);

	if(err == SOCKET_ERROR)
	{
		WSACleanup();//Terminate Winsock
		XNetCleanup();//Terminate Network
		OutputDebugString("Server did not respond\n");
		return FALSE;
	}

	//We made a http request, so we're now expecting a http header from the server.
	//We're now going to receive the header and examine it in order to get the
	//exact file size of our requested file.
	OutputDebugString("Receiving HTTP header\n");
	ReceiveHeader();

	//Now that everything went fine we can safely delete our old file
	OutputDebugString("Deleting old file\n");
	DeleteFile(save_as);

	//Receive and write the requested file onto the HDD
	OutputDebugString("Receiving requested file\n");

	//Create a buffer that has the size of our incoming file
	char *puffer = new char[size];

	//Fill our buffer
	done = FALSE;
	while (done < size && IsXboxConnected()){
		done += recv(s_socket, puffer, size, 0);
	}


	//WARNING: This ugly hack doesn't work for BINARY FILES !
	//We check the buffer(every single byte) and filter out all 0x0D (CRLF ??)
	fp=fopen(save_as, "w+");
	for (int i = 0; i < size; i++){
		mem = cblock;
		*mem = puffer[i];

		if(*mem != 0x0D || *mem != 0x0A || *mem != 0x00){
			fputc(*mem,fp);
		}
	}
	fclose(fp);

return TRUE;
}


//Main UpdateRiceIni function.
BOOL UpdateIni(int Version)
{
InitNetwork();
InitWinsock();

CreateSocket();

switch (Version)
{
	case 510:
		DownloadFile(Server,"/ini/RiceDaedalus5.1.0.ini","D:\\RiceDaedalus5.1.0.ini");
		break;
	case 531:
		DownloadFile(Server,"/ini/RiceDaedalus5.3.1.ini","D:\\RiceDaedalus5.3.1.ini");
		break;
	case 560:
		DownloadFile(Server,"/ini/RiceVideo5.6.0.ini","D:\\RiceVideo5.6.0.ini");
		break;
}

//At this point we're basically done
//Clean up Winsock
TerminateWinsock();
//Clean up Network
TerminateNetwork();

return TRUE;
}