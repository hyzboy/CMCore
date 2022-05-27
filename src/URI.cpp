#include<hgl/URI.h>

namespace hgl
{
    namespace
    {
        // https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
        
        struct SchemePort
        {
            uint16 port;
            char scheme[16];
        };

        const SchemePort SchemePortList[]=
        {
            {7,     "echo"},
            {21,    "ftp"},
            {22,    "ssh"},
            {23,    "telnet"},
            {25,    "smtp"},
            {53,    "dns"},
            {69,    "tftp"},
            {80,    "http"},
            {80,    "ws"},
            {110,   "pop3"},
            {119,   "nntp"},
            {123,   "ntp"},
            {143,   "imap"},
            {161,   "snmp"},
            {162,   "snmptrap"},
            {194,   "irc"},
            {199,   "smux"},
            {389,   "ldap"},
            {443,   "https"},
            {443,   "wss"},
            {445,   "smb"},
            {465,   "smtps"},
            {548,   "afp"},     //Apple File Services
            {554,   "rtsp"},
            {636,   "ldaps"},
            {853,   "dnss"},
            {873,   "rsync"},
            {993,   "imaps"},
            {5060,  "sip"},
            {5061,  "sips"},
            {5222,  "xmpp"}
        };
    }

    const uint16 GetSchemePort(const char *scheme)
    {
        for(const SchemePort &sp:SchemePortList)
            if(strcmp(scheme,sp.scheme)==0)
                return sp.port;

        return 0;
    }

    URI::URI(const OSString &fn)
    {
        origin_name=fn;

        scheme_type=SchemeType::Unknow;
    }
    
    URI::URI(const OSString &scheme,const OSString &path,const OSString &filename)
    {
        scheme_name=scheme;

        if(scheme=="file")
            scheme_type=SchemeType::LocalFile;
        else
        
    }

    const bool URI::isFile()
    {
        if(scheme_type==SchemeType::Unknow)
        {
            
        }

        if(scheme_type==SchemeType::LocalFile)
            return(true);

        return(false);
    }
}//namespace hgl
