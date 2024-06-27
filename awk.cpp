#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

class FileParser {
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

class PatternParser {
public:
  PatternParser(const std::string &str) {
    std::string pattern = "";
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
      std::string s = "";
      for (int i = 0; i < pattern.length(); i++) {
        if ((pattern[i] == '&' || pattern[i] == '|') && s != "") {
          vect_patterns.push_back(s);
          s = "";
          if (pattern[i] == '|')
            op_type = 1;
        } else if ((pattern[i] != '&') && (pattern[i] != '|'))
          s += pattern[i];
      }
      if (s != "")
        vect_patterns.push_back(s);
    } else {
      std::cout << "Usage: " << " <pattern { action }> <filename>" << std::endl;
      exit(0);
    }
  }

  bool check_pattern(const std::vector<std::string> &tokens, int row_num) {
    std::vector<bool> values;
    for (std::string pattern : vect_patterns) {
      bool val = true;
      int is_not = 0;
      if (pattern[0] == '!') {
        is_not = 1;
        pattern.erase(0, 1);
      }
      if (pattern == "1")
        val = true;
      else if (pattern == "0")
        val = false;
      else if (pattern[0] == '/' && pattern[pattern.size() - 1] == '/') {
        std::string red_pattern =
            pattern.substr(1, pattern.size() - 2); // Reduced Pattern
        val = false;
        for (const std::string &str : tokens) {
          size_t found = str.find(red_pattern);
          if (found != std::string::npos)
            val = true;
        }
      } else if (pattern[0] == 'N' && pattern[1] == 'R') {
        int num = stoi(pattern.substr(3));
        switch (pattern[2]) {
        case '>':
          if (row_num <= num)
            val = false;
          break;
        case '<':
          if (row_num >= num)
            val = false;
          break;
        case '=':
          if (row_num != num)
            val = false;
          break;
        default:
          std::cout << "Invalid Pattern" << std::endl;
          exit(0);
          break;
        }
      } else {
        std::cout << "Invalid Pattern" << std::endl;
        exit(0);
      }
      if (is_not)
        val = !val;
      values.push_back(val);
    }
    bool ans;
    if (op_type == 0)
      ans = true;
    else
      ans = false;
    for (int i = 0; i < values.size(); i++) {
      if (op_type == 1)
        ans = ans || values[i];
      else
        ans = ans && values[i];
    }
    return ans;
  }

  const std::vector<std::string> &get_pattern() const { return vect_patterns; }

private:
  std::vector<std::string> vect_patterns;
  int op_type = 0;
};

class Stmt {
public:
  virtual void run(const std::vector<std::string> &tokens) = 0;
};

// Only supports fields at the moment ($0, $1 etc.)
// Always adds a delimiter of space whether or not comma was present b/w fields
class PrintStmt : public Stmt {
public:
  PrintStmt(std::string::const_iterator &it, const std::string &input)
      : fields() {
    bool is_field = false;
    int field = -1;
    for (; it != input.cend() && *it != ';' && *it != '}'; ++it) {
      if (is_field) {
        if (!std::isdigit(*it)) {
          if (field == -1)
            throw std::runtime_error("Syntax error!");

          fields.push_back(field);
          field = -1;
          is_field = false;
        } else {
          if (field == -1) {
            field = (*it - '0');
          } else {
            field = field * 10 + (*it - '0');
          }
        }
      }

      // Potential field found.
      if (*it == '$')
        is_field = true;
    }

    if (field != -1)
      fields.push_back(field);
  }

  void run(const std::vector<std::string> &tokens) {
    for (const int field : fields) {
      if (field == 0) {
        for (int i = 0; i < tokens.size(); ++i) {
          std::cout << tokens[i];
          if (i != tokens.size() - 1)
            std::cout << ' ';
        }
      } else if (field <= tokens.size()) {
        std::cout << tokens[field - 1];
      }

      std::cout << ' ';
    }
  }

private:
  std::vector<int> fields;
};

class ActionParser {
public:
  ActionParser(const std::string &input) : stmts() {
    std::string::const_iterator it = input.cbegin();

    // Find start of action.
    for (; it != input.cend() && *it != '{'; ++it) {
    }

    ++it;
    std::string stmt;
    for (; it != input.cend() && *it != '}'; ++it) {
      if (*it == ' ')
        continue;
      if (stmt == "print")
        stmts.push_back(new PrintStmt(it, input));

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
      if (i != stmts.size() - 1)
        std::cout << std::endl;
    }
  }

private:
  std::vector<Stmt *> stmts;
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

  for (int i = 0; i < file.get_parsed().size(); ++i) {
    if (!pattern.check_pattern(file.get_parsed()[i], i + 1))
      continue;
    action.run(file.get_parsed()[i]);
    std::cout << std::endl;
  }

  return 0;
}
