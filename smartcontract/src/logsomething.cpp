#include <logsomething.hpp>

uint64_t logsomething::get_next_id()
{
   id next_id{ids.exists() ? ids.get().value + 1 : 1};
   ids.set(next_id, get_self());
   return next_id.value;
}

void logsomething::createlog(name who, string what)
{
   require_auth(who);
   logs.emplace(get_self(), [&](auto &row) {
      row.id = get_next_id();
      row.who = who;
      row.what = what;
      row.when = time_point_sec(current_time_point());
   });
}

void logsomething::deletelog(uint64_t log_id, name who)
{
   require_auth(who);
   auto log_itr = logs.find(log_id);
   check(log_itr != logs.end(), "log does not exist");
   string error_msg = who.to_string() + " cannot delete this log";
   check(log_itr->who == who, error_msg.c_str());
   logs.erase(log_itr);
}
