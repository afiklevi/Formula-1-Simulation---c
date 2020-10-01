
// Created by Gilad Nir 0545251202 on 26/04/2018.

#include "team.h"
#include "season.h"
#include "driver.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "driver.h"

struct team{
    char* name;
    Driver first_driver;        //pointers
    Driver second_driver;
};

Team TeamCreate(TeamStatus* status, char* name){
    char *empty_name ="";
    if (name == NULL || *name == *empty_name){
        if (status!=NULL){
            *status=TEAM_NULL_PTR;
        }
        return NULL;
    }
    Team team = malloc(sizeof(*team));
    if (team==NULL) {
        printf("Error allocating\n");
        if (status != NULL) {
            *status = TEAM_MEMORY_ERROR;
        }
        return NULL;
    }
    team->name = malloc((strlen(name)+1)* sizeof(char));
    if (team->name==NULL) {
        printf("Error allocating\n");
        if (status != NULL) {
            *status = TEAM_MEMORY_ERROR;
        }
        return NULL;
    }
    strcpy(team->name,name);
    team->first_driver=NULL;
    team->second_driver=NULL;
    if (status != NULL) {
        *status = TEAM_STATUS_OK;
    }
    return team;
}

void TeamDestroy(Team team){
    if (team!=NULL) {
        if (team->first_driver != NULL) DriverDestroy(team->first_driver);
        if (team->second_driver != NULL) DriverDestroy(team->second_driver);
        free(team->name);
        free(team);
    }
}
TeamStatus TeamAddDriver(Team team, Driver driver){
    if ((team==NULL)||(driver==NULL))
        return TEAM_NULL_PTR;
    if (team->first_driver!=NULL){
        if (team->second_driver!=NULL){
            printf("No room in team\n");
            return TEAM_FULL;
        }
        else{
            team->second_driver=driver;
        }
    }
    else{
        team->first_driver=driver;
    }
    return TEAM_STATUS_OK;
}


const char * TeamGetName(Team team){
    if (team==NULL)
        return NULL;
    return team->name;
}

Driver TeamGetDriver(Team team, DriverNumber driver_number){
    if (team==NULL)
        return NULL;
    if ((driver_number<FIRST_DRIVER)||(driver_number>SECOND_DRIVER))
        return NULL;
    if ((team->first_driver==NULL)&&(driver_number==FIRST_DRIVER))
        return NULL;
    if ((team->second_driver==NULL)&&(driver_number==SECOND_DRIVER))
        return NULL;
    if (driver_number==FIRST_DRIVER){
        return team->first_driver;
    }
    else{
        return team->second_driver;
    }
}

int TeamGetPoints(Team team, TeamStatus *status){
    if ((team==NULL)||(status==NULL)) {
        if (status != NULL) {
            *status = TEAM_NULL_PTR;
        }
        return 0;
    }
    int sum=0;
    DriverStatus* driver_status=NULL;
    sum+= DriverGetPoints(team->first_driver,driver_status);
    sum+= DriverGetPoints(team->second_driver,driver_status);
    if (status != NULL) {
        *status = TEAM_STATUS_OK;
    }
    return sum;
}

