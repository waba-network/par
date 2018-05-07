#pragma once

#include <vector>
#include "mutual_credit_system_contract.hpp"

namespace waba {

    using std::vector;

    void mutual_credit_system_contract::create(account_name issuer,
                                               eosio::symbol_type symbol,
                                               vector<contract_setting> contract_settings) const {

        contract_settings_table contract_settings_table(_self, issuer);

        // TODO validate individual values
        for (contract_setting &contract_setting : contract_settings) {
            bool found = false;
            for (const auto key : all_settings) {
                if (contract_setting.key == key) {
                    found = true;
                    break;
                }
            }

            eosio_assert(found, "invalid contract setting key");

            contract_settings_table.emplace(contract_setting.key, [&](waba::contract_setting &to_update) {
                to_update.key = contract_setting.key;
                to_update.value = contract_setting.value;
            });

        }
    }

}/// namespace eosio
