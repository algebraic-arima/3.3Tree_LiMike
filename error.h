#ifndef BPTREE_ERROR_H
#define BPTREE_ERROR_H
#pragma once

#include <string>
#include <exception>
#include <utility>

class ErrorException : public std::exception {
public:
  explicit ErrorException(std::string message) {
    this->message = std::move(message);
  }

  std::string getMessage() const {
    return message;
  }

private:
  std::string message;
};

void error(std::string message) {
  throw ErrorException(message);
}


#endif