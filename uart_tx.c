/**************************************************

file: demo_rx.c
purpose: simple demo that receives characters from
the serial port and print them on the screen,
exit the program by pressing Ctrl-C

compile with the command: gcc demo_rx.c rs232.c -Wall -Wextra -o2 -o test_rx

**************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232.h"
#include <ctype.h>
#include <time.h>
#include <pthread.h>

static void *scanf_t(char *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (1)
    {

        Sleep(1);
        scanf(" %[^\n]", arg);
        //gets(arg);
        //    usleep(100000);
    }
    printf("Close scanf thread!!");
    return(0);
}

int main()
{
    int stoptime = 1;
    int i, n,
        //      cport_nr=5,        /* /dev/ttyS0 (COM6 on windows) */
        bdrate = 115200; /* 9600 baud */

    DWORD t1, t2;
    int cport_nr;
    unsigned char buf[511];
    unsigned char txbuf[511];

    char mode[] = {'8', 'N', '1', 0},
                  str[511];

    pthread_t t;

    printf("Please assign comport:");
    scanf("%d", &cport_nr);
    cport_nr--;

    //  strcpy(str[0], "The quick brown fox jumped over the lazy grey dog.\n");

    //  strcpy(str[1], "Happy serial programming!.\n");

    if (RS232_OpenComport(cport_nr, bdrate, mode))
    {
        //   printf("Can not open comport\n");

        return (0);
    }
    else
    {
        printf("Open COM Port %d Success\n", cport_nr + 1);
    }

    pthread_create(&t, NULL, scanf_t, txbuf);

    printf("Please enter instruction:");

    while (1)
    {
        n = RS232_PollComport(cport_nr, buf, 511);

        if (n > 0)
        {
            buf[n] = 0; /* always put a "null" at the end of a string! */

            for (i = 0; i < n; i++)
            {
                if (buf[i] < 32) /* replace unreadable control-codes by dots */
                {
                    buf[i] = '.';
                }
            }

            printf("received %i bytes(include LF & CR): %s\n\n", n, (char *)buf);

            if (strncmp("EXIT", buf, 4) == 0)
            {
                RS232_CloseComport(cport_nr);
                printf("\nClose COM Port\n");
                goto mainend;
            }
            t1 = GetTickCount();
            stoptime = 0;
        }

        t2 = GetTickCount();
        if ((t2 - t1) > 500 && stoptime == 0)
        {
            printf("Please enter instruction:");
            stoptime = 1;
        }
        // scanf("%s", txbuf);
        size_t length = strlen(txbuf);


        if (length > 0)
        {
            for (i = 0; i < length; i++)
            {
                txbuf[i] = toupper(txbuf[i]);
            }


            if (strncmp("EXIT", txbuf, 4) == 0)
            {
                RS232_CloseComport(cport_nr);
                printf("\nClose COM Port\n");
                goto mainend;
            }
            strcpy(str, txbuf);
            strcat(str, "\n\r");

            RS232_cputs(cport_nr, str);
            printf("sent: %s", str);
            strcpy(txbuf, "");
            t1 = GetTickCount();
            stoptime = 0;
        }

#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000); /* sleep for 100 milliSeconds */
#endif
    }

mainend:
    pthread_cancel(t);
    printf("Cancel thread!!\n");
    printf("Press enter to exit!!\n");
    pthread_join(t, NULL);
    printf("Thread cancelled!!");


//    system("pause");
    return (0);
}
