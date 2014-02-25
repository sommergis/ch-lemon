#include "Test/Default.h"
#include "Test/Unweighted.h"
#include "Test/Sameorder.h"

int main() {
  Default_test("../Graphs/USA-road-d.RO.gr", 100);
  //Unweighted_test("../Graphs/USA-road-d.RO.gr", 1000);
  //Sameorder_test("../Graphs/USA-road-d.RO.gr", "../Graphs/USA-road-d.RO.gr", 100);
}
