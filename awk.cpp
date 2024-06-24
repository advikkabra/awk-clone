#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

class Parser {
  // I guess this can have one function which takes in a string and splits it
  // based on the given character? idk how else to incorporate inheritance.
};

class FileParser : public Parser {
public:
  FileParser(std::string &&filename, char delimiter = ' ') {
    std::ifstream fin(filename);

    if (!fin) {
      std::cerr << "Error opening file" << std::endl;
      exit(0);
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

  const std::vector<std::vector<std::string>> &get_parsed() const {
    return parsed;
  }

private:
  std::vector<std::vector<std::string>> parsed;
};

class PatternParser : public Parser {
public:
  PatternParser(const std::string &str) {
    std::string def_pattern = "1"; // Default Pattern
    int choice = 0;
    for (int i = 0; i < str.length(); i++) {
      if (str[i] != '{') {
        if (str[i] != ' ')
          pattern += str[i];
      } else {
        choice = 1;
        break;
      }
    }
    if (choice == 1) {
      if (pattern == "")
        pattern = def_pattern;
    } else {
      std::cout << "Usage: " << " <pattern { action }> <filename>" << std::endl;
      exit(0);
    }
  }

  const std::string &get_pattern() const { return pattern; }

private:
  std::string pattern = "";
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <pattern { action }> <filename>"
              << std::endl;
    return 1;
  }

  FileParser file(std::move(argv[2]));
  PatternParser pattern(argv[1]);

  return 0;
}
