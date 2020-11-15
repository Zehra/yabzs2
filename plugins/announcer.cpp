// Announcer.cpp

#include "bzfsAPI.h"
#include "bzcommon.h"

void game_end_function(void) {
    int winner = get_winner();   
    bz_sendTextMessagef(BZ_SERVER, BZ_ALLUSERS, "%s has won! Congratulations!", bz_getPlayerCallsign(winner));
    // Modified: Instead of a per-player message loop, it simply announces to all users.
}

class Announcer : public bz_Plugin
{
public:
    virtual const char* Name(){return "Announcer";}
    virtual void Init ( const char* /*config*/ );
    virtual void Event(bz_EventData *eventData );
    virtual void Cleanup ( void );
};

BZ_PLUGIN(Announcer)

void Announcer::Init (const char* commandLine) {
    Register(bz_ePlayerJoinEvent);
    Register(bz_eGameEndEvent);
    Register(bz_ePlayerDieEvent);
}

void Announcer::Cleanup (void) {
    Flush();
}

void Announcer::Event(bz_EventData *eventData ){
    switch (eventData->eventType) {
     case bz_ePlayerJoinEvent: {
        bz_PlayerJoinPartEventData_V1* joinData = (bz_PlayerJoinPartEventData_V1*)eventData;
        bz_sendTextMessagef(BZ_SERVER, BZ_ALLUSERS, "Everybody say 'TI-DI-DI', %s has joined!",
            joinData->record->callsign.c_str());
     }break;

     case bz_eGameEndEvent: {
        bz_GameStartEndEventData_V2* gameEndData = (bz_GameStartEndEventData_V2*)eventData;
        game_end_function();
     }break;

     case bz_ePlayerDieEvent: {
        bz_PlayerDieEventData_V2* deathData = (bz_PlayerDieEventData_V2*)eventData;
        int killer = deathData->killerID;
        if (!is_server_player(killer)) {
            if (is_score_max(killer)) {
                game_end_function();
                // Probably should be modified to prevent possible dual-announcement.
            }
        }
     }break;

     default:{ 
     }break;
    }
}
