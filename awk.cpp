#include <iostream>
#include <vector>

class Parser {
// I guess this can have one function which takes in a string and splits it based on the given character? idk how else to incorporate inheritance.
};

class FileParser : public Parser {
  public:
    std::vector<std::vector<std::string>> parsed;
    FileParser(std::string filename);

};

class PatternParser : public Parser {

};

int main (int argc, char *argv[]) 
{
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <pattern { action }> <filename>" << std::endl;
    return 1;
  } 

  FileParser file(argv[2]);


  return 0;
}
