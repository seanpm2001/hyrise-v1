// Copyright (c) 2014 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#pragma once

#include <helper/types.h>

#include "access/system/PlanOperation.h"

namespace hyrise {
namespace access {

struct param_t {
  storage::table_id_t table;
  field_t field;
};
typedef std::vector<param_t> param_vector_t;

class QueryManager {
public:
  QueryManager(const QueryManager&) = delete;

  static QueryManager& instance();

  void registerQuery(const std::string& name, const param_vector_t& params);

  bool exists(const std::string& name) const;
  void assureExists(const std::string& name) const;
  query_id_t getId(const std::string& name) const;

  param_vector_t parametersOf(const std::string& query) const;
  param_vector_t parametersOf(query_id_t query) const;

private:
  QueryManager();

  std::vector<param_vector_t> _queryParameters;
  std::map<std::string, query_id_t> _queryNames;
};

} } // namespace hyrise::access

