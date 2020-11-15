// FileLogger.cpp

//
#include <fstream>
#include <iostream>

// BZ
#include "bzfsAPI.h"
#include "bzcommon.h"

class FileLogger : public bz_Plugin
{
public:
    virtual const char* Name(){return "FileLogger";}
    virtual void Init ( const char* /*config*/ );
    virtual void Event(bz_EventData *eventData );
    virtual void Cleanup ( void );
    // Variables
    bool logEnabled = false;
    bool log_sync = false;
    bool log_last_sync = false;
    bool log_total_sync = false; 
    std::string line = "";
    std::ofstream log;
    std::ofstream log_last;
    std::ofstream log_total;
//
void logs(std::string lineToLog) {
    log << lineToLog << "\n";
    log_last << lineToLog << "\n";
    log_total << lineToLog << "\n"; 
}

void intialize(void) {
    std::string date = get_date();
    std::string time = get_time();
    std::string now = date + "-" + time;
    log.open(bz_format("stats/_log_%s.txt", now.c_str()), std::ios::out | std::ios::trunc);
    log_last.open("stats/_log_match.txt", std::ios::out | std::ios::trunc);
    log_total.open("stats/_log_total.txt", std::ios::out | std::ios::app);
    log_sync = true;
    log_last_sync = true;
    log_total_sync = true;
    line = "\n------------ SERVER START [ " + date + " " + time + " ] ------------ \n";
    logs(line);
}

void game_end_function(void) {
    logEnabled = false;
    int winner = get_winner();
    line = "*** Game ended.";
    logs(line);
    std::string winner_callsign = bz_getPlayerCallsign(winner);
    line = "*** '" + winner_callsign + "' has won!";
    logs(line);
    bz_APIIntList *playerList = bz_newIntList();
    if (playerList) {
        line = "*** Game players";
        logs(line);
        bz_getPlayerIndexList(playerList);
        for ( unsigned int i = 0; i < playerList->size(); i++) {
            int targetID = (*playerList)[i];
            bz_BasePlayerRecord *playRec = bz_getPlayerByIndex ( targetID );
            if (playRec != NULL) {
                line = bz_format("*** Player - '%s'", playRec->callsign);;
                logs(line); 
            }
            bz_freePlayerRecord(playRec);
        }
        bz_deleteIntList(playerList);
    }
}
//
};

BZ_PLUGIN(FileLogger)

void FileLogger::Init (const char* commandLine) {
    Register(bz_ePlayerJoinEvent);
    Register(bz_ePlayerPartEvent);
    Register(bz_ePlayerDieEvent);
    Register(bz_eGameStartEvent);
    Register(bz_eGameEndEvent);
}

void FileLogger::Cleanup (void) {
    line = bz_format("\n------------ SERVER END [ %s %s ] ------------",get_date().c_str(), get_time().c_str());
    logs(line);  
    
    log.close();
    log_last.close();
    log_total.close(); 

    Flush();
}

void FileLogger::Event(bz_EventData *eventData ){
    switch (eventData->eventType) {

     case bz_ePlayerJoinEvent: {
        bz_PlayerJoinPartEventData_V1* joinData = (bz_PlayerJoinPartEventData_V1*)eventData;
        std::string callsign = joinData->record->callsign;
        bz_eTeamType team = joinData->record->team;
        line = bz_format("*** '%s' joined the game as %s.",callsign.c_str(), get_team_color(team).c_str());
        logs(line);
     }break;

     case bz_ePlayerPartEvent: {
        bz_PlayerJoinPartEventData_V1* partData = (bz_PlayerJoinPartEventData_V1*)eventData;
        std::string callsign = partData->record->callsign;
        line = bz_format("*** '%s' left the game.", callsign.c_str());
        logs(line);
     }break;

     case bz_ePlayerDieEvent: {
        bz_PlayerDieEventData_V2* deathData = (bz_PlayerDieEventData_V2*)eventData;
        if ((logEnabled == true) && (is_server_player(deathData->killerID) == false)) {
            int killer = deathData->killerID;
            int player = deathData->playerID;
            std::string flagCarried = deathData->flagKilledWith;

            if (killer == player) {
                line = bz_format("*** '%s' blew myself up.", bz_getPlayerCallsign(killer));
                logs(line);
            } else {
                std::string flag = "no";
                if (strlen(flagCarried.c_str()) > 0) {
                    flag = flagCarried;
                }
                line = bz_format("*** '%s' destroyed by '%s' who carried -%s- flag.",
                    bz_getPlayerCallsign(player), bz_getPlayerCallsign(killer), flag.c_str());
                logs(line);
            }

            if (is_score_max(killer)) {
                game_end_function();
            }
        }
     }break;

     case bz_eGameStartEvent: {
        bz_GameStartEndEventData_V2* gameStartData = (bz_GameStartEndEventData_V2*)eventData;
        logEnabled = true;
        line = "*** Game started.";
        logs(line);
     }break;

     case bz_eGameEndEvent: {
        bz_GameStartEndEventData_V2* gameEndData = (bz_GameStartEndEventData_V2*)eventData;
        game_end_function();
     }break;

     default:{
     }break;
    }
}
