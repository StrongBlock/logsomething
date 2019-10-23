#pragma once

#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

using namespace eosio;
using std::string;
using std::vector;

struct eulatable
{
    name id;
    uint16_t version;
    string content;
    time_point_sec timestamp;
    uint64_t primary_key() const { return id.value; }
};
typedef multi_index<"eulatable"_n, eulatable> eulatable_index;

struct signer
{
    name id;
    vector<name> signed_eula_ids;
    vector<uint16_t> signed_eula_versions;
    vector<time_point_sec> signed_eula_timestamps;
    uint64_t primary_key() const { return id.value; }
};
typedef multi_index<"signer"_n, signer> signer_index;

bool can_proceed(name eula_id, name signer, name actual_eula_account_name)
{
    eulatable_index eulatables(actual_eula_account_name, actual_eula_account_name.value);
    signer_index signers(actual_eula_account_name, actual_eula_account_name.value);
    auto e_itr = eulatables.find(eula_id.value);
    if (e_itr == eulatables.end())
    {
        return true;
    }
    auto s_itr = signers.find(signer.value);
    if (s_itr == signers.end())
    {
        return false;
    }
    vector<name> signed_eula_ids = s_itr->signed_eula_ids;
    vector<name>::iterator signed_eula_ids_itr = std::find(signed_eula_ids.begin(), signed_eula_ids.end(), eula_id);
    if (signed_eula_ids_itr == signed_eula_ids.end())
    {
        return false;
    }
    int index = std::distance(signed_eula_ids.begin(), signed_eula_ids_itr);
    if (s_itr->signed_eula_versions[index] == e_itr->version)
    {
        return true;
    }
    return false;
}
