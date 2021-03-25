#include "query_funcs.h"
#include <iomanip>

void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
  stringstream ss;
  work W(*C);
  string header="INSERT INTO PLAYER (TEAM_ID,UNIFORM_NUM,FIRST_NAME,LAST_NAME,MPG,PPG,RPG,APG,SPG,BPG) VALUES(";
  ss<<header;
  ss<<team_id<<",";
  ss<<jersey_num<<",";
  ss<<W.quote(first_name)<<",";
  ss<<W.quote(last_name)<<",";
  ss<<mpg<<",";
  ss<<ppg<<",";
  ss<<rpg<<",";
  ss<<apg<<",";
  ss<<spg<<",";
  ss<<bpg<<");\n";
  string command=ss.str();
  W.exec(command);
  W.commit();
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses)
{
  stringstream ss;
  work W(*C);
  string header="INSERT INTO TEAM (NAME,STATE_ID,COLOR_ID,WINS,LOSSES) VALUES(";
  ss<<header;
  ss<<W.quote(name)+",";
  ss<<state_id<<",";
  ss<<color_id<<",";
  ss<<wins<<",";
  ss<<losses<<");\n";
  string command=ss.str();
  W.exec(command);
  W.commit();
}


void add_state(connection *C, string name)
{
  work W(*C);
  string command="INSERT INTO STATE (NAME) VALUES("+W.quote(name)+");\n";
  W.exec(command);
  W.commit();
}


void add_color(connection *C, string name)
{
  work W(*C);
  string command="INSERT INTO COLOR (NAME) VALUES("+W.quote(name)+");\n";
  W.exec(command);
  W.commit();
}


void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            )
{
}


void query2(connection *C, string team_color)
{
}


void query3(connection *C, string team_name)
{
}


void query4(connection *C, string team_state, string team_color)
{
}


void query5(connection *C, int num_wins)
{
}
