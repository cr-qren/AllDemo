/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_STATUS_HH
#define RAINTIME_STATUS_HH

#include <memory>
#include <utility>

namespace raintime {

class Status {
 public:
  enum Code {
    OK_,
  };

  Status() = default;

  Status(Code code, std::string msg) {
    state_ = std::unique_ptr<State>(new State);
    state_->code = code;
    state_->msg = std::move(msg);
  }

  /*! create a status that is OK */
  static Status OK() { return {}; }

  /*! give whether the state is OK */
  bool ok() const { return (state_ == nullptr); }

  Code code() const {
    return (state_ == nullptr) ? OK_ : state_->code;
  }

  const std::string &error_message() const {
    return ok() ? empty_string() : state_->msg;
  }

 private:
  const std::string &empty_string() const {
    static auto *empty = new std::string;
    return *empty;
  }

  struct State {
    Code code;
    std::string msg;
  };

  std::unique_ptr<State> state_;
};
}

#endif //RAINTIME_STATUS_HH
