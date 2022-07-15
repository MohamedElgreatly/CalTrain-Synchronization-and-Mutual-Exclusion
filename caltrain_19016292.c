#include <pthread.h>
#include "caltrain.h"

//here intialize our struct condition variables and counters
void
station_init(struct station *station)
{
	station->outerPassanger=0; // this counter for passanger out the train in station
	station->passanger_loaded=0;  // this counter to check that all passangers loaded in the train     
        station->Emptysets=0;
	pthread_mutex_init(&station->mutex, NULL);
	pthread_cond_init(&station->arrivalTrain,NULL);
	pthread_cond_init(&station->Trainleaving,NULL);
}


void
station_load_train(struct station *station, int count)
{
   pthread_mutex_lock(&station->mutex);
    /*
      this check if there is full train arrive or if there is no outer passanger in station
   */
   if(count==0  ||station->outerPassanger==0 ){
       pthread_mutex_unlock(&station->mutex);
       return;
    }
   station->Emptysets=count;
   pthread_cond_broadcast(&station->arrivalTrain);
   pthread_cond_wait(&station->Trainleaving,&station->mutex);
   pthread_mutex_unlock(&station->mutex);
}

void
station_wait_for_train(struct station *station)
{
	pthread_mutex_lock(&station->mutex);
	station->outerPassanger++;
       while(station->Emptysets==0){
	pthread_cond_wait(&station->arrivalTrain,&station->mutex); 
         }
	station->outerPassanger--;
	station->passanger_loaded++;//here one passanger added
        station->Emptysets--; //decrease empty places in train after full
	pthread_mutex_unlock(&station->mutex);

}

void
station_on_board(struct station *station)
{
	pthread_mutex_lock(&station->mutex);

	    station->passanger_loaded--;  //here i subtract the passanger how is seated in train 
	
	pthread_mutex_unlock(&station->mutex);

        if(station->passanger_loaded==0 &&(station->outerPassanger==0 || station->Emptysets==0) ) //if train is full or no passanger out
	{
	  pthread_cond_signal(&(station->Trainleaving));
	}
}

