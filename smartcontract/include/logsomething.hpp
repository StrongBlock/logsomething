#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <string>
#include <vector>
#include "can_proceed.hpp"

using namespace eosio;
using std::string;
using std::vector;

CONTRACT logsomething : public contract
{
public:
    using contract::contract;
    logsomething(name receiver, name code, datastream<const char *> ds)
        : contract(receiver, code, ds),
          logs(receiver, receiver.value),
          dependencies(receiver, receiver.value),
          ids(receiver, receiver.value) {}

    const vector<name> smart_contract_dependency_expected_names = {name("eulaeulaeula")};

    ACTION createlog(name who, string what);
    ACTION deletelog(uint64_t log_id, name who);
    ACTION setdep(vector<name> smart_contract_dependency_actual_names);

    uint64_t get_next_id();
    name get_dependency_name(int index);

private:
    TABLE log
    {
        uint64_t id;
        name who;
        string what;
        time_point_sec when;
        uint64_t primary_key() const { return id; }
        uint64_t by_who() const { return who.value; }
    };
    typedef multi_index<name("log"), log, indexed_by<name("bywho"), const_mem_fun<log, uint64_t, &log::by_who>>> log_index;
    log_index logs;

    TABLE dependency
    {
        vector<name> expected;
        vector<name> actual;
    };
    typedef singleton<name("dependency"), dependency> dependency_index;
    dependency_index dependencies;

    struct id
    {
        uint64_t value;
    };
    typedef singleton<name("id"), id> id_index;
    id_index ids;
};