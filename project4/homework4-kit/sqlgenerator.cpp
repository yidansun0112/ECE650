#include sqlgenerator.h

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

}
string Sqlgenerator::getCreateTeam(){

}
string Sqlgenerator::getCreateState(){
  string header="CREATE TABLE STATE\n"+"(";
  string ender=");\n";
  string state_id="STATE_ID INT NOT NULL,\n";
  string name="NAME VARCHAR(50),\n";
  string primary_key="CONSTRAINT STATID PRIMARY KEY(STATE_ID)\n";
  string total=header+state_id+name+primary_key+ender;
  return total;
}

string Sqlgenerator::getCreateColor(){
  string header="CREATE TABLE COLOR\n"+"(";
  string ender=");\n";
  string color_id="COLOR_ID INT NOT NULL,\n";
  string name="NAME VARCHAR(50),\n";
  string primary_key="CONSTRAINT STATID PRIMARY KEY(COLOR_ID)\n";
  string total=header+color_id+name+primary_key+ender;
  return total;
}

string Sqlgenerator::getCreateTable(){

}
