#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"
#include <stdint.h>
#include <stdio.h>
#include <mariadb/mysql.h>

// SQL
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *server = "localhost";
	char *user = "duyrasp";
	char *password = "1212"; 
	char *database = "demo_iot";
// MQTT

#define ADDRESS     "ws://broker.emqx.io:8083"
#define CLIENTID    "subscriber_demo"

// SUB ID
#define SUB_TOPIC   "rasptoserver"

// PUB ID
#define PUBTOPIC1   "servertorasp" 

// khai bai bien 
int time_set_green_from_db,time_set_yellow_from_db,time_set_red_from_db;
int id_set_from_db;
int count_1,count_2,count_3;
int ID_LIGHT[3]={1,2,1060};


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
    sscanf(payload,"count_1:%d,count_2:%d,count_3:%d\n",&count_1,&count_2,&count_3);
    printf("Stattion_1:%d\tStation_2:%d\tStation_3:%d\n",count_1,count_2,count_3);
    fflush(stdout);
    // them du lieu vao database
    
    char sql[200];

	sprintf(sql,"update traffic_lights set time = %d where id = %d",count_1,ID_LIGHT[0]);
    mysql_query(conn,sql);

    //gui tap du lieu 2
    sprintf(sql,"update traffic_lights set time = %d where id = %d",count_2,ID_LIGHT[1]);
    mysql_query(conn,sql);

    // gui tap du lieu 3
    sprintf(sql,"update traffic_lights set time = %d where id = %d",count_3,ID_LIGHT[2]);
    mysql_query(conn,sql);
    // ket thuc qua trinh them du lieu
    
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
int main(int argc, char* argv[]) {
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
    MQTTClient_subscribe(client, SUB_TOPIC, 0);

	char messeger[200];
    char messeger_1[200];
    conn = mysql_init(NULL);
    mysql_real_connect(conn,server,user,password,database,0,NULL,0);
    while(1) {
        
        char querry_connect[200]="";
        for(uint8_t i = 0; i < 3;i++){

            sprintf(querry_connect,"select * from traffic_lights where id = %d",ID_LIGHT[i]);
            //printf("%s\n",querry_connect);

            mysql_query(conn,querry_connect);

            res = mysql_store_result(conn);

            while(row = mysql_fetch_row(res)){

                sscanf(row[0],"%d\n",&id_set_from_db);
                sscanf(row[1],"%d\n",&time_set_green_from_db);
                sscanf(row[2],"%d\n",&time_set_yellow_from_db);
                sscanf(row[3],"%d\n",&time_set_red_from_db);

            }
            // clear result and close the connection

            mysql_free_result(res);
            // gui du lieu cho rasp qua mqtt
            
            sleep(2);
            sprintf(messeger_1,"ID:%d\n",id_set_from_db);
            publish(client,PUBTOPIC1,messeger_1);
            sprintf(messeger,"GreenTime:%d\tYellowTime:%d\tRedTime:%d\n",time_set_green_from_db,time_set_yellow_from_db,time_set_red_from_db);
            publish(client,PUBTOPIC1,messeger);
         
        } 

    }
    mysql_close(conn);
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    //sleep(1);
    return rc;
}
