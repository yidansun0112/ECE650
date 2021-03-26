#include <iostream>
#include <pqxx/pqxx>
#include <fstream>
#include <sstream>

#include "exerciser.h"
#include "sqlgenerator.h"

using namespace std;
using namespace pqxx;

void createTables(connection *C){
  Sqlgenerator sql;
  string total=sql.getCreateTable();
  work W(*C);
  W.exec(total);
  W.commit();
}

void addStates(connection *C){
  ifstream f("state.txt");
  if(f.fail()){
    throw invalid_argument("Cannot find player.txt");
  }
  string str;
  int id;
  string name;
  while(getline(f,str)){
    stringstream ss(str);
    ss>>id>>name;
    add_state(C,name);
  }
}

void addColors(connection *C){
  ifstream f("color.txt");
  if(f.fail()){
    throw invalid_argument("Cannot find color.txt");
  }
  string str;
  int id;
  string name;
  while(getline(f,str)){
    stringstream ss(str);
    ss>>id>>name;
    add_color(C,name);
  }
}

void addPlayers(connection *C){
  ifstream f("player.txt");
  if(f.fail()){
    throw invalid_argument("Cannot find player.txt");
  }
  string str;
  int id;
  int team_id, jersey_num, mpg, ppg, rpg, apg;
  double spg,bpg;
  string first_name,last_name;
  while(getline(f,str)){
    stringstream ss(str);
    ss>>id>>team_id>>jersey_num>>first_name>>last_name>>mpg>>ppg>>rpg>>apg>>spg>>bpg;
    add_player(C,team_id,jersey_num,first_name,last_name,mpg,ppg,rpg,apg,spg,bpg);
  }
}

void addTeams(connection *C){
  ifstream f("team.txt");
  if(f.fail()){
    throw invalid_argument("Cannot find team.txt");
  }
  string str;
  int id;
  int state_id,color_id,wins,losses;
  string name;
  while(getline(f,str)){
    stringstream ss(str);
    ss>>id>>name>>state_id>>color_id>>wins>>losses;
    add_team(C,name,state_id,color_id,wins,losses);
  }
}


int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }


  //TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  //      load each table with rows from the provided source txt files

  createTables(C);
  addStates(C);
  addColors(C);
  addTeams(C);
  addPlayers(C);
  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


