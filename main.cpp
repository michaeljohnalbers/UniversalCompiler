/**
 * @file main.cpp
 * @brief Entry point of program
 *
 * @author Michael Albers
 */

#include <iostream>
#include <stdexcept>
#include <string>

#include "ErrorWarningTracker.h"
#include "Scanner.h"

int main(int argc, char **argv)
{
  try
  {
    if (argc == 1)
    {
      throw std::runtime_error("No input file provided.");
    }

    std::string file(argv[1]);

    ErrorWarningTracker ewTracker(file);
    Scanner scanner(file, ewTracker);

    DriverTable::TokenCode tokenCode;
    std::string token;
    do
    {
      scanner.scan(token, tokenCode);
      std::cout << "Token: " << DriverTable::getDescription(tokenCode)
                << " (\"" << token << "\")" << std::endl;
    }
    while (tokenCode != DriverTable::TokenCode::EofSym);
  }
  catch (const std::exception &exception)
  {
    std::cerr << argv[0] << ": error: " << exception.what() << std::endl;
    return 1;
  }

  return 0;
}
