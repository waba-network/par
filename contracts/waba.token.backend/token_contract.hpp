#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

namespace waba {

    using eosio::asset;

    typedef enum contract_type : uint8_t {
        mutual_credit_system = 1
    } contract_type;

    typedef struct setting {

        uint64_t key;
        int64_t value;

        uint64_t primary_key() const { return key; }

    } setting;

    struct account {
        asset balance;
        bool frozen = false;
        int64_t overdraft = 0;
        int64_t issue_limit = 0;
        int64_t issued = 0;
        account_name referrer;
        std::vector<setting> settings;

        uint64_t primary_key() const { return balance.symbol.name(); }
    };

    typedef eosio::multi_index<N(accounts), account> accounts_table;

    class token_contract : public eosio::contract {
    public:
        token_contract(account_name self) : contract(self) {}

        virtual void validate_create(account_name issuer,
                            eosio::symbol_type symbol,
                            std::vector<setting> contract_settings) const = 0;

        virtual void initialize(account_name issuer,
                                     eosio::symbol_type symbol,
                                     std::vector<setting> contract_settings) const = 0;

        virtual void issue(account_name to, asset quantity) const = 0;

    };

}

