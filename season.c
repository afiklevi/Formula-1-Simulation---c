//
// Created by Gilad Nir 0545251202 on 26/04/2018.
//
#include "team.h"
#include "season.h"
#include "driver.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

int linesCounter (char* paragraph);

char** stringDivider  (char* paragraph, int linesNumber);

int driversCounterAndSetup (Driver drivers[], char** phrases, int number_of_lines);

void teamSetup (Team teams[], char** phrases, int number_of_lines);

void SetDriversToTeam (Driver drivers[], Team teams[], char** phrases, int number_of_lines);

void SetDriversToSeason (Driver* drivers, Season season);

void fillNewestResultsInTeamArray(Season season);

void sortDrivers(Driver drivers[], int number_of_drivers, int last_points[]);

void mergeSortDrivers(Driver drivers[], int number_of_drivers,
                      Driver driver_helper[],int last_points[],
                      int last_points_helper[]);

void sortTeams(Season season,Team teams[], int number_of_teams);

void mergeSortTeams(Team teams[], int number_of_teams,Team sort_helper[],
                    int point_helper[], int last_results[],Season season);

void mergeTeams(Team team_array1[],Team team_array2[],
                int len1,int len2, Team team_helper[],
                int last_points_array1[], int last_points_array2[],
                int last_points_helper[],Season season);

int newRaceResult(Season season, Team team,int index);

int findIndexOfDriverWithThisId(Season season,int id);

int getBestDriverPoints(Season season,Team team);

struct season{
    int year;
    int number_of_teams;
    int number_of_drivers;
    Team* teams;
    Driver* drivers;
    int* drivers_last_race_result;
    int* teams_last_race_result;
};

int linesCounter (char* paragraph) {
    int count=1, index=0;
    while (paragraph[index]){
        if (paragraph[index]=='\n')
            count++;
        index++;
    }
    return count;
}

char** stringDivider  (char* paragraph, int linesNumber){
    int index = 1;
    char** phrases = malloc(sizeof(char*)*linesNumber);
    char* temp;
    temp = strtok(paragraph,"\n");
    phrases[0] = temp;
    while (temp != NULL)
    {
        temp = strtok(NULL,"\n");
        phrases[index] = temp;
        index++;
    }
    return phrases;
}

int driversCounterAndSetup (Driver drivers[], char** phrases, int number_of_lines){
    int number_of_drivers=0;
    for (int i=3; i<number_of_lines; i+=3){
        int cmp = strcmp(phrases[i-1],"None");
        if (cmp){
            number_of_drivers++;
            DriverStatus status = DRIVER_STATUS_OK;
            drivers[number_of_drivers-1]=DriverCreate(&status, phrases[i-1], number_of_drivers);
        }
        int cmp2 = strcmp(phrases[i],"None");
        if (cmp2){
            number_of_drivers++;
            DriverStatus status = DRIVER_STATUS_OK;
            drivers[number_of_drivers-1]=DriverCreate(&status, phrases[i], number_of_drivers);
        }
    }
    return number_of_drivers;
}

void teamSetup (Team teams[], char** phrases, int number_of_lines){
    int index=0;
    TeamStatus status= TEAM_STATUS_OK;
    for (int i=1; i<number_of_lines-1; i+=3){
        teams[index]=TeamCreate(&status, phrases[i]);
        index++;
    }
}

void SetDriversToTeam (Driver drivers[], Team teams[], char** phrases, int number_of_lines){
    int drivers_index=0;
    int teams_index=0;
    for (int i=2; i<number_of_lines; i+=3){
        int cmp = strcmp(phrases[i],"None");
        if (cmp){
            TeamAddDriver(teams[teams_index],drivers[drivers_index]);
            drivers_index++;
        }
        int cmp2 = strcmp(phrases[i+1],"None");
        if (cmp2){
            TeamAddDriver(teams[teams_index],drivers[drivers_index]);
            drivers_index++;
        }
        teams_index++;
    }
}

Season SeasonCreate(SeasonStatus* status,const char* season_info){
    Season new_season = malloc(sizeof(*new_season));
    if (new_season == NULL)
    {
        printf("Allocation error\n");
        if (status != NULL) {
            *status = SEASON_MEMORY_ERROR;
        }
        return NULL;
    }
    char* copied_string = malloc(sizeof(char)*(strlen(season_info)+1));
    if (copied_string == NULL)
    {
        printf("Memory allocation error\n");
        return NULL;
    }
    strcpy(copied_string, season_info);
    int number_of_lines=linesCounter(copied_string);
    Team* teams = malloc(sizeof(Team)*number_of_lines);
    new_season->teams=teams;
    Driver* drivers = malloc(sizeof(Driver)*number_of_lines);
    new_season->drivers=drivers;
    char** phrases=stringDivider(copied_string,number_of_lines);
    new_season->year=atoi(phrases[0]);
    new_season->number_of_teams=(number_of_lines-1)/3;
    new_season->number_of_drivers=driversCounterAndSetup(new_season->drivers,phrases,
                                                         number_of_lines);
    teamSetup(new_season->teams,phrases, number_of_lines);
    int* drivers_last_race_result = malloc(sizeof(int)*new_season->number_of_drivers);
    new_season->drivers_last_race_result = drivers_last_race_result;
    int* teams_last_race_result = malloc(sizeof(int)*new_season->number_of_teams);
    new_season->teams_last_race_result = teams_last_race_result;
    SetDriversToTeam (new_season->drivers, new_season->teams, phrases, number_of_lines);
    if (status != NULL) {
        *status = SEASON_OK;
    }
    SetDriversToSeason (new_season->drivers, new_season);
    free(copied_string);
    free(phrases);
    return new_season;
}

void SetDriversToSeason (Driver* drivers, Season season){
    for(int i=0; i<season->number_of_drivers;i++){
        DriverSetSeason(drivers[i],season);
    }
}

void SeasonDestroy(Season season){
    if (season==NULL){
        return;
    }
    for (int i= 0; i<(season->number_of_teams);i++){
        TeamDestroy(season->teams[i]);
    }
    free(season->drivers_last_race_result);
    free(season->teams_last_race_result);
    free(season->teams);
    free(season->drivers);
    free(season);
}

int driversCounter (char** phrases, int number_of_lines){
    int number_of_drivers=0;
    for (int i=2; i<number_of_lines; i+=3){
        int cmp = strcmp(phrases[i],"None");
        if (cmp)
            number_of_drivers++;
        int cmp2 = strcmp(phrases[i+1], "None");
        if (cmp2)
            number_of_drivers++;
    }
    return number_of_drivers;
}

Driver SeasonGetDriverByPosition(Season season, int position,
                                 SeasonStatus* status){
    if (position<1){
        if (status!=NULL){
            *status=BAD_SEASON_INFO;
        }
        return NULL;
    }
    if (season == NULL){
        if (status!=NULL){
            *status=SEASON_NULL_PTR;
        }
        return NULL;
    }
    sortDrivers(season->drivers, season->number_of_drivers,
                season->drivers_last_race_result);
    if (status!=NULL){
        *status=SEASON_OK;
    }
    return season->drivers[position-1];
}

void sortDrivers(Driver drivers[], int number_of_drivers, int last_points[]){
    Driver *driver_helper = malloc(sizeof(Driver)*number_of_drivers);
    if (driver_helper==NULL){
        printf("Allocation error\n");
        return;
    }
    int *last_points_helper = malloc(sizeof(int)*number_of_drivers);
    if (last_points_helper==NULL){
        printf("Allocation error\n");
        return;
    }
    mergeSortDrivers(drivers, number_of_drivers, driver_helper,last_points
            ,last_points_helper);
    free(driver_helper);
    free(last_points_helper);
}

void mergeDrivers(Driver driver_array1[],Driver driver_array2[],
                  int len1,int len2, Driver driver_helper[],
                  int last_points_array1[], int last_points_array2[],
                  int last_points_helper[]){
    int a=0, b=0, c=0, driver_a_points, driver_b_points;
    DriverStatus status = DRIVER_STATUS_OK;
    while ((a<len1)&&(b<len2)) {
        driver_a_points = DriverGetPoints(driver_array1[a], &status);
        driver_b_points = DriverGetPoints(driver_array2[b], &status);
        if (driver_a_points == driver_b_points) {
            if ((last_points_array1[a]) > (last_points_array2[b])) {
                driver_helper[c] = driver_array1[a];
                last_points_helper[c++] = last_points_array1[a++];
            } else {
                driver_helper[c] = driver_array2[b];
                last_points_helper[c++] = last_points_array2[b++];
            }
        }else if (driver_a_points > driver_b_points) {
            last_points_helper[c] = last_points_array1[a];
            driver_helper[c++] = driver_array1[a++];
        } else {
            last_points_helper[c] = last_points_array2[b];
            driver_helper[c++] = driver_array2[b++];
        }
    }
    while (a<len1){
        last_points_helper[c] = last_points_array1[a];
        driver_helper[c++] = driver_array1[a++];
    }
    while (b<len2){
        last_points_helper[c] = last_points_array2[b];
        driver_helper[c++] = driver_array2[b++];
    }
    for (int i=0;i<len1+len2;i++) {
        last_points_array1[i] = last_points_helper[i];
        driver_array1[i] = driver_helper[i];
    }
}

void mergeSortDrivers(Driver drivers[], int number_of_drivers,
                      Driver driver_helper[],int last_points[],
                      int last_points_helper[]){
    if (number_of_drivers==1)return;
    int half = number_of_drivers/2;
    int remainder =number_of_drivers%2;
    mergeSortDrivers(drivers, half, driver_helper,last_points,last_points_helper);
    mergeSortDrivers(drivers + half, half+ remainder, driver_helper,
                     last_points+half,last_points_helper);
    mergeDrivers(drivers,drivers+half,half,half+remainder,driver_helper,
                 last_points,last_points+half, last_points_helper);
}

Team SeasonGetTeamByPosition(Season season, int position, SeasonStatus* status){
    if (position < 1){
        if (status!=NULL){
            *status=BAD_SEASON_INFO;
        }
        return NULL;
    }
    if (season == NULL){
        if (status!=NULL){
            *status=SEASON_NULL_PTR;
        }
        return NULL;
    }
    sortTeams(season,season->teams,season->number_of_teams);
    if (status!=NULL){
        *status=SEASON_OK;
    }
    return season->teams[position-1];
}

void sortTeams(Season season,Team teams[], int number_of_teams){
    int *point_helper = malloc(sizeof(int)*number_of_teams);
    if (point_helper==NULL){
        printf("Allocation error\n");
        return;
    }
    Team *sort_helper = malloc(sizeof(Team)*number_of_teams);
    if (sort_helper==NULL){
        printf("Allocation error\n");
        return;
    }
    mergeSortTeams(teams, number_of_teams, sort_helper, point_helper,
                   season->teams_last_race_result,season);
    free(point_helper);
    free(sort_helper);
}

void mergeSortTeams(Team teams[], int number_of_teams,Team sort_helper[],
                    int point_helper[], int last_results[],Season season){
    if (number_of_teams==1)return;
    int half = number_of_teams/2;
    int remainder =number_of_teams%2;
    mergeSortTeams(teams, half, sort_helper,point_helper,last_results,season);
    mergeSortTeams(teams + half, half+ remainder, sort_helper, point_helper,
                   last_results+half,season);
    mergeTeams(teams,teams+half,half,half+remainder,sort_helper,last_results,
               last_results+half,point_helper, season);
}

void mergeTeams(Team team_array1[],Team team_array2[],
                int len1,int len2, Team team_helper[],
                int last_points_array1[], int last_points_array2[],
                int last_points_helper[], Season season){
    int a=0, b=0, c=0, team_a_points, team_b_points;
    TeamStatus status = TEAM_STATUS_OK;
    while ((a<len1)&&(b<len2)) {
        team_a_points = TeamGetPoints(team_array1[a], &status);
        team_b_points = TeamGetPoints(team_array2[b], &status);
        if (team_a_points == team_b_points) {
            if (last_points_array1[a]==last_points_array2[b]){
                if (getBestDriverPoints(season,team_array1[a])>
                        getBestDriverPoints(season,team_array2[b])){
                    team_helper[c] = team_array1[a];
                    last_points_helper[c++] = last_points_array1[a++];
                } else {
                    team_helper[c] = team_array2[b];
                    last_points_helper[c++] = last_points_array2[b++];
                }
            }else if ((last_points_array1[a]) > (last_points_array2[b])) {
                team_helper[c] = team_array1[a];
                last_points_helper[c++] = last_points_array1[a++];
            } else {
                team_helper[c] = team_array2[b];
                last_points_helper[c++] = last_points_array2[b++];
            }
        }else if (team_a_points > team_b_points) {
            last_points_helper[c] = last_points_array1[a];
            team_helper[c++] = team_array1[a++];
        } else {
            last_points_helper[c] = last_points_array2[b];
            team_helper[c++] = team_array2[b++];
        }
    }
    while (a<len1){
        last_points_helper[c] = last_points_array1[a];
        team_helper[c++] = team_array1[a++];
    }
    while (b<len2){
        last_points_helper[c] = last_points_array2[b];
        team_helper[c++] = team_array2[b++];
    }
    for (int i=0;i<len1+len2;i++){
        last_points_array1[i]=last_points_helper[i];
        team_array1[i]=team_helper[i];
    }
}

Team* SeasonGetTeamsStandings(Season season){
    if (season == NULL){
        return NULL;
    }
    sortTeams(season,season->teams,season->number_of_teams);
    return season->teams;
}

Driver* SeasonGetDriversStandings(Season season){
    if (season == NULL){
        return NULL;
    }
    sortDrivers(season->drivers,season->number_of_drivers,
                season->drivers_last_race_result);
    return season->drivers;
}

int SeasonGetNumberOfDrivers(Season season){
    if (season == NULL){
        return 0;
    }
    return season->number_of_drivers;
}

int SeasonGetNumberOfTeams(Season season){
    if (season == NULL){
        return 0;
    }
    return season->number_of_teams;
}

SeasonStatus SeasonAddRaceResult(Season season, int* results){
    if ((season==NULL)||(results==NULL)){
        return SEASON_NULL_PTR;
    }
    for (int i =0; i< season->number_of_teams;i++){
        season->teams_last_race_result[i]=0;
    }
    for (int i =0; i< season->number_of_drivers;i++){
        season->drivers_last_race_result[i]=0;
    }
    int number_of_drivers = season->number_of_drivers, drivers_index,
            points;
    for (int i=0;i< number_of_drivers;i++) {
        points= number_of_drivers - (i+1);
        drivers_index= findIndexOfDriverWithThisId(season,results[i]);
        season->drivers_last_race_result[drivers_index] = points;
        DriverAddRaceResult(season->drivers[drivers_index],i+1);
    }
    fillNewestResultsInTeamArray(season);
    return SEASON_OK;
}

int findIndexOfDriverWithThisId(Season season,int id){
    if ((id>season->number_of_drivers)||(id<1))
        return -1;
    for (int i = 0; i < season->number_of_drivers; i++){
        if (id == DriverGetId(season->drivers[i])) return i;
    }
    return -1;
}

void fillNewestResultsInTeamArray(Season season){
    if (season == NULL){
        return;
    }
   /*
    for (int i=0 ; i< season->number_of_teams;i++){
        season->teams_last_race_result[i]= newRaceResult(season,season->teams[i],i);
        printf("\n%s just earned %d points ",TeamGetName(season->teams[i]),
                                                         season->teams_last_race_result[i]);
    }
    */
}

int newRaceResult(Season season, Team team,int index){
    Driver driver1 = TeamGetDriver(team,FIRST_DRIVER);
    Driver driver2 = TeamGetDriver(team,SECOND_DRIVER);
    int first_drivers_index=0,second_drivers_index=0,sum=0;
    if (driver1!=NULL){
        first_drivers_index =
                findIndexOfDriverWithThisId(season, DriverGetId(driver1));
   //     printf("\n index: %d got in last race: %d points" , first_drivers_index,
    //           season->drivers_last_race_result[first_drivers_index]);
        sum +=season->drivers_last_race_result[first_drivers_index];
    }
    if (driver2!=NULL) {
        second_drivers_index =
                findIndexOfDriverWithThisId(season, DriverGetId(driver2));
   //     printf("\n index: %d got in last race: %d points", second_drivers_index,
   //            season->drivers_last_race_result[second_drivers_index]);
        sum +=season->drivers_last_race_result[second_drivers_index];
    }
    return sum;
}

int getBestDriverPoints(Season season,Team team){
    int first_drivers_last_points=0,second_drivers_last_points=0;
    int first_drivers_index = findIndexOfDriverWithThisId(season
            ,DriverGetId(TeamGetDriver(team,FIRST_DRIVER)));
    if (first_drivers_index!=-1){
        first_drivers_last_points =
                season->drivers_last_race_result[first_drivers_index];
    }
    int second_drivers_index = findIndexOfDriverWithThisId(season
            ,DriverGetId(TeamGetDriver(team,SECOND_DRIVER)));
    if (second_drivers_index!=-1){
        second_drivers_last_points =
                season->drivers_last_race_result[second_drivers_index];
    }
    return first_drivers_last_points > second_drivers_last_points ?
           first_drivers_last_points : second_drivers_last_points;
}