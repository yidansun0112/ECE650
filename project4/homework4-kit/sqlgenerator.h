#ifndef __SQLGENERATOR_H__
#define __SQLGENERATOR_H__
#include <string>
using namespace std;

class Sqlgenerator{
  public:
    string getDrop();
    string getCreatePlayer();
    string getCreateTeam();
    string getCreateState();
    string getCreateColor();
    string getCreateTable();
};

#endif