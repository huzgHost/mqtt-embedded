#include "../src/MQTTLinux.h"
#include <stdio.h>
#include "pub.h"
#include "src/MQTTClient.h"

volatile int toStopPub = 0;


void usagePub()
{
    printf("MQTT stdout subscriber\n");
    printf("Usage: stdoutsub topicname <options>, where options are:\n");
    printf("  --host <hostname> (default is localhost)\n");
    printf("  --port <port> (default is 1883)\n");
    printf("  --qos <qos> (default is 2)\n");
    printf("  --delimiter <delim> (default is \\n)\n");
    printf("  --clientid <clientid> (default is hostname+timestamp)\n");
    printf("  --username none\n");
    printf("  --password none\n");
    printf("  --showtopics <on or off> (default is on if the topic has a wildcard, else off)\n");
    printf("  --m <payload> (puslish content)");
    exit(-1);
}


void cfinishPub(int sig)
{
    signal(SIGINT, NULL);
    toStopPub = 1;
}


struct opts_struct
{
    char* clientid;
    int nodelimiter;
    char* delimiter;
    enum QoS qos;
    char* username;
    char* password;
    char* host;
    int port;
    int showtopics;
    char *payload;
    int payloadLen;
} optsPub =
{
    (char*)"launch-publicer", 0, (char*)"\n", QOS2, NULL, NULL, (char*)"localhost", 1883, 0, NULL, 0
};

void getoptsPub(int argc, char** argv)
{
    int count = 2;

    while (count < argc)
    {
        if (strcmp(argv[count], "--qos") == 0)
        {
            if (++count < argc)
            {
                if (strcmp(argv[count], "0") == 0)
                    optsPub.qos = QOS0;
                else if (strcmp(argv[count], "1") == 0)
                    optsPub.qos = QOS1;
                else if (strcmp(argv[count], "2") == 0)
                    optsPub.qos = QOS2;
                else
                    usagePub();
            }
            else
                usagePub();
        }
        else if (strcmp(argv[count], "--host") == 0)
        {
            if (++count < argc)
                optsPub.host = argv[count];
            else
                usagePub();
        }
        else if (strcmp(argv[count], "--port") == 0)
        {
            if (++count < argc)
                optsPub.port = atoi(argv[count]);
            else
                usagePub();
        }
        else if (strcmp(argv[count], "--clientid") == 0)
        {
            if (++count < argc)
                optsPub.clientid = argv[count];
            else
                usagePub();
        }
        else if (strcmp(argv[count], "--username") == 0)
        {
            if (++count < argc)
                optsPub.username = argv[count];
            else
                usagePub();
        }
        else if (strcmp(argv[count], "--password") == 0)
        {
            if (++count < argc)
                optsPub.password = argv[count];
            else
                usagePub();
        }
        else if (strcmp(argv[count], "--delimiter") == 0)
        {
            if (++count < argc)
                optsPub.delimiter = argv[count];
            else
                optsPub.nodelimiter = 1;
        }
        else if (strcmp(argv[count], "--showtopics") == 0)
        {
            if (++count < argc)
            {
                if (strcmp(argv[count], "on") == 0)
                    optsPub.showtopics = 1;
                else if (strcmp(argv[count], "off") == 0)
                    optsPub.showtopics = 0;
                else
                    usagePub();
            }
            else
                usagePub();
        }
        else if(strcmp(argv[count], "--m") == 0) {
            if(++count < argc) {
                optsPub.payload = argv[count];
                optsPub.payloadLen = strlen(optsPub.payload);
            }
        }
        count++;
    }

}

int startPub(int argc, char *argv[]) {
    int rc = 0;
    unsigned char buf[100];
    unsigned char readbuf[100];

    if (argc < 2)
        usagePub();

    char* topic = argv[1];

    if (strchr(topic, '#') || strchr(topic, '+'))
        optsPub.showtopics = 1;
    if (optsPub.showtopics)
        printf("topic is %s\n", topic);

    getoptsPub(argc, argv);

    Network n;
    MQTTClient c;

    signal(SIGINT, cfinishPub);
    signal(SIGTERM, cfinishPub);

    NetworkInit(&n);
    NetworkConnect(&n, optsPub.host, optsPub.port);
    MQTTClientInit(&c, &n, 1000, buf, 100, readbuf, 100);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.willFlag = 0;
    data.MQTTVersion = 3;
    data.clientID.cstring = optsPub.clientid;
    data.username.cstring = optsPub.username;
    data.password.cstring = optsPub.password;

    data.keepAliveInterval = 10;
    data.cleansession = 1;
    printf("Connecting to %s %d\n", optsPub.host, optsPub.port);

    rc = MQTTConnect(&c, &data);
    printf("Connected %d\n", rc);

    printf("Publishing to %s\n", topic);
    MQTTMessage message = {
        0,0,0,0,optsPub.payload,optsPub.payloadLen
    };

    rc = MQTTPublish(&c, topic, &message);

    MQTTDisconnect(&c);
    printf("Published %d\n", rc);
}
