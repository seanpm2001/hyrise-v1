// Copyright (c) 2014 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include "QueryManager.h"

#include <iostream>
#include <algorithm>

namespace hyrise {
namespace access {

QueryManager::QueryManager() {

}

QueryManager& QueryManager::instance() {
  static auto* qm = new QueryManager();
  return *qm;
}

void QueryManager::registerQuery(const std::string& name, std::shared_ptr<aging::SelectExpression> select) {
  if (exists(name))
    return; //TODO throw std::runtime_error("Query \"" + name + "\" already exists");
  select->verify();

  const query_t id = _selectExpressions.size();
  _selectExpressions.push_back(select);
  _queryNames.insert(std::make_pair(name, id));

  std::cout << "register Query \"" << name << "\"" << std::endl;

  cleanRegistered();

  /*TODO for (const auto& registeredIndex : _registered) {
    const auto& index = registeredIndex.lock();
    const auto tableId = index->table()->id();
    std::vector<field_t> fields;
    for (const auto& param : params) {
      if (param.table == tableId)
        fields.push_back(param.field);
    }
  }*/
}

bool QueryManager::exists(const std::string& name) const {
  return _queryNames.find(name) != _queryNames.end();
}

void QueryManager::assureExists(const std::string& name) const {
  if (!exists(name))
    throw std::runtime_error("Query \"" + name + "\" does not exist");
}

query_t QueryManager::getId(const std::string& name) const {
  const auto& it = _queryNames.find(name);
  if (it == _queryNames.end())
    assureExists(name); // which will fail, but no need to rewrite the error code
  return it->second;
}

const std::string& QueryManager::getName(query_t query) const {
  for (const auto& queryName : _queryNames) {
    if (queryName.second == query)
      return queryName.first;
  }
  throw std::runtime_error("QueryID is not registered");
}

std::shared_ptr<aging::SelectExpression> QueryManager::selectExpressionOf(query_t query) const {
  if (query >= _selectExpressions.size())
    throw std::runtime_error("invalid query id");
  return _selectExpressions.at(query);
}

std::vector<query_t> QueryManager::queriesOfTable(storage::atable_ptr_t table) {
  std::vector<query_t> ret;
  for (unsigned i = 0; i < _selectExpressions.size(); ++i) {
    if (_selectExpressions.at(i)->accessesTable(table))
      ret.push_back(i);
  }
  return ret;
}

void QueryManager::cleanRegistered() {
  //TODO necessary
  _registered.erase(
    std::remove_if(_registered.begin(), _registered.end(),
        [](const std::weak_ptr<storage::AgingIndex>& index) { return index.expired(); }),
    _registered.end());
}

} } // namespace hyrise::access

