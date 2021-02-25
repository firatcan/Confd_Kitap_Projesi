/*********************************************************************
 * ConfD Subscriber intro example
 * Implements a DHCP server adapter
 *
 * (C) 2005-2007 Tail-f Systems
 * Permission to use this code as a starting point hereby granted
 *
 * See the README file for more information
 ********************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>

#include <confd_lib.h>
#include <confd_cdb.h>

/* include generated file */
#include "kategori.h"
#define DAEMON_NAME "kategori"
#define SUBPATH "/tarih/"
#define LISTPATH SUBPATH "/kitap/"

/********************************************************************/
//CDB config
int confd_configuration(const char *CONFD_ADDRESS)
{ 
    //CONFD_ADDRESS = 127.0.0.1
    struct sockaddr_in addr;
    //struct confd_cs_node *csnode;
    //int subsock, ret, nvals;
    int subsock, ret;

    // Configure the Socket
    addr.sin_addr.s_addr = inet_addr(CONFD_ADDRESS);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(CONFD_PORT);

    confd_init(DAEMON_NAME, stderr, CONFD_TRACE);

    // Loading schemas from confd
    if (confd_load_schemas((struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != CONFD_OK)
    {
        confd_fatal("Failed to load schemas from confd\n");
    }

    // Open the socket
    // SOCK_STREAM = TCP
    if ((subsock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        confd_fatal("Failed to open socket\n");
    }

    // Connect the ConfD
    if (cdb_connect(subsock, CDB_DATA_SOCKET, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        confd_fatal("Failed to cdb_connect() to confd \n");
    }

    // Start a new session with Operational mode
    // operational mode = open setting mode
    if ((ret = cdb_start_session(subsock, CDB_OPERATIONAL)) != CONFD_OK)
    {
        return ret;
    }

    // 3 = number of leaf nodes in kitap.yang file
    /**
     * string kitapAdi;
     * uint32 kitapSayfaSayisi; 
     * string kitapYayinEvi;
     */
    confd_value_t kategoriConfiguration[3];
    int counter = 0;

    //CONFIGURE DATA
    CONFD_SET_STR(&kategoriConfiguration[counter],"NUTUK"); //kitapAdi
    counter++;
    CONFD_SET_UINT32(&kategoriConfiguration[counter],639); //kitapSayfaSayisi
    counter++;
    CONFD_SET_STR(&kategoriConfiguration[counter],"ALFA_YAYINLARI"); //kitapYayinEvi
    counter++;

    //set data in yang files
    ret = cdb_set_object(subsock, kategoriConfiguration, counter, LISTPATH,DAEMON_NAME);

    if (ret == CONFD_ERR && confd_errno == CONFD_ERR_BADPATH)
    {
        /* assume interface doesn't exist in config */
        ret = CONFD_OK;
    }

    cdb_end_session(subsock);

    return cdb_close(subsock);
}
/********************************************************************/


/********************************************************************/
int main(int argc, char **argv)
{
    const char *CONFD_ADDRESS = "127.0.0.1";
    confd_configuration(CONFD_ADDRESS);

    return 0;
}
/********************************************************************/
