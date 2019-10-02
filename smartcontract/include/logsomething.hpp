#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <string>

using namespace eosio;
using std::string;

CONTRACT logsomething : public contract
{
public:
    using contract::contract;
    logsomething(name receiver, name code, datastream<const char *> ds)
        : contract(receiver, code, ds),
          logs(receiver, receiver.value),
          ids(receiver, receiver.value) {}

    ACTION createlog(name who, string what);
    ACTION deletelog(uint64_t log_id, name who);
    uint64_t get_next_id();

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

    struct id
    {
        uint64_t value;
    };
    typedef singleton<name("id"), id> id_index;
    id_index ids;
};