#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

#include <string>
#include <vector>

#include "token_contract.hpp"

namespace waba {

    using std::string;
    using std::vector;
    using eosio::asset;
    using eosio::symbol_name;
    using eosio::symbol_type;

    enum account_type {
        consumer = 1,
        self_employed = 2,
        micro_company = 3,
        small_company = 4,
        medium_company = 5
    };

    enum mutual_credit_system_contract_setting : uint64_t {
        demurrage = N(demurrage),
        interest_rate = N(interestrate),

        credit_limit_initial_consumer = N(intialconsumer),
        credit_limit_initial_self_employed = N(intialselfemployed),
        credit_limit_initial_micro_company = N(intialmicrocompany),
        credit_limit_initial_small_company = N(intialsmallcompany),
        credit_limit_initial_medium_company = N(intialmediumcompany),

        credit_limit_max_consumer = N(maxconsumer),
        credit_limit_max_self_employed = N(maxselfemployed),
        credit_limit_max_micro_company = N(maxmicrocompany),
        credit_limit_max_small_company = N(maxsmallcompany),
        credit_limit_max_medium_company = N(maxmediumcompany)
    };

    static const mutual_credit_system_contract_setting all_settings[] = {
            demurrage,
            interest_rate,
            credit_limit_initial_self_employed,
            credit_limit_initial_micro_company,
            credit_limit_initial_small_company,
            credit_limit_initial_medium_company,
            credit_limit_max_self_employed,
            credit_limit_max_micro_company,
            credit_limit_max_small_company,
            credit_limit_max_medium_company
    };

    class mutual_credit_system_contract : public token_contract {

    public:

        mutual_credit_system_contract(account_name self) : token_contract(self) {}

        void create(account_name issuer,
                    eosio::symbol_type symbol,
                    vector<contract_setting> contract_settings) const;

        void issue(account_name to, account_type type, asset quantity, string memo);

        void transfer(account_name from,
                      account_name to,
                      asset quantity,
                      string memo);


    };
}/// namespace eosio
