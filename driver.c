//
// Created by Afik Levi on 26/04/2018.
//

#include "team.h"
#include "season.h"
#include "driver.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct driver {
    int id;
    char* name;
    Team team;
    int points;
    Season season;
};

Driver DriverCreate(DriverStatus* status, char* driver_name, int driverId){
    char *empty_name ="";
    if (driver_name == NULL || *driver_name == *empty_name){
        if (status!=NULL){
            *status=INVALID_DRIVER;
        }
        return NULL;
    }
    Driver new_driver = malloc(sizeof(*new_driver));
    if (new_driver == NULL)
    {
        printf("Allocation error\n");
        if (status != NULL) {
            *status = DRIVER_MEMORY_ERROR;
        }
        return NULL;
    }
    if (driverId<=0) {
        if (status != NULL) {
            *status = INVALID_DRIVER;
        }
        return NULL;
    }
    new_driver->name = malloc(sizeof(char)*(strlen(driver_name))+1);
    if (new_driver->name == NULL)
    {
        printf("Allocation error\n");
        return NULL;
    }
    strcpy(new_driver->name,driver_name);
    new_driver->id = driverId;
    if (status != NULL) {
        *status = DRIVER_STATUS_OK;
    }
    new_driver->team = NULL;
    new_driver->season=NULL;
    new_driver->points=0;
    return new_driver;
}

void   DriverDestroy(Driver driver) {
    if (driver != NULL) {
        free(driver->name);
        driver->season=NULL;
        driver->team=NULL;
        free(driver);
    }
}

const char* DriverGetName(Driver driver){
    if (!driver) return NULL;
    else
        return driver->name;
}

int DriverGetId(Driver driver){
    if (!driver) return 0;
    else return driver->id;
}

Team  DriverGetTeam(Driver driver){
    if (!driver) return NULL;
    else return driver->team;
}
void  DriverSetTeam(Driver driver, Team team){
    if ((driver==NULL)||(team==NULL)){
        return;
    }
    driver->team = team;
    TeamAddDriver(team, driver);
}

void  DriverSetSeason(Driver driver, Season season){
    if ((driver==NULL)||(season==NULL)){
        return;
    }
    driver->season=season;
    driver->points=0;
}

DriverStatus DriverAddRaceResult(Driver driver, int position){
    if (driver==NULL) return INVALID_DRIVER;
    if (position<=0) return INVALID_POSITION;
    if (!(driver->season)) return SEASON_NOT_ASSIGNED;
    int new_points = SeasonGetNumberOfDrivers(driver->season) - position;
    driver->points += new_points;
    return DRIVER_STATUS_OK;
}

int DriverGetPoints(Driver driver, DriverStatus* status){
    if (driver==NULL){
        if (status != NULL) {
            *status = INVALID_DRIVER;
        }
        return 0;
    }
    if (status != NULL) {
        *status = DRIVER_STATUS_OK;
    }
    return driver->points;
}

