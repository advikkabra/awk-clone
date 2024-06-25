#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
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
    std::string pattern="";
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
      for (int i = 0; i < pattern.length(); i++)
      {
        if(pattern[i]!='&' && s!=""){
          vect_patterns.push_back(s);
          s="";
        }
        else if(pattern[i]!='&')s+=pattern[i];
      }
      
    } else {
      std::cout << "Usage: " << " <pattern { action }> <filename>" << std::endl;
      exit(0);
    }
  }
  bool Check_Pattern(std::string &str, int row_num){
    bool val = true;
  for(std::string pattern : vect_patterns){
    if(pattern=="1")val = true;
    else if(pattern=="0")val = false;
    else if(pattern[0]=='/' && pattern[pattern.size()-1]=='/'){
      std::string red_pattern = pattern.substr(1, pattern.size() - 2); //Reduced Pattern
      size_t found = str.find(red_pattern);
      if(found!=std::string::npos)return true;
      val = false;
    }
    else if(pattern[0]=='N' && pattern[1]=='R'){
      int num = stoi(pattern.substr(3));
      switch (pattern[2])
      {
      case '>':
        if(row_num<=num)val=false;
        break;
      case '<':
        if(row_num>=num)val=false;
        break;
      case '=':
        if(row_num!=num)val=false;
        break;
      case '>=':
        if(row_num<num)val=false;
        break;
      case '<=':
        if(row_num>num)val=false;
        break;
      default:
        std::cout << "Invalid Pattern" << std::endl;
        exit(0);
        break;
      }
    }
    else{
      std::cout << "Invalid Pattern" << std::endl;
      exit(0);
    }
    if(val==false)return false;
  }
  return true;
  }

  const std::vector<std::string> &get_pattern() const { return vect_patterns; }

private:
  std::vector<std::string> vect_patterns ;
};


class Stmt {
public:
  virtual void run(const std::vector<std::string> &tokens) = 0;
};

// Only supports fields at the moment ($0, $1 etc.)
// Always adds a delimiter of space whether or not comma was present b/w fields
class PrintStmt : public Stmt {
public:
  PrintStmt(std::string::const_iterator &it, const std::string &input): fields() {
    bool is_field = false;
    int field = -1;
    for (; it != input.cend() && *it != ';' && *it != '}'; ++it) {
      if (is_field) {
        if (!std::isdigit(*it)) {
          if (field == -1) throw std::runtime_error("Syntax error!");

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
      if (*it == '$') is_field = true;
    }

    if (field != -1) fields.push_back(field);
  }

  void run(const std::vector<std::string> &tokens) {
    for (const int field : fields) {
      if (field == 0) {
        for (int i = 0; i < tokens.size(); ++i) {
          std::cout << tokens[i];
          if (i != tokens.size() - 1) std::cout << ' ';
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
  ActionParser(const std::string &input): stmts() {
    std::string::const_iterator it = input.cbegin();

    // Find start of action.
    for (; it != input.cend() && *it != '{'; ++it) {}

    ++it;
    std::string stmt;
    for (; it != input.cend() && *it != '}'; ++it) {
      if (*it == ' ') continue;
      if (stmt == "print") stmts.push_back(new PrintStmt(it, input));

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
  std::vector<Stmt*> stmts;
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
