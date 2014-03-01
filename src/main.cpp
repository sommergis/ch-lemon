#include <iostream>

#include "Test/Default.h"
#include "Test/Unweighted.h"
#include "Test/Sameorder.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "A graph is required for the test.\n" <<
      "See the README for a location where you can download one.";
  } else {
    Default_test(argv[1]);
    //Default_test("../Graphs/USA-road-d.RO.gr", 100);
    //Unweighted_test("../Graphs/USA-road-d.RO.gr", 1000);
    //Sameorder_test("../Graphs/USA-road-d.RO.gr", "../Graphs/USA-road-d.RO.gr", 100);
  }
}
