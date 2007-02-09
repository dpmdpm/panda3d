#ifndef __BUFFEREDREADER_GM_H__
#define __BUFFEREDREADER_GM_H__

#include "ringbuffer.h"
#include "datagram.h"

inline unsigned short GetUnsignedShort(char * in)
{
    return *((unsigned short *)in);
};


class Buffered_DatagramReader : protected RingBuffer
{
	inline bool GetMessageFromBuffer(Datagram &inmsg);
public:
	inline Buffered_DatagramReader(int in_size = 8192) ;
	inline void ReSet(void); 
    //
    // SOCK_TYPE is used to allow for 
    // abstract socket type to be used .. 
    // see socket_tcp and socket_ssl

    template < class SOCK_TYPE>
        inline int PumpMessageReader(Datagram &inmsg, SOCK_TYPE &sck)
    {
        if(GetMessageFromBuffer(inmsg) == true)
            return 1;
        int rp = ReadPump(sck); 
        if(rp == 0)
            return 0;

        if(rp < 1)
            return -1;
        if(GetMessageFromBuffer(inmsg) == true)
            return 1;
        return 0;
    }


    template < class SOCK_TYPE>
        inline int ReadPump(SOCK_TYPE &sck)
    {		
        int		answer = 0;
        size_t		readsize = BufferAvailabe();

        if(readsize < 1)
        {
            Compress();
            readsize = BufferAvailabe();
        }

        if(readsize > 0)
        {
            char * ff = GetBufferOpen();
            int gotbytes = sck.RecvData(ff,(int)readsize);
            if(gotbytes < 0)  // some error
            {
                //int er = GETERROR(); 
                if(!sck.ErrorIs_WouldBlocking(gotbytes) )
                {
                    answer = -3;  // hard error ?
                }
                else
                {
                    answer = 0; // try again nothing to read
                }
            }
            else if(gotbytes > 0) // ok got some lets process it
            {

                _EndPos	+=  gotbytes;
                answer = 1;
            }
            else   // 0 mean other end disconect arggggg
            {
                answer = -1;
            }
        }		
        else
        {
            answer = -2;
        }
        return answer;
    }
};

#include "buffered_datagramreader.i"

#endif //__BUFFEREDREADER_GM_H__

