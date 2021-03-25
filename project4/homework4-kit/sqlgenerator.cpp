#include "sqlgenerator.h"

string Sqlgenerator::getDrop(){
  string dropHeader="DROP TABLE IF EXISTS";
  string dropEnder="CASCADE;\n";
  string dropPlayer=dropHeader+" PLAYER "+dropEnder;
  string dropTeam=dropHeader+" TEAM "+dropEnder;
  string dropState=dropHeader+" STATE "+dropEnder;
  string dropColor=dropHeader+" COLOR "+dropEnder;
  string dropTotal=dropPlayer+dropTeam+dropState+dropColor+"\n";
  return dropTotal;
}

string Sqlgenerator::getCreatePlayer(){
  string header="CREATE TABLE PLAYER\n(";
  string ender=");\n";
  string player_id="PLAYER_ID SERIAL NOT NULL,\n";
  string team_id="TEAM_ID INT,\n";
  string uniform_num="UNIFORM_NUM INT,\n";
  string first_name="FIRST_NAME VARCHAR(50),\n";
  string last_name="LAST_NAME VARCHAR(50),\n";
  string mpg="MPG INT,\n";
  string ppg="PPG INT,\n";
  string rpg="RPG INT,\n";
  string apg="APG INT,\n";
  string spg="SPG FLOAT,\n";
  string bpg="BPG FLOAT,\n";
  string primary_key="CONSTRAINT PLAYPK PRIMARY KEY(PLAYER_ID),\n";
  string foreign_key_team="CONSTRAINT TMIDFK FOREIGN KEY(TEAM_ID) REFERENCES TEAM(TEAM_ID) ON DELETE SET NULL ON UPDATE CASCADE\n";
  string total=header+player_id+team_id+uniform_num+first_name+last_name+mpg+ppg+rpg+apg+spg+bpg+primary_key+foreign_key_team+ender;
  return total;
}
string Sqlgenerator::getCreateTeam(){
  string header="CREATE TABLE TEAM\n(";
  string ender=");\n";
  string team_id="TEAM_ID SERIAL NOT NULL,\n";
  string name="NAME VARCHAR(50),\n";
  string state_id="STATE_ID INT,\n";
  string color_id="COLOR_ID INT,\n";
  string wins="WINS INT,\n";
  string losses="LOSSES INT,\n";
  string primary_key="CONSTRAINT TEAMID PRIMARY KEY(TEAM_ID),\n";
  string foreign_key_state="CONSTRAINT STATEIDFK FOREIGN KEY(STATE_ID) REFERENCES STATE(STATE_ID) ON DELETE SET NULL ON UPDATE CASCADE,\n";
  string foreign_key_color="CONSTRAINT CLORIDFK FOREIGN KEY(COLOR_ID) REFERENCES COLOR(COLOR_ID) ON DELETE SET NULL ON UPDATE CASCADE\n";
  string total=header+team_id+name+state_id+color_id+wins+losses+primary_key+foreign_key_state+foreign_key_color+ender;
  return total;
}
string Sqlgenerator::getCreateState(){
  string header="CREATE TABLE STATE\n(";
  string ender=");\n";
  string state_id="STATE_ID SERIAL NOT NULL,\n";
  string name="NAME VARCHAR(50),\n";
  string primary_key="CONSTRAINT STATEIDPK PRIMARY KEY(STATE_ID)\n";
  string total=header+state_id+name+primary_key+ender;
  return total;
}

string Sqlgenerator::getCreateColor(){
  string header="CREATE TABLE COLOR\n(";
  string ender=");\n";
  string color_id="COLOR_ID SERIAL NOT NULL,\n";
  string name="NAME VARCHAR(50),\n";
  string primary_key="CONSTRAINT COLORIDPK PRIMARY KEY(COLOR_ID)\n";
  string total=header+color_id+name+primary_key+ender;
  return total;
}

string Sqlgenerator::getCreateTable(){
  string drop=getDrop();
  string state=getCreateState();
  string color=getCreateColor();
  string team=getCreateTeam();
  string player=getCreatePlayer();
  string total=drop+state+color+team+player;
  return total;
}
