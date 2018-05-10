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

    enum mutual_credit_system_contract_setting : uint64_t {
        demurrage = N(demurrage),
        interest_rate = N(interestrate),
    };

    static const mutual_credit_system_contract_setting all_settings[] = {
            demurrage,
            interest_rate
    };

    class mutual_credit_system_contract : public token_contract {

    public:

        explicit mutual_credit_system_contract(account_name self) : token_contract(self) {}

        void validate_create(account_name issuer,
                    eosio::symbol_type symbol,
                    std::vector<setting> contract_settings) const override;

        void initialize(account_name issuer,
                       eosio::symbol_type symbol,
                       std::vector<setting> contract_settings) const override;

        void issue(account_name to, asset quantity) const override;

    };
}/// namespace eosio
