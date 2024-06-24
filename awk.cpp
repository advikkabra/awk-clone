#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

class Parser {
// I guess this can have one function which takes in a string and splits it based on the given character? idk how else to incorporate inheritance.
};

class FileParser : public Parser {
  public:
    std::vector<std::vector<std::string>> parsed;
    FileParser(std::string &&filename, char delimiter=' ') {
      std::ifstream fin(filename);

      if (!fin) {
        std::cerr << "Error opening file" << std::endl;
        return;
      }

      std::string line;
      while (std::getline(fin, line)) {
        std::stringstream ss(line);

        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(ss, token, delimiter)) {
          tokens.push_back(token);
        }

        parsed.push_back(tokens);
      }
    }
};

class PatternParser : public Parser {

};

int main (int argc, char *argv[]) 
{
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <pattern { action }> <filename>" << std::endl;
    return 1;
  } 

  FileParser file(std::move(argv[2]));



  return 0;
}
