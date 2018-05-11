#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>

#include <string>

#include "token_contract.hpp"
#include "mutual_credit_system_contract.hpp"

namespace waba {

    using std::string;
    using std::vector;
    using eosio::asset;
    using eosio::symbol_name;
    using eosio::symbol_type;

    typedef struct token_settings {
        asset supply;
        account_name owner;
        contract_type contract_type;
        vector<setting> contract_settings;

        uint64_t primary_key() const { return supply.symbol.name(); }
    } token_settings;

    class token : public eosio::contract {
    public:
        token(account_name self) : contract(self) {}

        void create(account_name owner,
                    symbol_type symbol,
                    contract_type type,
                    vector<setting> contract_settings);

        void issue(account_name to, asset quantity, vector<setting> settings, string memo);

        void setissuelimit(account_name to, asset limit, vector<setting> settings, string memo);

        void transfer(account_name from,
                      account_name to,
                      asset quantity,
                      string memo);

    private:

        inline asset get_supply(symbol_name sym) const;

        inline asset get_balance(account_name owner, symbol_name sym) const;

    private:

        typedef eosio::multi_index<N(token), token_settings> token_settings_table;

        void sub_balance(account_name owner, asset value, const token_settings &st);

        void add_balance(account_name owner, asset value, const token_settings &st, account_name ram_payer, account_name referrer);

    public:
        struct transfer_args {
            account_name from;
            account_name to;
            asset quantity;
            string memo;
        };

        const token_contract& get_contract(contract_type type);
    };

    asset token::get_supply(symbol_name sym) const {
        token_settings_table token_settings_table(_self, sym);
        const auto &st = token_settings_table.get(sym);
        return st.supply;
    }

    asset token::get_balance(account_name owner, symbol_name sym) const {
        accounts_table accountstable(_self, owner);
        const auto &ac = accountstable.get(sym);
        return ac.balance;
    }

} /// namespace eosio
