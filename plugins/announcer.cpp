// announcer.cpp

#include "bzfsAPI.h"

class announcer: public bz_Plugin
{
public:
    virtual const char* Name(){return "Announcer";}
    virtual void Init ( const char* /*config*/ );
    virtual void Event(bz_EventData *eventData );
    virtual void Cleanup ( void );
    // utility functions
    int getWinner(void) {
        int winner = -1;
        int wnWin = 0;
        int wnLoss = 0;
        
        bz_APIIntList *player_list = bz_newIntList();
        bz_getPlayerIndexList(player_list);

        for (unsigned int i = 0; i < player_list->size(); i++) {
            // (player_list->get(i))
            int plyr = player_list->get(i);
            int plWins = bz_getPlayerWins(plyr);
            int plLosses = bz_getPlayerLosses(plyr);
            int playerScore = plWins - plLosses;
            int winnerScore = wnWin - wnLoss;
            
            if (playerScore > winnerScore) {
                winner = plyr; 
            }
        }
        bz_deleteIntList(player_list);

        return winner;
    }
    
    void gameEndFunction(void) {
        int gmWinner = getWinner();
        bz_BasePlayerRecord *pr = bz_getPlayerByIndex(gmWinner);
        if (pr) {
            bz_sendTextMessagef(BZ_SERVER, BZ_ALLUSERS, "%s has won! Congratulations!", pr->callsign.c_str());
            bz_freePlayerRecord(pr);
        }  
    }

};

BZ_PLUGIN(announcer)

void announcer::Init (const char*config) {
    Register(bz_ePlayerJoinEvent);
    Register(bz_ePlayerDieEvent);
    Register(bz_eGameEndEvent);
}

void announcer::Cleanup (void) {
    Flush();
}

void announcer::Event(bz_EventData *eventData ){
    switch (eventData->eventType) {

     case bz_ePlayerJoinEvent: {
     bz_PlayerJoinPartEventData_V1* joinData = (bz_PlayerJoinPartEventData_V1*)eventData;

     bz_BasePlayerRecord *pr = bz_getPlayerByIndex(joinData->playerID);
        if (pr) {
            bz_sendTextMessagef(BZ_SERVER, BZ_ALLUSERS, "Everybody say 'TI-DI-DI', %s has joined!", pr->callsign.c_str());
            bz_freePlayerRecord(pr);
        }
     }break;

     case bz_ePlayerDieEvent: {
        bz_PlayerDieEventData_V1* deathData = (bz_PlayerDieEventData_V1*)eventData;
        
        int killer = deathData->killerID;
        if (killer != 253) { // If killer is not equal to server.
            int wins = bz_getPlayerWins(killer);
            int losses = bz_getPlayerLosses(killer);
            int score = wins - losses;
                if ((score + 1) == 20) {
                    gameEndFunction();
                } 
        }
     }break;
     
     case bz_eGameEndEvent: {
        bz_GameStartEndEventData_V2* gameEndData = (bz_GameStartEndEventData_V2*)eventData;      
        gameEndFunction();
     }break;
     
     default: {
     } break;
    }
}
