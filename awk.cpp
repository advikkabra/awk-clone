#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <memory>
#include <optional>

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


class Stmt {
public:
  virtual void run(const std::vector<std::string> &tokens) = 0;
};

// Only supports fields at the moment ($1 etc.)
// DOES NOT YET SUPPORT $0
class PrintStmt : public Stmt {
public:
  PrintStmt(std::string::const_iterator &it, const std::string &input): fields() {
    std::optional<int> field;

    for (; it != input.cend() && *it != ';' && *it != '}'; ++it) {
      if (field.has_value()) {
        if (!std::isdigit(*it)) {
          if (field.value() > 0) fields.push_back(field.value());
          field.reset();
        } else {
          field = field.value() * 10 + (*it - '0');
          continue;
        }
      }
      
      // Potential field found.
      if (*it == '$') field = 0;
    }

    if (field.has_value()) fields.push_back(field.value());
  }

  void run(const std::vector<std::string> &tokens) {
    const int max = tokens.size();
    for (const int field : fields) {
      if (field <= max) std::cout << tokens[field - 1];
      std::cout << ' ';
    }
  }
private:
  std::vector<int> fields;
};

class ActionParser {
public:
  ActionParser(const std::string &input): stmts() {
    std::string::const_iterator it = input.cbegin();

    // Find start of action.
    for (; it != input.cend() && *it != '{'; ++it) {}

    ++it;
    std::string stmt;
    for (; it != input.cend() && *it != '}'; ++it) {
      if (*it == ' ') continue;
      if (stmt == "print") stmts.push_back(std::make_unique<PrintStmt>(it, input));

      // Statements are delimited by ';'
      if (*it == ';') {
        stmt.clear();
        continue;
      }

      stmt += *it;
    }
  }

  void run(const std::vector<std::string> &tokens) {
    for (int i = 0; i < stmts.size(); ++i) {
      stmts[i]->run(tokens);
      if (i != stmts.size() - 1) std::cout << std::endl;
    }
  }

private:
  std::vector<std::unique_ptr<Stmt>> stmts;
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <pattern { action }> <filename>"
              << std::endl;
    return 1;
  }

  FileParser file(std::move(argv[2]));
  PatternParser pattern(argv[1]);
  ActionParser action(argv[1]);

  for (const auto &tokens : file.get_parsed()) {
    action.run(tokens);
    std::cout << std::endl;
  }

  return 0;
}
