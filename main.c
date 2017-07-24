#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */



typedef struct SensorReading{

    int temperature;
    int lux;

}SensorReading;


int getTemperature(int *min, int *max);
void seedRandom();
int getRandomInt(int *min, int *max);
int getLux();
SensorReading *gen_reading(int *min_temp, int *max_temp, int *min_lux, int *max_lux);
void printSensorReading(SensorReading *readingToPrint);
//void sendMsg();
void sendMsg(int temperature, int lux, char* destination_ip_address, int destination_portNumber, char* sensor_id);
void error(const char *msg) { perror(msg); exit(0); }




/***********************************************************************
MAIN PROGRAM
*************************************************************************/

int main(int argc, char *argv[])
{

    if ( argc != 5 ) /* argc should be 2 for correct execution */
        {
        printf("Wrong number of arguments\n");
        exit(0);

    }



    /******************
    Seed the randomiser
    *******************/

    char* destination_ip_address  = argv[1];


    int *destination_portNumber = (int *)malloc(sizeof(int));
    *destination_portNumber = atoi(argv[2]);

    int *messages_to_send = (int *)malloc(sizeof(int));
    *messages_to_send = atoi(argv[3]);


    char* input_sensor_id  = argv[4];
    
    int *sensor_send_delay = (int *)malloc(sizeof(int));
    *sensor_send_delay = atoi(argv[5]);


    printf("\n%s",destination_ip_address);
    printf("\n%s",input_sensor_id);
    printf("\n%i",*destination_portNumber);
    printf("\n%i",*messages_to_send);
    printf("%lu\n", (unsigned long)time(NULL));



    printf("\n%s",argv[1]);
    printf("\n%s",argv[2]);
    printf("\n%s",argv[3]);



    seedRandom();

    printf("\nSensor Data Sim!\n");



    int *min_temp = (int *)malloc(sizeof(int));
    *min_temp = 15;

    int *max_temp = (int *)malloc(sizeof(int));
    *max_temp = 25;

    int *min_lux = (int *)malloc(sizeof(int));
    *min_lux = 100;

    int *max_lux = (int *)malloc(sizeof(int));
    *max_lux = 200;

/*
    SensorReading *read1 =  gen_reading(min_temp,max_temp,min_lux,max_lux);

    printf("The temperature is %i\n", read1->temperature);
    printf("The lux is %i\n", read1->lux);


    SensorReading *read2 =  gen_reading(min_temp,max_temp,min_lux,max_lux);

    printf("The temperature is %i\n", read2->temperature);
    printf("The lux is %i\n", read2->lux);

 */
    /************************************
    Set up Experiment
    **********************************/



    int x = 0;
    while(x< *messages_to_send){

        SensorReading *read_temp =  gen_reading(min_temp,max_temp,min_lux,max_lux);
        sendMsg(read_temp->temperature,read_temp->lux,destination_ip_address, *destination_portNumber, input_sensor_id);
        printSensorReading(read_temp);
        printf("\n*******Message ID: %i\n",x);
        x++;
        free(read_temp);
        sleep(*sensor_send_delay);


    }

    /**************************
    END the experiment
    **************************/





    free(min_temp);
    free(max_temp);

    free(min_lux);
    free(max_lux);

    free(messages_to_send);
    free(sensor_send_delay);
    free(destination_portNumber);

    return 0;

}


/****************************************************************************/


int getTemperature(int *min, int *max){

    return getRandomInt(min,max);
}



int getLux(int *min_lux, int *max_lux){

    return getRandomInt(min_lux,max_lux);
}


void seedRandom(){

    time_t t;
    /* Initializes random number generator */
    srand((unsigned) time(&t));
}

int getRandomInt(int *min, int *max){

    return *min + rand() / (RAND_MAX / (*max - *min + 1) + 1);

}



struct SensorReading *gen_reading(int *min_temp, int *max_temp, int *min_lux, int *max_lux)
{
    struct SensorReading *newReading = NULL;

    /* allocate a new reading */
    newReading = (struct SensorReading *)malloc(sizeof(struct SensorReading));

    /* check that we have a reading to fill in */
    if(newReading != NULL) {

       newReading->temperature = getTemperature(min_temp,max_temp);
       newReading->lux = getLux(min_lux,max_lux);

    }
    /* return the pointer to the new reading */
    return newReading;
}

void printSensorReading(SensorReading *readingToPrint){


    printf("The temperature is %i\n", readingToPrint->temperature);
    printf("The lux is %i\n", readingToPrint->lux);

}


void sendMsg(int temperature, int lux, char* destination_ip_address, int destination_portNumber,char* input_sensor_id){

  /* first what are we going to send and where are we going to send it? */
    int portno =  destination_portNumber;
    char *host =        destination_ip_address;
    char *message_fmt = "POST /readings-form/?temp=%i&lux=%i&id=%s&timestamp=%lu&daydate=%s HTTP/1.0\r\n\r\n";
    char *temp_sensor_id = input_sensor_id;
    unsigned long unix_timestamp = (unsigned long)time(NULL);


    //create day date data

 /*********************/
    time_t rawtime;
   struct tm *info;
   char daydate_buffer[80];

   time( &rawtime );

   info = localtime( &rawtime );

   strftime(daydate_buffer,80,"%Y%m%d", info);
   printf("Formatted date & time:%s\n", daydate_buffer );

/***************************************/


    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char message[1024],response[4096];


    /* fill in the parameters */
    sprintf(message,message_fmt,temperature,lux,temp_sensor_id,unix_timestamp,daydate_buffer);
    printf("Request:\n%s\n",message);

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL) error("ERROR, no such host");

    /* fill in the structure */
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    /* connect the socket */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    /* send the request */
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    /* receive the response */
    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = read(sockfd,response+received,total-received);
        if (bytes < 0)
            error("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received+=bytes;
    } while (received < total);

    if (received == total)
        error("ERROR storing complete response from socket");

    /* close the socket */
    close(sockfd);

    /* process response */
    printf("Response:\n%s\n",response);




}





