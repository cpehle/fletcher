// Copyright 2018 Delft University of Technology
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <unistd.h>

#include "../../include/fletcher/logging.h"
#include "../../include/fletcher/UserCore.h"

namespace fletcher {

UserCore::UserCore(std::shared_ptr<FPGAPlatform> platform)
{
  this->_platform = platform;
  this->arg_offset = this->_platform->argument_offset();
}

bool UserCore::implements_schema(const std::shared_ptr<arrow::Schema>& schema)
{
  // TODO: Implement checking if the platform implements the same Schema
  std::cerr << schema->ToString();
  return true;
}

uc_stat UserCore::reset()
{
  this->_platform->write_mmsr(UC_REG_CONTROL, this->ctrl_reset);
  return SUCCESS;
}

uc_stat UserCore::set_arguments(std::vector<fr_t> arguments)
{
  LOGD("Setting arguments. Argument offset: " << this->arg_offset);
  for (int i = 0; (size_t) i < arguments.size(); i++) {
    this->_platform->write_mmsr(this->arg_offset + i, arguments[i]);
  }

  return SUCCESS;
}

uc_stat UserCore::start()
{
  this->_platform->write_mmsr(UC_REG_CONTROL, this->ctrl_start);
  return SUCCESS;
}

fr_t UserCore::get_status()
{
  return this->_platform->read_mmsr(UC_REG_STATUS);
}

fr_t UserCore::get_return()
{
  return this->_platform->read_mmsr(UC_REG_RETURN);
}

uc_stat UserCore::wait_for_finish()
{
  return wait_for_finish(0);
}

uc_stat UserCore::wait_for_finish(unsigned int poll_interval_usec)
{
  fr_t status = 0;
  if (poll_interval_usec == 0) {
    do {
      status = this->_platform->read_mmsr(UC_REG_STATUS);
    } while ((status & this->done_status_mask) != this->done_status);
  } else {
    do {
      usleep(poll_interval_usec);
      status = this->_platform->read_mmsr(UC_REG_STATUS);
    } while ((status & this->done_status_mask) != this->done_status);
  }
  return SUCCESS;
}

std::shared_ptr<FPGAPlatform> UserCore::platform()
{
  return this->_platform;
}

}