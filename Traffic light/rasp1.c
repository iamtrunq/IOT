#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"
#include <time.h>
#include <wiringPi.h>
#include <stdint.h>

//define GPIO port
#define Green_1 0
#define Yellow_1 3
#define Red_1 2

#define Green_2 23
#define Yellow_2 24
#define Red_2 25

#define Green_3 27
#define Yellow_3 28
#define Red_3 29

unsigned char GPIO[9] = {0,3,2,23,24,25,27,28,29};
unsigned char Station_port_1[3] = {0,3,2};

typedef struct Station{
	int id;
	int time_set_g;
	int time_set_y;
	int time_set_r;
	int time_pre_g;
	int time_pre_y;
	int time_pre_r;
	int count;
}Station;

Station S1,S2,S3;

int ID;

#define ADDRESS     "ws://broker.emqx.io:8083"
#define CLIENTID    "publisher_demo"
// PUB ID
#define PUBTOPIC1   "rasptoserver"

//SUB ID
#define SUBTOPIC1   "servertorasp" 

void init_wiringPi(){
	wiringPiSetup();
	for(uint8_t i = 0;i < 9; i++){
		pinMode(GPIO[i],OUTPUT);
		digitalWrite(GPIO[i],LOW);
	}
}

void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    //printf("Message '%s' with delivery token %d delivered\n", payload, token);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
    //printf("Received message: %s\n", payload);

    sscanf(payload,"ID:%d\n",&ID);
	//printf("ID:%d\n",ID);
	
    if(ID==S1.id){
    	sscanf(payload,"GreenTime:%d\tYellowTime:%d\tRedTime:%d\n",&S1.time_set_g,&S1.time_set_y,&S1.time_set_r);
    	printf("ID:%d\tGreen:%d\tYellow:%d\tRed:%d\n",ID,S1.time_set_g,S1.time_set_y,S1.time_set_r);
    	if(S1.time_set_g != S1.time_pre_g | S1.time_set_y != S1.time_pre_y| S1.time_set_r != S1.time_pre_r){
			printf("Changed\n");
    		digitalWrite(Green_1,LOW);
			digitalWrite(Yellow_1,LOW);
			digitalWrite(Red_1,LOW);
			delay(500);

			S1.count=S1.time_set_g+S1.time_set_y+S1.time_set_r;
    		S1.time_pre_g=S1.time_set_g;
    		S1.time_pre_y=S1.time_set_y;
    		S1.time_pre_r=S1.time_set_r;
		}
	}
	
	if(ID==S2.id){
    	sscanf(payload,"GreenTime:%d\tYellowTime:%d\tRedTime:%d\n",&S2.time_set_g,&S2.time_set_y,&S2.time_set_r);
    	printf("ID:%d\tGreen:%d\tYellow:%d\tRed:%d\n",ID,S2.time_set_g,S2.time_set_y,S2.time_set_r);
    	if(S2.time_set_g != S2.time_pre_g | S2.time_set_y != S2.time_pre_y| S2.time_set_r != S2.time_pre_r){
			printf("Changed\n");
    		digitalWrite(Green_2,LOW);
			digitalWrite(Yellow_2,LOW);
			digitalWrite(Red_2,LOW);
			delay(500);

			S2.count=S2.time_set_g+S2.time_set_y+S2.time_set_r;
    		S2.time_pre_g=S2.time_set_g;
    		S2.time_pre_y=S2.time_set_y;
    		S2.time_pre_r=S2.time_set_r;
		}
	}

	if(ID==S3.id){
    	sscanf(payload,"GreenTime:%d\tYellowTime:%d\tRedTime:%d\n",&S3.time_set_g,&S3.time_set_y,&S3.time_set_r);
    	printf("ID:%d\tGreen:%d\tYellow:%d\tRed:%d\n",ID,S3.time_set_g,S3.time_set_y,S3.time_set_r);
    	if(S3.time_set_g != S3.time_pre_g | S3.time_set_y != S3.time_pre_y| S3.time_set_r != S3.time_pre_r){
			printf("Changed\n");
    		digitalWrite(Green_3,LOW);
			digitalWrite(Yellow_3,LOW);
			digitalWrite(Red_3,LOW);
			delay(500);

			S3.count=S3.time_set_g+S3.time_set_y+S3.time_set_r;
    		S3.time_pre_g=S3.time_set_g;
    		S3.time_pre_y=S3.time_set_y;
    		S3.time_pre_r=S3.time_set_r;
		}
	}
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char* argv[]) {
	//khai bao thu vien wiringPi va config
	init_wiringPi();
 
	//thiet lap thong so ban dau
	S1.id=1;
	S2.id=2;
	S3.id=1060;

	S1.time_pre_g=-1;
	S1.time_pre_y=-1;
	S1.time_pre_r=-1;

	S2.time_pre_g=-1;
	S2.time_pre_y=-1;
	S2.time_pre_r=-1;

	S3.time_pre_g=-1;
	S3.time_pre_y=-1;
	S3.time_pre_r=-1;


    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }

    //listen for operation
    MQTTClient_subscribe(client, SUBTOPIC1, 0);
	
	char msg [300];

    while(1) {
		// dieu khien tram den 1
		if(S1.count!=0 && S1.count > S1.time_set_r+S1.time_set_y ){
			digitalWrite(Green_1, HIGH);	
		}
		if(S1.count==S1.time_set_r+S1.time_set_y){
			digitalWrite(Green_1,LOW);
			digitalWrite(Yellow_1,HIGH);
		}
		if(S1.count==S1.time_set_r){
			digitalWrite(Yellow_1,LOW);
			digitalWrite(Red_1,HIGH);
		}

		S1.count--;
		//printf("count_1: %d\n",S1.count);

		if(S1.count<=0){
			digitalWrite(Red_1,LOW);
			digitalWrite(Green_1, HIGH);
			S1.count=S1.time_set_g+S1.time_set_y+S1.time_set_r;
		}

		//dieu khien tram den 2
		if(S2.count!=0 && S2.count > S2.time_set_r+S2.time_set_y){
			digitalWrite(Red_2,LOW);
			digitalWrite(Green_2,HIGH);	
		}
		if(S2.count==S2.time_set_r+S2.time_set_y){
			digitalWrite(Green_2,LOW);
			digitalWrite(Yellow_2,HIGH);
		}
		if(S2.count==S2.time_set_r){
			digitalWrite(Yellow_2,LOW);
			digitalWrite(Red_2,HIGH);
		}

		S2.count--;
		//printf("count_2: %d\n",S2.count);

		if(S2.count<=0){
			digitalWrite(Red_2,LOW);
			digitalWrite(Green_2, HIGH);
			S2.count=S2.time_set_g+S2.time_set_y+S2.time_set_r;
		}

		//tram dieu khien 3
		if(S3.count!=0 && S3.count > S3.time_set_r+S3.time_set_y){
			digitalWrite(Red_3,LOW);
			digitalWrite(Green_3,HIGH);	
		}
		if(S3.count==S3.time_set_r+ S3.time_set_y){
			digitalWrite(Green_3,LOW);
			digitalWrite(Yellow_3,HIGH);
		}
		if(S3.count==S3.time_set_r){
			digitalWrite(Yellow_3,LOW);
			digitalWrite(Red_3,HIGH);
		}

		S3.count--;

		if(S3.count<=0){
			digitalWrite(Red_3,LOW);
			digitalWrite(Green_3, HIGH);
			S3.count=S3.time_set_g+S3.time_set_y+S3.time_set_r;
		}
		
		//Gui Data
        sprintf(msg,"count_1:%d,count_2:%d,count_3:%d\n",S1.count,S2.count,S3.count);
        publish(client,PUBTOPIC1,msg);
        sleep(1);
    }
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}
