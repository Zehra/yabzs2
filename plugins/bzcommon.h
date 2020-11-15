// bzcommon.h
// ----------
// Part of the Yet-Another-BZ-Stats-(2) project.
//

std::string get_team_color(bz_eTeamType team_id) {
    std::string team = "";

    if (eRogueTeam == team_id) {
        team = "Rogue";
    } else if (eRedTeam == team_id) {
        team = "Red Team";
    } else if (eGreenTeam == team_id) {
        team = "Green Team";
    } else if (eBlueTeam == team_id) {
        team = "Blue Team";
    } else if (ePurpleTeam == team_id) {
        team = "Purple Team";
    } else if (eRabbitTeam == team_id) {
        team = "rabbit";
    } else if (eHunterTeam == team_id) {
        team = "hunter";
    } else if (eObservers == team_id) {
        team = "observer";
    } else if (eAdministrators == team_id) {
        team = "admin";
    } else {
        team = "unknown";
    }

    return team;
}

bool is_server_player(int id) {
    bool server = false;
    if (id == 253) {
        server = true;
    }
    return server;
}

bool is_score_max(int player) {
    int score = (bz_getPlayerWins(player) - bz_getPlayerLosses(player));
    if ((score + 1) == 20) {
        return true;
    } else {
        return false;
    }
}

std::string get_date(void) {
    bz_Time time;
    bz_getUTCtime(&time);// Appears to be by default UTC in Ruby, so we go with that.
    std::string Time = bz_format("%d-%d-%d",time.year, time.month, time.day);
    return Time;
}
  
std::string get_time(void) {
    bz_Time time;
    bz_getUTCtime(&time);// Time is UTC according to Ruby docs.
    std::string Time = bz_format("%d-%d-%d",time.hour, time.minute, time.second);
    return Time;
}

int get_winner(void) {
    int winner = -1;
    int winnerScore = 0;

    bz_APIIntList *playerList = bz_newIntList();
    if (playerList) {
    
    bz_getPlayerIndexList(playerList);
    for ( unsigned int i = 0; i < playerList->size(); i++) {
    int player = (*playerList)[i];
    bz_BasePlayerRecord* playRec = bz_getPlayerByIndex(player);
        if (playRec != NULL) {
            int score = (bz_getPlayerWins(player) - bz_getPlayerLosses(player));
            //: Originally written with score and winnerScore being compared by grabbing score updates.
            //: Should be equivalent to original, but more optimized/less redundant in comparisons.
            if (score > winnerScore) {
                winnerScore = score;
                winner = player;
            }
        }
        bz_freePlayerRecord(playRec);
    }
    bz_deleteIntList(playerList);
    }
    return winner;
}
