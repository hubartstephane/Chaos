#include <chaos/StandardHeaders.h>
#include <chaos/Application.h>


#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>



// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

// 
// The code misses to include the prototype for inet_ntoa().
// 
//     The compiler should have told you this.
// 
//     Do add:

//#include <arpa/inet.h>

void RunClient(char const * url, int port, std::string & request_string)
{
    printf("CLIENT\n");

    char port_str[20];
    sprintf_s(port_str, sizeof(port_str) / sizeof(port_str[0]), "%d", port);

    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result == 0)
    {
        struct addrinfo *addr = NULL;


        struct addrinfo hints;
        ZeroMemory( &hints, sizeof(hints) );
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        result = getaddrinfo(url, port_str, &hints, &addr);
        if (result == 0)
        {
            bool job_done = false;

            struct addrinfo * ptr = NULL;
            for (ptr = addr ; (ptr != NULL) && (!job_done) ; ptr = ptr->ai_next)
            {
                SOCKET s = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
                if (s != INVALID_SOCKET)
                {
                    result = connect( s, ptr->ai_addr, (int)ptr->ai_addrlen);
                    if (result == 0)
                    {


                        std::this_thread::sleep_for(std::chrono::seconds(10));


                         shutdown(s, SD_SEND);
                         job_done = true;
                    }

                    closesocket(s);
                }
            }

            freeaddrinfo(addr);
        }

        WSACleanup();
    }
}

void RunServer(char const * url, int port, std::string & output_string)
{
    printf("SERVER\n");

    char port_str[20];
    sprintf_s(port_str, sizeof(port_str) / sizeof(port_str[0]), "%d", port);

    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result == 0)
    {
        struct addrinfo *addr = NULL;


        struct addrinfo hints;
        ZeroMemory( &hints, sizeof(hints) );
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        result = getaddrinfo(url, port_str, &hints, &addr);
        if (result == 0)
        {
            bool job_done = false;

            struct addrinfo * ptr = NULL;
            for (ptr = addr ; (ptr != NULL) && (!job_done) ; ptr = ptr->ai_next)
            {
                SOCKET s = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);




                if (s != INVALID_SOCKET)
                {



                    result = bind( s, ptr->ai_addr, (int)ptr->ai_addrlen);
                    if (result != SOCKET_ERROR)
                    {
                        result = listen(s, SOMAXCONN);
                        if (result != SOCKET_ERROR)
                        {
                            SOCKET client_s = accept(s, NULL, NULL);
                            if (client_s != INVALID_SOCKET)
                            {
                          
                                struct timeval tv;
                                tv.tv_usec = 0;
                                tv.tv_sec  = 7;

                                //result = setsockopt(client_s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
                          
                                DWORD timeout = 3000;

                                result = setsockopt(client_s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));


                                printf("CLIENT connected\n");

                                char recvbuf[7];
                                int  recvbuflen = sizeof(recvbuf);
                                
                                int sum = 0;

                                int count = 0;
                                do 
                                {
                                    count = recv(client_s, recvbuf, recvbuflen, 0);

                                    if (count == 0) // connection with client lost
                                    {

                                    }
                                    else if (count < 0) // error
                                    {
                                        int err2 = 0;
                                        socklen_t err_len2 = 0; //sizeof(err2);

                                        int k = getsockopt(client_s, SOL_SOCKET, SO_ERROR, (char*)&err2, (socklen_t*)&err_len2 );

                                        k = k;

                                                                            ETIMEDOUT;
                                    }




                                    ;

                                    int err = WSAGetLastError();

                                    if (err != 0)
                                    {
                                        // WSAETIMEDOUT

                                        char * ss = NULL;
                                        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                                            NULL, err,
                                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                            (LPSTR)&ss, 0, NULL);
                                        printf("%s\n", ss);
                                        LocalFree(ss);
                                    }

  





                                    // count == 0 : client disconnected
                                    // count  < 0 : errorcode

                                    for (int i = 0 ; i < count ; ++i)
                                    {
                                        output_string += recvbuf[i];
                                        

                                  //      printf("%c", isprint(recvbuf[i])? recvbuf[i] : '.');
                                  //      if ((sum++) % 50 == 49)
                                  //          printf("\n");
                                    }

                                    if (count < recvbuflen)
                                        break;

                                } while (count > 1);
                                
                                closesocket(client_s);
                                job_done = true;
                            }
                        }
                    }
                    closesocket(s);
                }
            }
            freeaddrinfo(addr);
        }
        WSACleanup();
    }
}


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

#if 0
    if (argc >= 1)
    {
        char const * url  = "127.0.0.1";
        int          port = 80;

        std::string result;

        if (argc < 2 || atoi(argv[1]) <= 0)
        {
            RunServer(NULL, port, result);
            FILE * f = NULL;
            fopen_s(&f, "d://tmp_req.txt", "w");
            if (f != NULL)
            {
                fwrite(&result[0], 1, result.length(), f);

                fclose(f);
                f = NULL;
            }
        }
        else
            RunClient(NULL, port, result);
    }

#endif
  return 0;
}