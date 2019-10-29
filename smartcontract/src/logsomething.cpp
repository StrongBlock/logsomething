#include <logsomething.hpp>

uint64_t logsomething::get_next_id()
{
   id next_id = {ids.exists() ? ids.get().value + 1 : 1};
   ids.set(next_id, get_self());
   return next_id.value;
}

name logsomething::get_dependency_name(int index)
{
   dependency d = dependencies.get();
   return d.actual.at(index);
}

void logsomething::createlog(name who, string what)
{
   require_auth(who);
   check(dependencies.exists(), "need to call setdep");
   string err_msg = "To continue, go back to your dashboard, click DApps and then click the View DApp button on the EULA card. While in the EULA DApp, look for " + get_self().to_string() + " in the Current EULAs table to view and sign " + get_self().to_string() + "'s EULA.";
   check(can_proceed(get_self(), who, get_dependency_name(0) /* eulaeulaeula*/), err_msg.c_str());
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
   check(dependencies.exists(), "need to call setdep");
   string err_msg = "To continue, go back to your dashboard, click DApps and then click the View DApp button on the EULA card. While in the EULA DApp, look for " + get_self().to_string() + " in the Current EULAs table to view and sign " + get_self().to_string() + "'s EULA.";
   check(can_proceed(get_self(), who, get_dependency_name(0) /* eulaeulaeula*/), err_msg.c_str());
   auto log_itr = logs.find(log_id);
   check(log_itr != logs.end(), "log does not exist");
   string error_msg = who.to_string() + " cannot delete this log";
   check(log_itr->who == who, error_msg.c_str());
   logs.erase(log_itr);
}

void logsomething::setdep(vector<name> smart_contract_dependency_actual_names)
{
   require_auth(get_self());
   check(smart_contract_dependency_expected_names.size() == smart_contract_dependency_actual_names.size(),
         "smart_contract_dependency_expected_names and smart_contract_dependency_actual_names lengths do not match");
   dependency d = {smart_contract_dependency_expected_names, smart_contract_dependency_actual_names};
   dependencies.set(d, get_self());
}