//
//  GUI_MQTT.cpp
//  PES
//
//  Created by Jimmy on 4/20/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//
#include "SDL_gui.h"
#include "GUI_MQTT.h"
#include "MQTTAsync.h"

static GUI_MQTT *pMQTT = NULL;


MQTTAsync_message pubmsg = MQTTAsync_message_initializer;

std::string GUI_MQTT::ADDRESS;
std::string GUI_MQTT::CLIENTID;
std::string GUI_MQTT::IN_TOPIC;
int GUI_MQTT::QOS;

int GUI_MQTT::finished;
int GUI_MQTT::subscribed;


GUI_MQTT *GUI_MQTT::create( const char *broker_address, const char *client_id ) {
    if( !pMQTT )
        pMQTT = new GUI_MQTT( broker_address, client_id );
    return pMQTT;
}

std::string gen_random_string() {
    static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    
    char s[16];
    const int len = 10;
    
    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    s[len] = 0;
    
    return std::string( s );
}

GUI_MQTT::GUI_MQTT( const char * broker_address, const char * client_id ) {
    finished = 0;
    subscribed = 0;
    
    connectCallback = nullptr;
    
    ADDRESS = std::string( broker_address );
    if( client_id )
        CLIENTID = std::string( client_id );
    else
        CLIENTID = gen_random_string();
    
    QOS = 2;
    
    MQTTAsync_create(&client, ADDRESS.c_str(), CLIENTID.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    
    MQTTAsync_setCallbacks(client, NULL, connlost, msgarrvd, NULL);
    
    int rc;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    
    conn_opts.context = client;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        
    }
}

void GUI_MQTT::connlost(void *context, char *cause)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;
    
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
    
    printf("Reconnecting\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        GUI_Quit();
    }
}

int GUI_MQTT::msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    int i;
    char* payloadptr;
    char buffer[1024];
    
    //GUI_Log("Message arrived\n");
    //GUI_Log("     topic: %s\n", topicName);
    
    
    payloadptr = (char *)message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        buffer[i] = *payloadptr++;
    }
    buffer[i] = 0;
    //GUI_Log("   message: %s", buffer);
    
    if( pMQTT->messageCallback ) {
        pMQTT->messageCallback( pMQTT, topicName, buffer );
    }

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

void GUI_MQTT::onConnect(void* context, MQTTAsync_successData* response)
{
    MQTTAsync client = (MQTTAsync)context;

    if( pMQTT->connectCallback ) {
        pMQTT->connectCallback( pMQTT, context );
    }
    printf("Successful connection\n");
}

void GUI_MQTT::subscribe(void* context, const char *in_topic)
{
    MQTTAsync client = (MQTTAsync)context;
    
    IN_TOPIC = std::string( in_topic );

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    //MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int rc;
    
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", IN_TOPIC.c_str(), CLIENTID.c_str(), QOS);
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;
    
    if ((rc = MQTTAsync_subscribe(client, IN_TOPIC.c_str(), QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        exit(-1);
    }
}

void GUI_MQTT::onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    GUI_Log("Connect failed, rc %d\n", response ? response->code : 0);
    finished = 1;
}

void GUI_MQTT::onSubscribe(void* context, MQTTAsync_successData* response)
{
    GUI_Log("Subscribe succeeded\n");
    subscribed = 1;
}

void GUI_MQTT::onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    GUI_Log("Subscribe failed, rc %d\n", response ? response->code : 0);
    finished = 1;
}

GUI_MQTT::~GUI_MQTT() {
    
}
