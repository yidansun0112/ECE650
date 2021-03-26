#include "exerciser.h"

void exercise(connection *C)
{
  cout << "TEST QUERY1:\n\n";
  query1(C, 1, 15, 20, 0, 0, 0, 0, 10, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  cout << "\n";
  query1(C, 0, 10, 40, 0, 0, 0, 2, 10, 20, 1, 0, 5, 0, 0, 0, 0, 0, 0);

}
