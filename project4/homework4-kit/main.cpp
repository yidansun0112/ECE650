#include <iostream>
#include <pqxx/pqxx>

#include "exerciser.h"
#include "sqlgenerator.h"

using namespace std;
using namespace pqxx;

void createTables(connection *c){
  Sqlgenerator sql;
  string total=sql.getCreateTable();
  work W(*c);
  W.exec(total);
  W.commit();
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
  //exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


