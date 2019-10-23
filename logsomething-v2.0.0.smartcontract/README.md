# `logsomething`

## Overview
The purpose of the `logsomething` smart contract is to provide a service such that a user can log something to the blockchain by storing it in the smart contract's table.

## Parties Involved

### Logger

Logger refers to the party creating or deleting a log from the smart contract's table.

### Smart Contract Account

Smart Contract Account refers to the party who can set the dependency account names. This is the account the smart contract is set to.

## Functionalities

* a logger can create a log
* a logger can delete a log

## Actions

### `createlog`

#### Definition

`ACTION createlog(name who, string what);`

#### Parameters

`who`  
This is the account who is creating the log.

`what`  
This is the content being logged.

#### Checks

* who has signed logsomething's most recent EULA, if it exists
* who has appropriate keys for who account

#### Results

This action creates a log with the specified content.

### `deletelog`

#### Definition

`ACTION deletelog(uint64_t log_id, name who);`

#### Parameters

`log_id`  
This is the id of the log being deleted.

`who`  
This is the account who is deleting the log.

#### Checks

* who has appropriate keys for who account
* who has signed logsomething's most recent EULA, if it exists
* log_id exists
* who is the account who created the log

#### Results

This action deletes a log with the specified id.

### `setdep`

#### Definition

`ACTION setdep(vector<name> smart_contract_dependency_actual_names);`

#### Parameters

`smart_contract_dependency_actual_names`  
This is the id of the log being deleted.


#### Checks

* the Smart Contract Account has appropriate keys for the Smart Contract Account
* smart_contract_dependency_actual_names and smart_contract_dependency_expected_names (this is an internal const set by the developer) have the same length

#### Results

This action sets the actual names of the smart contract dependencies.

## Tables

### `log`

####  Definition

```
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
```

#### Parameters

`id`  
This is the id of the log table. This is the primary key for the row.

`who`  
This is the account who created the log.

`what`  
This is the content of the log.

`when`  
This is the time stamp of when the log was created.

### `dependency`

####  Definition

```
TABLE dependency
{
    vector<name> expected;
    vector<name> actual;
};
typedef singleton<name("dependency"), dependency> dependency_index;
```

#### Parameters

`expected`  
This is the expected list of account names for smart contract dependencies.

`actual`  
This is the actual list of account names for smart contract dependencies.