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
  stringstream ss;
  ss<<"SELECT * FROM PLAYER WHERE ";
  int num=0;
  checkUse(ss,use_mpg,min_mpg,max_mpg,num,"MPG");
  checkUse(ss,use_ppg,min_ppg,max_ppg,num,"PPG");
  checkUse(ss,use_rpg,min_rpg,max_rpg,num,"RPG");
  checkUse(ss,use_apg,min_apg,max_apg,num,"APG");
  checkUse(ss,use_spg,min_spg,max_spg,num,"SPG");
  checkUse(ss,use_bpg,min_bpg,max_bpg,num,"BPG");
  ss<<";\n";
  string command=ss.str();
  nontransaction N(*C);
  result R(N.exec(command));
  print_q1(R);
}

void print_q1(result R){
  cout<<"PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG\n";
  int size=R.size();
  for(int i=0;i<size;i++){
    pqxx::row const r=R[i];
    cout << r[0] << " ";
    cout << r[1] << " ";
    cout << r[2]<< " ";
    cout << r[3]<< " ";
    cout << r[4] << " ";
    cout << r[5] << " ";
    cout << r[6]<< " ";
    cout << r[7]<< " ";
    cout << r[8] << " ";
    cout << fixed << setprecision(1) << r[9].as<double>() << " ";
    cout << fixed << setprecision(1) << r[10].as<double>() << endl;
  }
}

void checkUse(stringstream &ss, int use, int min, int max, int &num,string name){
  if(use){
    if(num>0){
      ss<<" AND ";
    }
    ss<<"("<<name<<" BETWEEN "<<min<<" AND "<<max<<")";
    num++;
  }
}

void checkUse(stringstream &ss, int use, double min, double max, int &num,string name){
  if(use){
    if(num>0){
      ss<<" AND ";
    }
    ss<<"("<<name<<" BETWEEN "<<min<<" AND "<<max<<")";
    num++;
  }
}

void query2(connection *C, string team_color)
{
  stringstream ss;
  ss<<"SELECT TEAM.NAME FROM TEAM,COLOR WHERE COLOR.COLOR_ID=TEAM.COLOR_ID AND COLOR.NAME=";
  work W(*C);
  ss<<W.quote(team_color);
  ss<<";\n";
  string command=ss.str();
  W.commit();
  nontransaction N(*C);
  result R(N.exec(command));
  print_q2(R);
}

void print_q2(result R){
  cout<<"NAME"<<endl;
  int size=R.size();
  for(int i=0;i<size;i++){
    pqxx::row const r=R[i];
    cout << r[0].as<string>() << endl;
  }
}

void query3(connection *C, string team_name)
{
  stringstream ss;
  ss<<"SELECT FIRST_NAME,LAST_NAME FROM PLAYER, TEAM WHERE TEAM.TEAM_ID=PLAYER.TEAM_ID AND TEAM.NAME=";
  work W(*C);
  ss<<W.quote(team_name);
  ss<<" ORDER BY PPG DESC;\n";
  string command=ss.str();
  W.commit();
  nontransaction N(*C);
  result R(N.exec(command));
  print_q3(R);
}

void print_q3(result R){
  cout<<"FIRST_NAME LAST_NAME"<<endl;
  int size=R.size();
  for(int i=0;i<size;i++){
    pqxx::row const r=R[i];
    cout << r[0]<< " ";
    cout << r[1]<< endl;
  }
}

void query4(connection *C, string team_state, string team_color)
{
  stringstream ss;
  ss<<"SELECT FIRST_NAME,LAST_NAME,UNIFORM_NUM FROM PLAYER,STATE,COLOR,TEAM WHERE ";
  ss<<"STATE.STATE_ID=TEAM.STATE_ID";
  ss<<" AND COLOR.COLOR_ID=TEAM.COLOR_ID";
  ss<<" AND PLAYER.TEAM_ID=TEAM.TEAM_ID AND ";
  work W(*C);
  ss<<"STATE.NAME="<<W.quote(team_state);
  ss<<" AND COLOR.NAME="<<W.quote(team_color)<<";\n";
  W.commit();
  string command=ss.str();
  nontransaction N(*C);
  result R(N.exec(command));
  print_q4(R);
}

void print_q4(result R){
  cout<<"FIRST_NAME LAST_NAME UNIFORM_NUM"<<endl;
  int size=R.size();
  for(int i=0;i<size;i++){
    pqxx::row const r=R[i];
    cout << r[0]<< " ";
    cout << r[1]<< " ";
    cout << r[2]<< endl;
  }
}

void query5(connection *C, int num_wins)
{
  stringstream ss;
  ss<<"SELECT FIRST_NAME,LAST_NAME,TEAM.NAME,WINS FROM PLAYER,TEAM WHERE ";
  ss<<"TEAM.TEAM_ID=PLAYER.TEAM_ID AND TEAM.WINS>"<<num_wins<<";\n";
  string command=ss.str();
  nontransaction N(*C);
  result R(N.exec(command));
  print_q5(R);
}

void print_q5(result R){
  std::cout << "FIRST_NAME LAST_NAME NAME WINS"<<endl;
  int size=R.size();
  for(int i=0;i<size;i++){
    pqxx::row const r=R[i];
    cout << r[0]<< " ";
    cout << r[1]<< " ";
    cout << r[2]<< " ";
    cout << r[3]<< endl;
  }
}
