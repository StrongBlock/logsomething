# `logsomething`

## Overview
The purpose of the `logsomething` smart contract is to provide a service such that a user can log something to the blockchain by storing it in the smart contract's table.

## Parties Involved

### Logger

Logger refers to the party creating or deleting a log from the smart contract's table.

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
* log_id exists
* who is the account who created the log

#### Results

This action deletes a log with the specified id.

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